// SelectGoodBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SelectGoodBDlg.h"
#include "afxdialogex.h"
#include "Application.h"

// CSelectGoodBDlg dialog

IMPLEMENT_DYNAMIC(CSelectGoodBDlg, CDialogEx)

CSelectGoodBDlg::CSelectGoodBDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectGoodBDlg::IDD, pParent)
{

}

CSelectGoodBDlg::~CSelectGoodBDlg()
{
}

void CSelectGoodBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEFECTS_LIST, m_ctrlDefectsAvailable);
	DDX_Control(pDX, IDC_GOODB_LIST, m_ctrlGoodB);
}


BEGIN_MESSAGE_MAP(CSelectGoodBDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSelectGoodBDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSelectGoodBDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RIGHT_BUTTON, &CSelectGoodBDlg::OnBnClickedRightButton)
	ON_BN_CLICKED(IDC_LEFT_BUTTON, &CSelectGoodBDlg::OnBnClickedLeftButton)
END_MESSAGE_MAP()


// CSelectGoodBDlg message handlers


BOOL CSelectGoodBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetGoodBList();
	GetDefectsList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSelectGoodBDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CSelectGoodBDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CSelectGoodBDlg::OnBnClickedRightButton()
{
	CApplication *pApp = (CApplication*)AfxGetApp();

	CString strDefect, strGoodBDefect;
	CString strValueName;
	int nValueNum;
	int nIndex = m_ctrlDefectsAvailable.GetCurSel();
	m_ctrlDefectsAvailable.GetText(nIndex, strDefect);

	nValueNum = iniFile.GetNumValues(m_strKeyName);
	if(nValueNum < 0)
		nValueNum = 0;
	else
	{
		for(int i=0; i<nValueNum; i++)
		{
			strValueName.Format("%d", i);
			strGoodBDefect = iniFile.GetCString(m_strKeyName, strValueName, "");
			if(!strGoodBDefect.Compare(strDefect))
				return;
		}
	}

	strValueName.Format("%d", nValueNum);
	iniFile.SetCString(m_strKeyName, strValueName, strDefect);
	iniFile.WriteFile();

	GetGoodBList();
}


void CSelectGoodBDlg::OnBnClickedLeftButton()
{
	CApplication *pApp = (CApplication*)AfxGetApp();

	CString strDefect, strGoodBDefect;
	CString strValueName, strNextValueName;
	int nValueNum;
	int nIndex = m_ctrlGoodB.GetCurSel();
	if(nIndex < 0)
		return;
	m_ctrlGoodB.GetText(nIndex, strDefect);

	strValueName.Format("%d", nIndex);
	if(iniFile.DeleteValue(m_strKeyName, strValueName))
	{
		nValueNum = iniFile.GetNumValues(m_strKeyName);
		if(nValueNum > 0)
		{
			for(int i=nIndex; i<nValueNum; i++)
			{
				strValueName.Format("%d", i);
				strNextValueName.Format("%d", i+1);
				strGoodBDefect = iniFile.GetCString(m_strKeyName, strNextValueName, "");
				iniFile.SetCString(m_strKeyName, strValueName, strGoodBDefect);
				iniFile.DeleteValue(m_strKeyName, strNextValueName);
			}
		}
	}

	iniFile.WriteFile();
	GetGoodBList();
	m_ctrlGoodB.SetCurSel(nIndex);
	UpdateData(FALSE);
}


void CSelectGoodBDlg::GetGoodBList()
{
	CApplication *pApp = (CApplication*)AfxGetApp();
	m_strKeyName = "GoodB";

	m_ctrlGoodB.ResetContent();
	//m_strFileDir.Format("%s\\GoodBList.ini", pApp->m_strConfigDir);
	m_strFileDir.Format("%s\\GoodBList.ini", pApp->m_strMainCfgDir);
	iniFile.SetPath(m_strFileDir);
	if(iniFile.ReadFile())
	{
		CString strValueName;
		int nValues = iniFile.GetNumValues(m_strKeyName);
		for(int i=0; i<nValues; i++)
		{
			CString strValue;
			strValueName.Format("%d", i);
			strValue = iniFile.GetCString(m_strKeyName, strValueName, "");
			m_ctrlGoodB.AddString(strValue);
		}
	}
}


void CSelectGoodBDlg::GetDefectsList()
{
	int nTotalDefect = 14;
	CString strDefectName;
	CApplication *pApp = (CApplication*)AfxGetApp();

	for(int i=1; i<nTotalDefect; i++)
	{
		strDefectName = pApp->GetFailedStr(i+50, FALSE);
		if(strDefectName == "")
			continue;
		m_ctrlDefectsAvailable.AddString(strDefectName);
	}
	m_ctrlDefectsAvailable.SetCurSel(0);
	UpdateData(FALSE);
}