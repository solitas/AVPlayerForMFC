#pragma once
class AudioDec
{
public:
	AudioDec();
	~AudioDec();
	bool stream_open(int audio_stream_index, AVFormatContext* pFormatCtx);
};

