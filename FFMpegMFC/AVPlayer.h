#pragma once
#include "AudioDec.h"
#include "VideoDec.h"

class AVPlayer
{
public:
	AVPlayer();
	virtual ~AVPlayer();

	int video_stream_index;
	int audio_stream_index;

	bool play_video(const char* resourceName);
	bool stop_video();
	void video_refresh_timer();

	AudioDec aDecoder;
	VideoDec vDecoder;

	Callback refresh_timer;

private:
	bool bRun;

	AVFormatContext* pFormatCtx;

	std::thread decode_thread;

	void decode_proc();

	void refresh_timer_callback(void* obj, int delay);
	static  Uint32 refresh_time_static(Uint32 interval, void *opaque)
	{
		reinterpret_cast<AVPlayer*>(opaque)->video_refresh_timer();
		return 0;
	}
};

