#include "stdafx.h"
#include "AVPlayer.h"
#include "AudioDec.h"
#include "VideoDec.h"


AVPlayer::AVPlayer()
{
}


AVPlayer::~AVPlayer()
{
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

	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{

		}
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{

		}
	}
	return true;
}
