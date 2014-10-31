#include "stdafx.h"
#include "VideoDec.h"


VideoDec::VideoDec()
: stream_index(-1)
, pStream(NULL)
, pSwsContext(NULL)
, pFrame(NULL)
, pBgrFrame(NULL)
, video_queue(10)
, max_queue_size(10)
, picture_queue_size(0)
, picture_queue_write_index(0)
, picture_queue_read_index(0)
{
}


VideoDec::~VideoDec()
{
}

bool VideoDec::stream_open(int index, AVFormatContext* pFormatCtx)
{
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
	width = pCodecCtx->width;
	height = pCodecCtx->height;
	
	// 영상 변환 컨텍스트 
	pSwsContext = getSwsContext(pStream->codec, width, height, AV_PIX_FMT_RGB565);
	

	if (pSwsContext == NULL)
	{
		return false;
	}

	/* 영상 버퍼 할당 부분 */
	pFrame = av_frame_alloc();
	pBgrFrame = av_frame_alloc();
	if (!pFrame || !pBgrFrame)
	{
		return false;
	}

	int numBytes = avpicture_get_size(AV_PIX_FMT_RGB565, width, height);
	uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	if (avpicture_fill((AVPicture*)pBgrFrame, buffer, AV_PIX_FMT_RGB565, width, height) < 0)
	{
		return false;
	}

	/* 스레드 설정 */
	// thread start!
	video_thread = std::thread(&VideoDec::video_thread_handler, this);

	return true;
}

void VideoDec::video_thread_handler()
{
	AVPacket packet;
	av_init_packet(&packet);
	std::chrono::milliseconds time_out(10);
	for (;;)
	{
		if (!video_queue.pop(packet, time_out))
		{
			break;
		}
		int got_frame;

		avcodec_decode_video2(pStream->codec, pFrame, &got_frame, &packet);
		
		if (got_frame)
		{
			if (queue_picture(pFrame) < 0)
			{
				break;
			}
		}
		av_free_packet(&packet);
	}
	av_free(pFrame);
}

bool VideoDec::queue_picture(AVFrame* frame)
{
	VideoPicture* vp;
	std::unique_lock <std::mutex> lock(queue_mutex);
	while (picture_queue_size >= VIDEO_PICTURE_QUEUE_SIZE)
	{
		cond.wait(lock);
	}
	lock.unlock();


	vp = &pictq[picture_queue_write_index];
	/* 버퍼를 생성 */
	if (!vp->data)
	{
		vp->allocated = 0;
		allocate_picture();

		lock.lock();
		while (!vp->allocated)
		{
			cond.wait(lock);
		}
		lock.unlock();
	}
	if (vp->data)
	{
		AVFrame* pict = vp->data;

		sws_scale(pSwsContext, pFrame->data, pFrame->linesize, 0, height, pict->data, pict->linesize);
		
		if (++picture_queue_write_index == VIDEO_PICTURE_QUEUE_SIZE)
		{
			picture_queue_write_index = 0;
		}
		lock.lock();
		picture_queue_size++;
		lock.unlock();
	}
	return true;
}

SwsContext* VideoDec::getSwsContext(AVCodecContext* pCodecCtx, int width, int height, AVPixelFormat fmt)
{
	SwsContext* pSwsCtx = sws_getCachedContext(pSwsContext,
		pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		width,
		height,
		fmt,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL);

	if (!pSwsCtx)
	{
		return NULL;
	}

	return pSwsCtx;
}

void VideoDec::allocate_picture()
{
	VideoPicture* vp;

	vp = &pictq[picture_queue_write_index];

	if (vp->data)
	{
		av_free(vp->data);
	}

	AVFrame* picture = av_frame_alloc();
	int numBytes = avpicture_get_size(AV_PIX_FMT_BGR24, width, height);
	uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	avpicture_fill((AVPicture*)picture, buffer, AV_PIX_FMT_BGR24, width, height);

	vp->data = picture;
	vp->width = width;
	vp->height = height;

	std::unique_lock<std::mutex> lock(queue_mutex);
	vp->allocated = 1;
	cond.notify_all();
	lock.unlock();
}

void VideoDec::video_display()
{
	VideoPicture *vp;
	vp = &pictq[picture_queue_read_index];
	if (vp->data)
	{
		if (handler != NULL)
		{
			handler(this, (void**)vp->data->data);
		}
	}
}




