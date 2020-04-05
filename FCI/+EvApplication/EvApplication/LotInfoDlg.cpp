// LotInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LotInfoDlg.h"
//#include "afxdialogex.h"
#include "Application.h"


// CLotInfoDlg dialog

IMPLEMENT_DYNAMIC(CLotInfoDlg, CDialog)

CLotInfoDlg::CLotInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLotInfoDlg::IDD, pParent)
	, m_strLotID(_T(""))
	, m_strOperator(_T(""))
	, m_strComments(_T(""))
{
	m_strInspCfgDir.Empty();
	m_strDeviceSpecDir.Empty();
	m_strConfigFile.Empty();
	m_strWildCard.Empty();

}

CLotInfoDlg::~CLotInfoDlg()
{
}

void CLotInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOT_INFO_LOT_ID, m_strLotID);
	DDX_Text(pDX, IDC_LOT_INFO_OPERATOR, m_strOperator);
	DDX_Control(pDX, IDC_LOT_INFO_RECIPE, m_ctrlRecipe);
	DDX_Text(pDX, IDC_LOT_INFO_COMMENTS, m_strComments);
}


BEGIN_MESSAGE_MAP(CLotInfoDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_LOT_INFO_RECIPE, &CLotInfoDlg::OnCbnSelchangeLotInfoRecipe)
END_MESSAGE_MAP()


// CLotInfoDlg message handlers


void CLotInfoDlg::OnCbnSelchangeLotInfoRecipe()
{
	// TODO: Add your control notification handler code here
	int nPos = m_ctrlRecipe.GetCurSel();
	m_ctrlRecipe.GetLBText(nPos,m_strConfigFile);
}

BOOL CLotInfoDlg::OnInitDialog() 
{
	m_pApp = (CApplication*)AfxGetApp();

	CDialog::OnInitDialog();

	
	m_ctrlRecipe.ResetContent();
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;
	CString strTemp;
	strTemp.Format("%s\\%s",m_pApp->m_strMainCfgDir,m_pApp->m_strMainCfgModeDir);
	if((hFile=FindFirstFile(strTemp+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if(str.Find(".")==-1)
					m_ctrlRecipe.AddString(CString(FindFileData.cFileName));
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	FindClose(hFile);
	if(m_ctrlRecipe.GetCount()) {
		m_ctrlRecipe.SelectString(-1, m_pApp->m_strConfigFile.GetBuffer(0));
		m_ctrlRecipe.GetLBText(m_ctrlRecipe.GetCurSel(), m_strConfigFile);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLotInfoDlg::FillComboBoxList()
{
	CString strFindName,	// Full path and filename with wildcard ".*" for file searching
			strFndName;		// Found FileName

	int nPos;

	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;

	strFindName = m_strDeviceSpecDir + "\\" + m_strWildCard;
	m_ctrlRecipe.ResetContent();

	hFile = FindFirstFile(strFindName,&FindFileData);

	if (hFile==INVALID_HANDLE_VALUE) {
		FindClose(hFile);
		return;
	}

	strFndName.Format("%s",FindFileData.cFileName);
	nPos = strFndName.Find('.');
	if (nPos!=-1)
		strFndName = strFndName.Left(nPos);
	m_ctrlRecipe.AddString(strFndName);

	while (1) {
		if(!FindNextFile(hFile,&FindFileData)) {
			FindClose(hFile);
			break;
		}
		else {
			strFndName.Format("%s",FindFileData.cFileName);
			nPos = strFndName.Find('.');
			if (nPos!=-1)
				strFndName = strFndName.Left(nPos);
			m_ctrlRecipe.AddString(strFndName);
		}
	}
}

void CLotInfoDlg::OnOK() 
{
	
	CDialog::OnOK();
}