/// MultipleRecipeDialog.cpp : implementation file
/// Udupa; Dec'2012

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "MultipleRecipeDialog.h"
#include "StatisticsView.h"
#include "NewRecipeDialog.h"
#include "EvTabView.h"
#include "FileOperations.h"
#include "StationMapping.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMultipleRecipeDialog::CMultipleRecipeDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMultipleRecipeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultipleRecipeDialog)
	//}}AFX_DATA_INIT
	m_pApp = (CApplication*) AfxGetApp();
}

CMultipleRecipeDialog::~CMultipleRecipeDialog()
{
}

void CMultipleRecipeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultipleRecipeDialog)
	DDX_Control(pDX, IDC_CONFIG_FILE_SELECT1, m_ComboConfigFileSelect[0]);
	DDX_Control(pDX, IDC_CONFIG_FILE_SELECT2, m_ComboConfigFileSelect[1]);
	DDX_Control(pDX, IDC_CONFIG_FILE_SELECT3, m_ComboConfigFileSelect[2]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMultipleRecipeDialog, CDialogEx)
	//{{AFX_MSG_MAP(CMultipleRecipeDialog)
	ON_BN_CLICKED(IDC_BUT_APPLY, OnButtonLoad)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_CONFIG_FILE_SELECT2, OnCbnSelchangeConfigFileSelect)
	ON_BN_CLICKED(IDC_BUTTON_COPYRECIPE, OnBnClickedButtonCopyrecipe)
	ON_BN_CLICKED(IDC_BUTTON_DELETERECIPE, OnBnClickedButtonDeleterecipe)
	ON_CBN_SELCHANGE(IDC_CONFIG_FILE_SELECT3, &CMultipleRecipeDialog::OnSelchangeConfigFileSelect3)
	ON_BN_CLICKED(IDC_COPY_PROD_MODE, &CMultipleRecipeDialog::OnBnClickedCopyProdMode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultipleRecipeDialog message handlers

BOOL CMultipleRecipeDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
	GetDlgItem(IDC_BUT_APPLY)->EnableWindow(!m_pApp->m_bOnline);
	
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;
	if((hFile=FindFirstFile(m_pApp->m_strMainCfgDir+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
		m_ComboConfigFileSelect[2].ResetContent();
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if(str.CompareNoCase("Production Config")==0 ||str.CompareNoCase("Engineering Config")==0 ) {
						m_ComboConfigFileSelect[2].AddString(CString(FindFileData.cFileName));
				}
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	m_ComboConfigFileSelect[2].SelectString(0, m_pApp->m_strMainCfgModeDir);
	OnSelchangeConfigFileSelect3();
	/*if((hFile=FindFirstFile(m_pApp->m_strMainCfgModeDir+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
		m_ComboConfigFileSelect[0].ResetContent();
		m_ComboConfigFileSelect[1].ResetContent();
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if(str.Find(".")==-1 && str.CompareNoCase("_Common")!=0) {
					for(int i=0; i<2; i++)
						m_ComboConfigFileSelect[i].AddString(CString(FindFileData.cFileName));
				}
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	FindClose(hFile);

	m_ComboConfigFileSelect[0].SelectString(-1, m_pApp->m_strConfigFile);*/
	GetDlgItem(IDC_COPY_PROD_MODE)->EnableWindow(FALSE);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMultipleRecipeDialog::OnButtonLoad() 
{
	CString strMsg;

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	if (m_pApp->m_bLotOpened)
		AfxMessageBox("Lot is opened! Not allowed to load recipe!");
	else
	{ 
		m_ComboConfigFileSelect[2].GetWindowText(m_pApp->m_strMainCfgModeDir);
	
		m_ComboConfigFileSelect[0].GetWindowText(m_pApp->m_strConfigFile);
	
		m_pApp->LoadRecipes();
		//m_pApp->m_LightHandler.LoadLightSettings();
		m_pApp->SaveRecipeInfo();
	
		strMsg.Format("%s Mode %s Recipe Loaded",m_pApp->m_strMainCfgModeDir,m_pApp->m_strConfigFile);
		m_pApp->AddLogEvent(strMsg);
	}

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	OnOK();
}

void CMultipleRecipeDialog::OnOK() 
{	
	CDialogEx::OnOK();
}

void CMultipleRecipeDialog::OnCbnSelchangeConfigFileSelect()
{
	if(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nCreateConfig) {
		CString strSel,strSelMode;
		m_ComboConfigFileSelect[2].GetLBText(m_ComboConfigFileSelect[2].GetCurSel(), strSelMode);
		m_ComboConfigFileSelect[1].GetLBText(m_ComboConfigFileSelect[1].GetCurSel(), strSel);
		if((m_pApp->m_strConfigFile.CompareNoCase(strSel)!=0) || (m_pApp->m_strMainCfgModeDir.CompareNoCase(strSelMode)!=0))
			GetDlgItem(IDC_BUTTON_DELETERECIPE)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_BUTTON_DELETERECIPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_COPYRECIPE)->EnableWindow(TRUE);
		GetDlgItem(IDC_COPY_PROD_MODE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DELETERECIPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_COPYRECIPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_COPY_PROD_MODE)->EnableWindow(FALSE);
	}
}


void CMultipleRecipeDialog::OnBnClickedButtonCopyrecipe()
{
	CNewRecipeDialog dlg;
	if(dlg.DoModal()==IDOK) {
		CString strSrc, strDst;
		strDst = dlg.m_strNewFileName.Trim();
		int nPos = 0;
		CString resToken = strDst.Tokenize(" ", nPos);
		if (strDst.IsEmpty() || resToken.IsEmpty())
		{
			AfxMessageBox("Please Provide Valid Name");
			strDst = "";
			return;
		}

		CString strSel;
		int nCount = m_ComboConfigFileSelect[1].GetCount();
		for(int i=0; i<nCount; i++) {
			m_ComboConfigFileSelect[1].GetLBText(i, strSel);
			if(strSel.CompareNoCase(strDst)==0) {
				CString str;
				str.Format("The recipe \"%s\" already exists", strDst);
				AfxMessageBox(str, MB_OK | MB_ICONERROR);
				return;
			}
		}
		CString strSelConfigMode;
		int nSelIndex = m_ComboConfigFileSelect[2].GetCurSel();
		m_ComboConfigFileSelect[2].GetLBText(nSelIndex, strSelConfigMode);
		m_ComboConfigFileSelect[1].GetWindowText(strSrc);

		int nError = m_pApp->CopyRecipe(strSelConfigMode,strSelConfigMode,strSrc, strDst);
		if (!nError) {
			for (int i = 0; i<2; i++)
				m_ComboConfigFileSelect[i].AddString(dlg.m_strNewFileName);
			m_ComboConfigFileSelect[1].SelectString(-1, dlg.m_strNewFileName);
			OnCbnSelchangeConfigFileSelect();
			CString strMsg;
			strMsg.Format("%s Mode %s Recipe Copied", m_pApp->m_strMainCfgModeDir, m_pApp->m_strConfigFile);
			m_pApp->AddLogEvent(strMsg);
		}
		
	}
}


void CMultipleRecipeDialog::OnBnClickedButtonDeleterecipe()
{
	CString str1, strSrc;
	CFileOperation m_FileOpertion;
	m_ComboConfigFileSelect[1].GetWindowText(strSrc);
	str1.Format("Confirm to delete recipe \"%s\"", strSrc);
	if(AfxMessageBox(str1, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
		CString strTemp;
		int nSelIndex = m_ComboConfigFileSelect[2].GetCurSel();
		CString strSelConfigMode;
		m_ComboConfigFileSelect[2].GetLBText(nSelIndex, strSelConfigMode);
		strTemp = m_pApp->m_strMainCfgDir +"\\"+strSelConfigMode;
		str1.Format("%s\\%s", strTemp, strSrc);
		if(DeleteDir(str1)) {
		//if(!m_FileOpertion.Delete(str1)) {
			CString str2;
			str2.Format("Failed to delete recipe folder\n\n\"%s\"\n", str1);
			AfxMessageBox(str2, MB_OK | MB_ICONERROR);
		}
		else {
			int nIndex;;
			for(int i=0; i<2; i++) {
				nIndex = m_ComboConfigFileSelect[i].FindString(-1, strSrc);
				if(nIndex >= 0)
					m_ComboConfigFileSelect[i].DeleteString(nIndex);
			}
			m_ComboConfigFileSelect[0].SelectString(-1, m_pApp->m_strConfigFile);
			m_ComboConfigFileSelect[1].SetCurSel(__min(nIndex, m_ComboConfigFileSelect[1].GetCount()-1));
			OnCbnSelchangeConfigFileSelect();
			UpdateData(FALSE);
		}
	}
}


void CMultipleRecipeDialog::OnSelchangeConfigFileSelect3()
{
	// TODO: Add your control notification handler code here
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;
	int nSelIndex = m_ComboConfigFileSelect[2].GetCurSel();
	CString strSelConfigMode;
	m_ComboConfigFileSelect[2].GetLBText(nSelIndex, strSelConfigMode);
	if(strSelConfigMode.Compare("Production Config")==0)
		GetDlgItem(IDC_COPY_PROD_MODE)->SetWindowTextA("Copy To Engineering Mode");
	else
		GetDlgItem(IDC_COPY_PROD_MODE)->SetWindowTextA("Copy To Production Mode");

	strSelConfigMode = m_pApp->m_strMainCfgDir+"\\"+strSelConfigMode;
	if((hFile=FindFirstFile(strSelConfigMode+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
		m_ComboConfigFileSelect[0].ResetContent();
		m_ComboConfigFileSelect[1].ResetContent();
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if(str.Find(".")==-1 && str.CompareNoCase("_Common")!=0) {
					for(int i=0; i<2; i++)
						m_ComboConfigFileSelect[i].AddString(CString(FindFileData.cFileName));
				}
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	FindClose(hFile);
	m_ComboConfigFileSelect[0].GetCount()==0?GetDlgItem(IDC_BUT_APPLY)->EnableWindow(FALSE):GetDlgItem(IDC_BUT_APPLY)->EnableWindow(TRUE);
	m_ComboConfigFileSelect[0].SelectString(-1, m_pApp->m_strConfigFile);
	UpdateData(FALSE);
}


void CMultipleRecipeDialog::OnBnClickedCopyProdMode()
{
	// TODO: Add your control notification handler code here
	CString strSrc, strDst,str1;
	CString strSelSrcConfigMode,strSelDestConfigMode;
	int nSelIndex = m_ComboConfigFileSelect[2].GetCurSel();
	m_ComboConfigFileSelect[2].GetLBText(nSelIndex, strSelSrcConfigMode);
	m_ComboConfigFileSelect[2].GetLBText(!nSelIndex, strSelDestConfigMode);
	
	str1.Format("Confirm to Copy recipe To %s Mode",strSelDestConfigMode);
	
	if(AfxMessageBox(str1, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
		CString strSel;
		int nCount = m_ComboConfigFileSelect[1].GetCount();
		for(int i=0; i<nCount; i++) {
			m_ComboConfigFileSelect[1].GetLBText(i, strSel);
			if(strSel.CompareNoCase(strDst)==0) {
				CString str;
				str.Format("The recipe \"%s\" already exists", strDst);
				AfxMessageBox(str, MB_OK | MB_ICONERROR);
				return;
			}
		}
		
		CString strMsg;
		m_ComboConfigFileSelect[1].GetWindowText(strSrc);
		strDst = strSrc;
		HANDLE hFile;
		WIN32_FIND_DATA FindFileData;
		CString strSelConfigMode;
		BOOL bOverWrite = TRUE;
		strSelConfigMode = m_pApp->m_strMainCfgDir+"\\"+strSelDestConfigMode;
		if((hFile=FindFirstFile(strSelConfigMode+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
			do {
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					CString str = FindFileData.cFileName;
					
					if(str.CompareNoCase(strSrc)==0) {
							str1.Format("%s recipe is already Present in %s Mode\n Do you want to Overwrite",strSrc,strSelDestConfigMode);
							if(AfxMessageBox(str1, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
								bOverWrite = TRUE;
								break;
							}
							else
								bOverWrite = FALSE;
					}
				}
			} while (FindNextFile(hFile, &FindFileData));
		}
		if(bOverWrite){
			m_pApp->CopyRecipe(strSelSrcConfigMode,strSelDestConfigMode,strSrc, strDst);			
			strMsg.Format("%s Mode %s Recipe Copied to %s Mode",strSelSrcConfigMode,strSrc,strSelDestConfigMode);
	
			m_pApp->AddLogEvent(strMsg);
			str1.Format("%s SuccessFully",strMsg);
		}
		else
			str1.Format("%s Not SuccessFull",strMsg);

		AfxMessageBox(str1);
		UpdateData(FALSE);
	}
}
