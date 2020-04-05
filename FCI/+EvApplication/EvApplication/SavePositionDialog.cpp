// SavePositionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "SavePositionDialog.h"
#include "afxdialogex.h"


// CSavePositionDialog dialog

IMPLEMENT_DYNAMIC(CSavePositionDialog, CDialogEx)

CSavePositionDialog::CSavePositionDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSavePositionDialog::IDD, pParent)
	, m_nSaveOption(0)
	, m_strNewRecipe(_T(""))
{

}

CSavePositionDialog::~CSavePositionDialog()
{
}

void CSavePositionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_POS_CUR_RADIO, m_nSaveOption);
	DDX_Text(pDX, IDC_POS_NEWRECIPE_EDIT, m_strNewRecipe);
}


BEGIN_MESSAGE_MAP(CSavePositionDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSavePositionDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CSavePositionDialog message handlers


void CSavePositionDialog::OnBnClickedOk()
{
	UpdateData();
	m_strNewRecipe = m_strNewRecipe.Trim();
	if(m_nSaveOption==1) {
		if(m_strNewRecipe.IsEmpty()) {
			GetDlgItem(IDC_POS_NEWRECIPE_EDIT)->SetFocus();
			return;
		}

		WIN32_FIND_DATA FindFileData;
		CApplication* pApp = (CApplication*) AfxGetApp();
		HANDLE hFile = FindFirstFile(pApp->m_strMainCfgDir+"\\*.", &FindFileData);
		if(hFile != INVALID_HANDLE_VALUE) {
			do {
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					CString str = FindFileData.cFileName;
					if(str.Find(".")==-1 && str.CompareNoCase("_Common")!=0) {
						if(!str.CompareNoCase(m_strNewRecipe)) {
							CString str;
							str.Format("The recipe \"%s\" already exists", m_strNewRecipe);
							AfxMessageBox(str, MB_OK | MB_ICONERROR);
							GetDlgItem(IDC_POS_NEWRECIPE_EDIT)->SetFocus();
							return;
						}
					}
				}
			} while (FindNextFile(hFile, &FindFileData));
		}
		FindClose(hFile);
	}
	
	CDialogEx::OnOK();
}
