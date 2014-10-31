#pragma once
class AudioDec
{
public:
	AudioDec();
	~AudioDec();

	bool		stream_open(int audio_stream_index, AVFormatContext* pFormatCtx); /* 오디오 코덱 디코딩 정보를 설정 */
		
	//void		push_packet(AVPacket packet);	/* 패킷 삽입 */

	void		audio_decode(AVPacket *packet);	/* 패킷 디코드 */

	double		get_audio_clock();				/* 오디오 클럭 반환 */

public:
	int stream_index;			// audio stream index
	double audio_clock;			// audio clock
	
private:
	AVStream*			pStream;		// audio stream
	SwrContext*			pSwrContext;	// swsContext
	AVFrame* pFrame;
	BlockingQueue<AVPacket> packet_queue;

	WAVEFORMATEX		wfx;			// wave format context
	HWAVEOUT			hWaveOut;		
	WAVEHDR*			waveBlocks;

	int					out_buffer_sz;	// buffer size
	int					waveCurrentBlock; // current block index
	int					nb_out_samples;
private:
	void		set_audio_spec(AVCodecContext* pCodecCtx);
	WAVEHDR*	audio_allocate_block(int size, int count);
	void		audio_block_free(WAVEHDR* blockArray);
	void		audio_write(HWAVEOUT hWaveOut, LPSTR data, int size);
	void		audio_callback_func(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);


	static void CALLBACK audio_callback_func_static(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

