#include "stdafx.h"
#include "AVPlayer.h"



AVPlayer::AVPlayer()
: pFormatCtx(NULL)
, video_stream_index(0)
, audio_stream_index(0)
, bRun(false)
{
	
}


AVPlayer::~AVPlayer()
{
	
}

bool AVPlayer::play_video(const char* resourceName)
{
	av_register_all();
	
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
	//refresh_timer(this, 40);
	refresh_timer_callback(this, 40);
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
		std::chrono::milliseconds time_out(10);
		/* 영상 내 프레임을 읽음 */
		while (av_read_frame(pFormatCtx, &packet) >= 0)
		{
// 			int queue_size = vDecoder.video_queue.size();
// 			if ( queue_size >= vDecoder.max_queue_size)
// 			{
// 				Sleep(10);
// 				continue;
// 			}

			int stream_index = packet.stream_index;
			if (stream_index == video_stream_index)
			{
				/* process audio packet */
				vDecoder.video_queue.push(packet, time_out);
			}
			else if (stream_index == audio_stream_index)
			{
				/* process video packet */
				aDecoder.audio_decode(&packet);
			}
			else
			{
				av_free_packet(&packet);
			}
		}

		stop_video();
	}
}

bool AVPlayer::stop_video()
{
	bRun = false;

	if (pFormatCtx)
	{
		avformat_close_input(&pFormatCtx);
	}

	if (decode_thread.joinable())
	{
		decode_thread.join();
	}
	return true;
}

void AVPlayer::video_refresh_timer()
{
	if (vDecoder.stream_index >= 0) {
		if (vDecoder.picture_queue_size == 0) {
			refresh_timer_callback(this, 1);
		}
		else {
			//vp = &is->pictq[is->pictq_rindex];
			/* Now, normally here goes a ton of code
			about timing, etc. we're just going to
			guess at a delay for now. You can
			increase and decrease this value and hard code
			the timing - but I don't suggest that ;)
			We'll learn how to do it for real later.
			*/
			refresh_timer_callback(this, 80);

			/* show the picture! */
			vDecoder.video_display();

			/* update queue for next picture! */
			if (++vDecoder.picture_queue_read_index == VIDEO_PICTURE_QUEUE_SIZE) {
				vDecoder.picture_queue_read_index = 0;
			}
			std::unique_lock <std::mutex> lock(vDecoder.queue_mutex);
			vDecoder.picture_queue_size--;
			vDecoder.cond.notify_all();
			lock.unlock();
		}
	}
	else {
		refresh_timer(this, 100);
	}
}

void AVPlayer::refresh_timer_callback(void* obj, int delay)
{
	/* 타이머 초기화 */
	SDL_AddTimer(delay, refresh_time_static, this);
}

