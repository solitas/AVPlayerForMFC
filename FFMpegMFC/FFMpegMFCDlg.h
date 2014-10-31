
// FFMpegMFCDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "AVPlayer.h"

// CFFMpegMFCDlg ��ȭ ����
class CFFMpegMFCDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CFFMpegMFCDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FFMPEGMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picCtrl;
	afx_msg void OnBnClickedButtonOpen();

	AVPlayer m_player;
};
