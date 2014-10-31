#pragma once
#include "OpenGLDevice.h"

static void* pData;



class COpenGLControl : public CWnd
{
	DECLARE_DYNAMIC(COpenGLControl)

public:
	COpenGLControl();
	virtual ~COpenGLControl();

	int width, height;
protected:
	DECLARE_MESSAGE_MAP()
public:
	void Create(CRect rect, CWnd* parent);
protected:
	void InitGL(void);
	void DrawGLScene(void);
	OpenGLDevice openGLDevice;
	CClientDC* dc;
	float rotation;
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	static void decode_callback(void* obj, void** ptr)
	{
		reinterpret_cast<COpenGLControl*>(obj)->decode(obj, ptr);
	}
	void decode(void* obj, void** ptr);

	CRITICAL_SECTION  waveCriticalSection2;

};


