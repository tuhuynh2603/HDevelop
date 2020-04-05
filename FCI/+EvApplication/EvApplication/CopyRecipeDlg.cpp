// CopyRecipeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "CopyRecipeDlg.h"
#include "afxdialogex.h"


// CCopyRecipeDlg dialog

IMPLEMENT_DYNAMIC(CCopyRecipeDlg, CDialogEx)

CCopyRecipeDlg::CCopyRecipeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCopyRecipeDlg::IDD, pParent)
	, m_strNewRecipeName(_T(""))
{

}

CCopyRecipeDlg::~CCopyRecipeDlg()
{
}

void CCopyRecipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PARM_SAVERECIPE_EDIT, m_strNewRecipeName);
}


BEGIN_MESSAGE_MAP(CCopyRecipeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCopyRecipeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCopyRecipeDlg message handlers


void CCopyRecipeDlg::OnBnClickedOk()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	UpdateData(TRUE);
	if(m_strNewRecipeName.IsEmpty())
		return;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFile = FindFirstFile(pApp->m_strMainCfgDir + "\\" + m_strNewRecipeName, &FindFileData);
	if(hFile==INVALID_HANDLE_VALUE) {
		FindClose(hFile);
		pApp->CopyConfigFile(m_strNewRecipeName);
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Config File", pApp->m_strConfigFile);

		CString str;
		str.Format("New recipe \"%s\" created", m_strNewRecipeName);
		::AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
	}
	else { 
		FindClose(hFile);
		::AfxMessageBox("This filename already exists\nPlease use another name", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_PARM_SAVERECIPE_EDIT)->SetFocus();
		return;
	}

	CDialogEx::OnOK();
}
