// StepSelection.cpp : implementation file
//

#include "stdafx.h"
#include "StepSelection.h"
#include "Application.h"
#include "afxdialogex.h"


// CStepSelection dialog

IMPLEMENT_DYNAMIC(CStepSelection, CDialogEx)

CStepSelection::CStepSelection(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStepSelection::IDD, pParent)
{

}

CStepSelection::~CStepSelection()
{
}

void CStepSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStepSelection, CDialogEx)
//	ON_WM_INITMENU()
	ON_BN_CLICKED(IDOK, &CStepSelection::OnBnClickedOk)
END_MESSAGE_MAP()


// CStepSelection message handlers


//void CStepSelection::OnInitMenu(CMenu* pMenu)
//{
//	CDialogEx::OnInitMenu(pMenu);
//
//	// TODO: Add your message handler code here
//}


BOOL CStepSelection::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CApplication	*m_pApp = (CApplication*) AfxGetApp();
	
	for(int i=0;i<THREAD_COUNT;i++){
		GetDlgItem(IDC_CHECK_SELECTION1+i)->EnableWindow(m_pApp->m_bStepSelectionEnable[i]);
		if(m_pApp->m_bStepSelectionEnable[i]){
			CheckDlgButton(IDC_CHECK_SELECTION1+i,m_pApp->m_bStepSelection[i]);
		}
		else
			m_pApp->m_bStepSelection[i] = FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CStepSelection::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CApplication	*m_pApp = (CApplication*) AfxGetApp();

	for(int i=0;i<15;i++)
		m_pApp->m_bStepSelection[i] = IsDlgButtonChecked(IDC_CHECK_SELECTION1+i);

	CDialogEx::OnOK();
}
