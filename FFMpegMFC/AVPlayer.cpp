#include "stdafx.h"
#include "AVPlayer.h"
#include "AudioDec.h"
#include "VideoDec.h"


AVPlayer::AVPlayer()
: pFormatCtx(NULL)
, video_stream_index(0)
, audio_stream_index(0)
, bRun(false)
{
}


AVPlayer::~AVPlayer()
{
	bRun = true;

	if (pFormatCtx)
	{
		avformat_close_input(&pFormatCtx);
	}
}

bool AVPlayer::play_video(const char* resourceName)
{
	

	av_register_all();

	AVFormatContext* pFormatCtx = NULL;

	if (avformat_open_input(&pFormatCtx, resourceName, NULL, NULL) < 0)
	{
		TRACE("format context open error");
		return false;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		TRACE("find stream info error");
		return false;
	}

	for (int i = 0; i < (int)pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = i;
		}
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = i;
		}
	}
	
	AudioDec aDecoder;
	VideoDec vDecoder;

	if (audio_stream_index >= 0)
	{
		if (aDecoder.stream_open(audio_stream_index, pFormatCtx))
		{

		}
	}

	if (video_stream_index >= 0)
	{
		if (vDecoder.stream_open(video_stream_index, pFormatCtx))
		{

		}
	}

	/* decoding thread start */
	bRun = true;
	decode_thread = std::thread(&AVPlayer::decode_proc, this);

	return true;
}

void AVPlayer::decode_proc()
{
	while (bRun)
	{
		AVPacket packet;
		av_init_packet(&packet);
		
		/* 영상 내 프레임을 읽음 */
		while (av_read_frame(pFormatCtx, &packet) >= 0)
		{
			int stream_index = packet.stream_index;
			if (stream_index == video_stream_index)
			{
				/* process audio packet */
			}
			else if (stream_index == audio_stream_index)
			{
				/* process video packet */
			}
			else
			{
				av_free_packet(&packet);
			}
		}

		bRun = false;
	}
}

