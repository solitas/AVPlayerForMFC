
// FFMpegMFC.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFFMpegMFCApp:
// �� Ŭ������ ������ ���ؼ��� FFMpegMFC.cpp�� �����Ͻʽÿ�.
//

class CFFMpegMFCApp : public CWinApp
{
public:
	CFFMpegMFCApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFFMpegMFCApp theApp;