// LoadRecipeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "LoadRecipeDlg.h"
#include "afxdialogex.h"


// CLoadRecipeDlg dialog

IMPLEMENT_DYNAMIC(CLoadRecipeDlg, CDialogEx)

CLoadRecipeDlg::CLoadRecipeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoadRecipeDlg::IDD, pParent)
{

}

CLoadRecipeDlg::~CLoadRecipeDlg()
{
}

void CLoadRecipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECIPE_LIST, m_listRecipe);
}


BEGIN_MESSAGE_MAP(CLoadRecipeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoadRecipeDlg::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_RECIPE_LIST, &CLoadRecipeDlg::OnLbnSelchangeRecipeList)
END_MESSAGE_MAP()


// CLoadRecipeDlg message handlers


BOOL CLoadRecipeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pApp = (CApplication*) AfxGetApp();
	PopulateRecipeList();

	CRect rect1, rect2;
	m_pApp->m_pMainWnd->GetWindowRect(&rect1);
	GetWindowRect(&rect2);
	SetWindowPos(NULL, rect1.Width()/2 - rect2.Width()/2, 0, 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadRecipeDlg::PopulateRecipeList()
{
	m_listRecipe.ResetContent();
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;
	if((hFile=FindFirstFile(m_pApp->m_strMainCfgDir+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if((str.Find(".")==-1) && (str != m_pApp->m_strConfigFile) && (str != "_Common"))
					m_listRecipe.AddString(CString(FindFileData.cFileName));
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	FindClose(hFile);
	///m_listRecipe.SelectString(-1, m_pApp->m_strConfigFile);

	OnLbnSelchangeRecipeList();
}

void CLoadRecipeDlg::OnBnClickedOk()
{
	if(m_listRecipe.GetCurSel() < 0)
		return;

	CString strSel;
	m_listRecipe.GetText(m_listRecipe.GetCurSel(), strSel);

	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;
	hFile = FindFirstFile(m_pApp->m_strMainCfgDir + "\\" + strSel, &FindFileData);
	if (hFile==INVALID_HANDLE_VALUE) { 
		AfxMessageBox("Error: File not found", MB_ICONSTOP);

		int nSel = m_listRecipe.GetCurSel();
		m_listRecipe.DeleteString(nSel);
		m_listRecipe.SetCurSel(__min(nSel+1, m_listRecipe.GetCount()-1));
	}
	else {
		m_pApp->LoadConfigFile(strSel);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	FindClose(hFile);

	//m_pApp->m_pFrames[m_pApp->m_nTotalTracks]->GetActiveDocument()->SetTitle("Statistics - " + m_pApp->m_strConfigFile);

	//For Last Config Modify date 
	///m_pApp->LoadConfigModifyDate(1);
	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Config File", m_pApp->m_strConfigFile);
	
	
	CDialogEx::OnOK();
}


void CLoadRecipeDlg::OnLbnSelchangeRecipeList()
{
	//CString strSel;
	//m_listRecipe.GetText(m_listRecipe.GetCurSel(), strSel);
	//GetDlgItem(IDOK)->EnableWindow(strSel != m_pApp->m_strConfigFile);
}
