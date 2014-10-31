#pragma once
class VideoPicture
{
public:
	VideoPicture();
	~VideoPicture();

	AVFrame* data;
	int width, height; /* source height & width */
	int allocated;
	double pts;
};

