#pragma once
#include "VideoPicture.h"

#define VIDEO_PICTURE_QUEUE_SIZE 1

class VideoDec
{
public:
	explicit VideoDec();
	virtual ~VideoDec();

	bool stream_open(int video_stream_index, AVFormatContext* pFormatCtx);
	void video_display();
public:
	int stream_index; 
	int width;
	int height;
	int max_queue_size;
	BlockingQueue<AVPacket>	video_queue;
	
	int picture_queue_size;

	int picture_queue_write_index;

	int picture_queue_read_index;

	std::mutex		queue_mutex;

	std::condition_variable cond;

	DisplayCallback handler;
private:

	AVStream*		pStream;

	SwsContext*		pSwsContext;

	AVFrame*		pFrame;

	AVFrame*		pBgrFrame;

	VideoPicture    pictq[VIDEO_PICTURE_QUEUE_SIZE];

	std::thread		video_thread;
private:
	SwsContext* getSwsContext(AVCodecContext* pCodecCtx, int width, int height, AVPixelFormat fmt);
	void		video_thread_handler();
	bool		queue_picture(AVFrame* frame);
	void		allocate_picture();
	
};

