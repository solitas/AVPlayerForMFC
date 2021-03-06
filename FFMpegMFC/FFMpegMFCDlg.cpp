
// FFMpegMFCDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FFMpegMFC.h"
#include "FFMpegMFCDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFFMpegMFCDlg 대화 상자



CFFMpegMFCDlg::CFFMpegMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFFMpegMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFFMpegMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CFFMpegMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CFFMpegMFCDlg::OnBnClickedButtonOpen)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CFFMpegMFCDlg 메시지 처리기

BOOL CFFMpegMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	GetDlgItem(IDC_OPENGL_WINDOW)->GetWindowRect(rect);
	ScreenToClient(rect);
	//GetWindowRect(rect);
	openGLControl.Create(rect, this);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFFMpegMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFFMpegMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (pVideoData != NULL)
		{
// 			CDC* pDC;
// 			int width = m_player.vDecoder.width;
// 			int height = m_player.vDecoder.height;
// 			if (bmi == NULL)
// 			{
// 				int width = m_player.vDecoder.width;
// 				int height = m_player.vDecoder.height;
// 				bmi = (BITMAPINFO*)new char[sizeof(BITMAPINFO)+sizeof(RGBQUAD)* 256];
// 				bmi->bmiHeader.biBitCount = 24;
// 				bmi->bmiHeader.biCompression = BI_RGB;
// 				bmi->bmiHeader.biHeight = -height;
// 				bmi->bmiHeader.biWidth = width;
// 				bmi->bmiHeader.biPlanes = 1;
// 				bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
// 				bmi->bmiHeader.biSizeImage = height * width * 3;
// 			}
// 			pDC = GetDlgItem(IDC_STATIC_PIC_CTRL)->GetDC();
// 			pDC->SetStretchBltMode(COLORONCOLOR);
// 			m_picCtrl.GetClientRect(&rect);
// 			StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.right, rect.bottom, 0, 0, width, height, pVideoData, bmi, DIB_RGB_COLORS, SRCCOPY);
// 			ReleaseDC(pDC);
		}
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFFMpegMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFFMpegMFCDlg::OnBnClickedButtonOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szFilter[] = "Video File(*.avi, *.mp4)|*.avi;*.mp4|Audio File(*.mp3,*.wav)|*.mp3;*.wav|All File(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		const char* file_name = ((LPCSTR)filePath);
		
		if (m_player.play_video(file_name))
		{
			openGLControl.width = m_player.vDecoder.width;
			openGLControl.height = m_player.vDecoder.height;
			m_player.vDecoder.handler = openGLControl.decode_callback;
		}

		SetTimer(0, 1, NULL);
		
	}
}


void CFFMpegMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (pVideoData != NULL)
	{
		//Invalidate(FALSE);
	}
	CDialogEx::OnTimer(nIDEvent);
}
