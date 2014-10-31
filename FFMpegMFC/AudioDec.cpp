#include "stdafx.h"
#include "AudioDec.h"

#define QUEUE_SIZE 10
#define AUDIO_BLOCK_COUNT  20
#define AUDIO_BLOCK_SIZE  4096 
#define MAX_AUDIO_FRAME_SIZE 192000

CRITICAL_SECTION  waveCriticalSection;
int waveFreeBlockCount = AUDIO_BLOCK_COUNT;

AudioDec::AudioDec()
: pStream(NULL)
, pSwrContext(NULL)
, stream_index(-1)
, packet_queue(QUEUE_SIZE)
, hWaveOut(NULL)
, waveBlocks(NULL)
, waveCurrentBlock(0)
, audio_clock(0.0)
, pFrame(NULL)
, out_buffer_sz(0)
{
}


AudioDec::~AudioDec()
{
	if (pStream != NULL)
	{
		av_free(pStream);
	}
	if (hWaveOut)
		audio_block_free(waveBlocks);

	DeleteCriticalSection(&waveCriticalSection);
}

void	CALLBACK AudioDec::audio_callback_func_static(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	/*
	* ignore calls that occur due to opening and closing the
	* device.
	*/
	reinterpret_cast<AudioDec*>(dwParam1)->audio_callback_func(hWaveOut, uMsg, dwInstance, dwParam1, dwParam2);
}

bool AudioDec::stream_open(int index, AVFormatContext* pFormatCtx)
{
	InitializeCriticalSection(&waveCriticalSection);
	stream_index = index;

	pStream = pFormatCtx->streams[index];

	AVCodecContext* pCodecCtx = pFormatCtx->streams[index]->codec;
	AVCodec* pCodec;

	if ((pCodec = avcodec_find_decoder(pCodecCtx->codec_id)) == NULL)
	{
		TRACE("err avcodec_find_decoder \n");
		return false;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		TRACE("err avcodec_open2 \n");
		return false;
	}

	// 오디오 스펙 초기화
	set_audio_spec(pCodecCtx);

	pFrame = av_frame_alloc();
	// 오디오 블록 할당
	waveBlocks = audio_allocate_block(AUDIO_BLOCK_SIZE, AUDIO_BLOCK_COUNT);
	nb_out_samples = 1024;
	return true;
}

void AudioDec::audio_decode(AVPacket *packet)
{
	if (packet->pts != AV_NOPTS_VALUE)
	{
		audio_clock = av_q2d(pStream->time_base)*packet->pts; // 오디오 클럭 초기화
	}

	int got_picture;
	out_buffer_sz = av_samples_get_buffer_size(NULL, av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO), nb_out_samples, AV_SAMPLE_FMT_S16, 0);
	uint8_t* buffer = (uint8_t*)av_malloc(out_buffer_sz);

	/* 오디오 프레임 디코딩 */
	if (avcodec_decode_audio4(pStream->codec, pFrame, &got_picture, packet) < 0)
	{
		TRACE("audio codec decode error! \n");
		return;
	}
	if (got_picture)
	{
		// 프레임이 디코딩 되면 지정한 형태로 변환 , 샘플 갯수 획득
		int numSamplesOut = swr_convert(pSwrContext, &buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t**)pFrame->data, pFrame->nb_samples);

		if (numSamplesOut > 0)
		{
			if (numSamplesOut != nb_out_samples)
			{
				nb_out_samples = numSamplesOut;
			}
			// 사이즈 다시 계산
			out_buffer_sz = av_samples_get_buffer_size(NULL, av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO), numSamplesOut, AV_SAMPLE_FMT_S16, 0);
			
			/* 오디오 클럭 계산 */
			//int pts = audio_clock;
			int n = 2 * (pStream->codec->channels);
			audio_clock += (double)out_buffer_sz / (double)(n * pStream->codec->sample_rate);
			/* 오디오 출력  */
			audio_write(hWaveOut, (LPSTR)(unsigned char*)buffer, out_buffer_sz);
		}
	}
}

double AudioDec::get_audio_clock()
{
	double pts;
	int hw_buf_size, bytes_per_sec;
	pts = audio_clock;
	hw_buf_size = out_buffer_sz;
	bytes_per_sec = 0;

	if (pStream)
	{
		bytes_per_sec = pStream->codec->sample_rate * pStream->codec->channels * 2;
	}
	if (bytes_per_sec)
	{
		pts -= (double)hw_buf_size / bytes_per_sec;
	}
	return pts;
}

void AudioDec::audio_callback_func(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg != WOM_DONE)
		return;

	EnterCriticalSection(&waveCriticalSection);
	waveFreeBlockCount++;
	LeaveCriticalSection(&waveCriticalSection);
}

WAVEHDR* AudioDec::audio_allocate_block(int size, int count)
{
	uint8_t* buffer;
	WAVEHDR* blocks;
	DWORD totalBufferSz = (size + sizeof(WAVEHDR)) * count;
	/*
	* allocate memory for the entire set in one go
	*/
	if ((buffer = (uint8_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSz)) == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		ExitProcess(1);
	}
	/*
	* and set up the pointers to each bit
	*/
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR)* count;
	for (int i = 0; i < count; i++) {
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (LPSTR)(unsigned char*)buffer;
		buffer += size;
	}
	return blocks;
}

void AudioDec::audio_block_free(WAVEHDR* blockArray)
{

}

void AudioDec::audio_write(HWAVEOUT hWaveOut, LPSTR data, int size)
{
	WAVEHDR* current;
	int remain = 0;

	current = &waveBlocks[waveCurrentBlock];
	while (size > 0) {
		/*
		* first make sure the header we're going to use is unprepared
		*/
		if (current->dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

		if (size < (int)(AUDIO_BLOCK_SIZE - current->dwUser)) {
			memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			break;
		}
		remain = AUDIO_BLOCK_SIZE - current->dwUser;
		memcpy(current->lpData + current->dwUser, data, remain);
		size -= remain;
		data += remain;
		current->dwBufferLength = AUDIO_BLOCK_SIZE;

		waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));

		EnterCriticalSection(&waveCriticalSection);
		waveFreeBlockCount--;
		LeaveCriticalSection(&waveCriticalSection);

		/*
		* wait for a block to become free
		*/
		while (!waveFreeBlockCount)
			Sleep(10);

		/*
		* point to the next block
		*/
		waveCurrentBlock++;
		waveCurrentBlock %= AUDIO_BLOCK_COUNT;

		current = &waveBlocks[waveCurrentBlock];
		current->dwUser = 0;
	}
}

void AudioDec::set_audio_spec(AVCodecContext* pCodecCtx)
{
	/* 오디오 출력을 위한 구조체 설정 파트 */
	/* sample rate */
	wfx.nSamplesPerSec = pCodecCtx->sample_rate;

	/* number of bits per sample of mono data */
	switch (pCodecCtx->sample_fmt)
	{
	case AV_SAMPLE_FMT_U8: wfx.wBitsPerSample = 8; break;
	case AV_SAMPLE_FMT_S16: wfx.wBitsPerSample = 16; break;
	case AV_SAMPLE_FMT_S32: wfx.wBitsPerSample = 32; break;
	case AV_SAMPLE_FMT_FLT: wfx.wBitsPerSample = sizeof(double)* 8; break;
	case AV_SAMPLE_FMT_FLTP: wfx.wBitsPerSample = 16; break;
	default: wfx.wBitsPerSample = 0; break;
	}
	wfx.wBitsPerSample = 16;
	wfx.nChannels = FFMIN(2, pCodecCtx->channels);

	/* the count in bytes of the size of extra information (after cbSize) */
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

	MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD)audio_callback_func_static, (DWORD)&waveFreeBlockCount, CALLBACK_FUNCTION);
	
	if (result != MMSYSERR_NOERROR)
	{
		// 오디오 출력장치를 열지 못했다...
		TRACE("err waveOutOpen \n");
		return;
	}

	pSwrContext = swr_alloc_set_opts(
		NULL, AV_CH_LAYOUT_STEREO,
		AV_SAMPLE_FMT_S16,
		pCodecCtx->sample_rate,
		wfx.nChannels,
		pCodecCtx->sample_fmt,
		pCodecCtx->sample_rate,
		0, NULL);
	swr_init(pSwrContext);
}