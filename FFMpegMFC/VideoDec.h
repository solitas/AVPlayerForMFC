#pragma once
class VideoDec
{
public:
	VideoDec();
	~VideoDec();
	bool stream_open(int video_stream_index, AVFormatContext* pFormatCtx);
};

