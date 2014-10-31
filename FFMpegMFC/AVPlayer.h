#pragma once
class AVPlayer
{
public:
	AVPlayer();
	virtual ~AVPlayer();

	bool play_video(const char* resourceName);
};

