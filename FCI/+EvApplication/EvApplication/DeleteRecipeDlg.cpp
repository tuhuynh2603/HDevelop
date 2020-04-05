// DeleteRecipeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "DeleteRecipeDlg.h"
#include "afxdialogex.h"


// CDeleteRecipeDlg dialog

IMPLEMENT_DYNAMIC(CDeleteRecipeDlg, CDialogEx)

CDeleteRecipeDlg::CDeleteRecipeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeleteRecipeDlg::IDD, pParent)
{

}

CDeleteRecipeDlg::~CDeleteRecipeDlg()
{
}

void CDeleteRecipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECIPE_LIST, m_listRecipe);
}


BEGIN_MESSAGE_MAP(CDeleteRecipeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeleteRecipeDlg::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_RECIPE_LIST, &CDeleteRecipeDlg::OnLbnSelchangeRecipeList)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CDeleteRecipeDlg message handlers


BOOL CDeleteRecipeDlg::OnInitDialog()
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

void CDeleteRecipeDlg::PopulateRecipeList()
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

void CDeleteRecipeDlg::OnBnClickedOk()
{
	if(m_listRecipe.GetCurSel() < 0)
		return;

	CString strSel;
	m_listRecipe.GetText(m_listRecipe.GetCurSel(), strSel);
	if(strSel != m_pApp->m_strConfigFile) {
		HANDLE hFile;
		WIN32_FIND_DATA FindFileData;
		hFile = FindFirstFile(m_pApp->m_strMainCfgDir + "\\" + strSel, &FindFileData);
		if (hFile==INVALID_HANDLE_VALUE) { 
			FindClose(hFile);
			AfxMessageBox("ERROR: File not found", MB_ICONSTOP);
		}
		else {
			CString str;
			str.Format("Do you wish to delete the recipe \"%s\"", strSel);
			if(AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION)==IDYES) {
				char strTemp[MAX_PATH];
				memset(strTemp, NULL, MAX_PATH);
				sprintf_s(strTemp, "%s\\%s", m_pApp->m_strMainCfgDir, strSel);
				sprintf_s(strTemp, "C:\\AxiMaxVision");
			
				SHFILEOPSTRUCT fileOpStruct;
				fileOpStruct.hwnd = m_hWnd;
				fileOpStruct.wFunc = FO_DELETE;
				fileOpStruct.pFrom = strTemp;
				fileOpStruct.pTo = NULL;
				fileOpStruct.fFlags = FOF_NOCONFIRMATION  | FOF_SILENT;
				int nRet = SHFileOperation(&fileOpStruct);
				if(nRet != 0) {
					str.Format("Error deleting Recipe \"%s\". Please check if the folder is in use", strSel);
					AfxMessageBox(str, MB_OK | MB_ICONERROR);
					return;
				}
				else {
					str.Format("Recipe \"%s\" deleted", strSel);
					AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
					//PopulateRecipeList();
				}
			}
		}
	}
	
	CDialogEx::OnOK();
}


void CDeleteRecipeDlg::OnLbnSelchangeRecipeList()
{
	//CString strSel;
	//m_listRecipe.GetText(m_listRecipe.GetCurSel(), strSel);
	//GetDlgItem(IDOK)->EnableWindow(strSel != m_pApp->m_strConfigFile);
}
