#include "resource.h"

#pragma once


class CApplication;

// CIOTestDialog dialog

class CIOTestDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CIOTestDialog)

public:
	CIOTestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIOTestDialog();

	int m_nCounter;
	int m_nNoOfSegments;
	unsigned int m_nIconStatus[8];//0: Black, 1: Yellow	, 2: Green, 3: Red
	CRgn m_prgnLightSegment[8];
	CBrush m_brushGreen;
	CBrush m_brushRed;
	CBrush m_brushBlack;
	CBrush m_brushYellow;

	CApplication* m_pApp;

// Dialog Data
	enum { IDD = IDD_HARDWAREIOTEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
