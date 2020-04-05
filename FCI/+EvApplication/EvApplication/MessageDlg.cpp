// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MessageDlg.h"
#include "afxdialogex.h"


// CMessageDlg dialog

IMPLEMENT_DYNAMIC(CMessageDlg, CDialogEx)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMessageDlg::IDD, pParent)
{

}

CMessageDlg::~CMessageDlg()
{
	m_Brush.DeleteObject();
	m_fontList.DeleteObject();
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMessageDlg message handlers


BOOL CMessageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CRect rctDesktop;
	SystemParametersInfo(SPI_GETWORKAREA,NULL, &rctDesktop, NULL);
	m_fontList.CreateFont((int) (rctDesktop.Width() / 1280.0 * 18), 0, 0, 0, 350, FALSE, FALSE, FALSE, 
						ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
						PROOF_QUALITY, FIXED_PITCH|FF_MODERN, "Segoe UI Semibold");
	
	GetDlgItem(IDC_STATIC_MSG_INFO)->SetFont(&m_fontList);
	m_Brush.CreateSolidBrush(RGB(255, 0, 0));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CMessageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	//return hbr;
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_MSG_INFO) {
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
	}
	return m_Brush;
}