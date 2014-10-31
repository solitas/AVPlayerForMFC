#pragma once
class OpenGLDevice
{
public:
	OpenGLDevice(void);
	virtual ~OpenGLDevice(void);
	OpenGLDevice(HDC& deviceContext, int stencil = 0);
	OpenGLDevice(HWND& window, int stencil = 0);
	bool create(HDC& deviceContext, int stencil = 0);
	bool create(HWND& window, int stencil = 0);
	void destroy(void);
	void makeCurrent(bool disableOther = true);
protected:
	bool setDCPixelFormat(HDC& deviceContext, int stencil);
	HGLRC renderContext;
	HDC deviceContext;
};

