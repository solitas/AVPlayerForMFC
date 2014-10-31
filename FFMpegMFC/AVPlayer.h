#pragma once
class AVPlayer
{
public:
	AVPlayer();
	virtual ~AVPlayer();

	int video_stream_index;
	int audio_stream_index;

	bool play_video(const char* resourceName);

private:
	bool bRun;

	AVFormatContext* pFormatCtx;

	std::thread decode_thread;

	void decode_proc();
};

