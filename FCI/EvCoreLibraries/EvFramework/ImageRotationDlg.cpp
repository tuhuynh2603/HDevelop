// ImgRotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageRotationDlg.h"
#include "EvImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageRotationDlg dialog


CImageRotationDlg::CImageRotationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageRotationDlg::IDD, pParent)
{
	m_dAngleDeg = 0;
	//{{AFX_DATA_INIT(CImageRotationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CImageRotationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageRotationDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImageRotationDlg, CDialog)
	//{{AFX_MSG_MAP(CImageRotationDlg)
	ON_BN_CLICKED(IDC_ROT_CCW, OnRotCCW)
	ON_BN_CLICKED(IDC_ROT_CW, OnRotCW)
	ON_BN_CLICKED(IDC_ROT_CCW2, OnRotCcw2)
	ON_BN_CLICKED(IDC_ROT_CW2, OnRotCw2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageRotationDlg message handlers

void CImageRotationDlg::OnRotCCW() 
{
	m_dAngleDeg -= 1.0;
	m_pView->PostMessage(WM_IMG_ROT_CCW,(int)(m_dAngleDeg*10),0);
}

void CImageRotationDlg::OnRotCcw2() 
{
	m_dAngleDeg -= 0.1;
	m_pView->PostMessage(WM_IMG_ROT_CCW,(int)(m_dAngleDeg*10),0);
}

void CImageRotationDlg::OnRotCW() 
{
	m_dAngleDeg += 1.0;
	m_pView->PostMessage(WM_IMG_ROT_CW,(int)(m_dAngleDeg*10),0);
}

void CImageRotationDlg::OnRotCw2() 
{
	m_dAngleDeg += 0.1;
	m_pView->PostMessage(WM_IMG_ROT_CW,(int)(m_dAngleDeg*10),0);
}

double CImageRotationDlg::GetAngle()
{
	return (m_dAngleDeg * PI/180);
}