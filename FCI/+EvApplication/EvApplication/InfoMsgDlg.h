#pragma once

#include "Resource.h"
#include "afxwin.h"

// CInfoMsgDlg dialog

class CInfoMsgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInfoMsgDlg)

public:
	CInfoMsgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoMsgDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INFO_MESSAGE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CFont m_fontHead;
	CFont m_fontContent;
	CBrush m_Brush;

	void UpdateDlgItems(CString strCamDocName, CString strInfoMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
