// ScanBarcode.cpp : implementation file
//

#include "stdafx.h"
#include "ScanBarcode.h"
#include "afxdialogex.h"
#include "Application.h"
#include "StatisticsView.h"

// CScanBarcode dialog

IMPLEMENT_DYNAMIC(CScanBarcode, CDialogEx)

CScanBarcode::CScanBarcode(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanBarcode::IDD, pParent)
{
	m_pApp = (CApplication*) AfxGetApp();
}

CScanBarcode::~CScanBarcode()
{
}

void CScanBarcode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BARCODE, m_editBox);
	DDX_Text(pDX, IDC_EDIT_BARCODE, m_strBarcode);
}


BEGIN_MESSAGE_MAP(CScanBarcode, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_BARCODE, OnEnChangeEditBarcode)
	ON_BN_CLICKED(IDOK, &CScanBarcode::OnBnClickedOk)
	ON_BN_CLICKED(ID_QUIT, &CScanBarcode::OnBnClickedQuit)
END_MESSAGE_MAP()


// CScanBarcode message handlers


void CScanBarcode::OnEnChangeEditBarcode()
{
	UpdateData(TRUE);
	int nTotalBarcode = m_strBarcode.GetLength();
	if(nTotalBarcode == m_pApp->m_nBarcodeSize)
	if(FindRecipe())
		OnCancel();
}

BOOL CScanBarcode::FindRecipe()
{
	CString strMsg;
	CString strDir;
	CString strConfigFile;
	CString strExistingFile;
	CString strInputFile = m_strBarcode.Right(m_pApp->m_nProductCodeSize);

	CFileFind FileFind;
	BOOL bRecipeFound = FALSE;
	strConfigFile.Format("%s", m_pApp->m_strConfigFile);
	int nIndex = strConfigFile.Find("_");
	if(nIndex != -1)
		strConfigFile = strConfigFile.Left(nIndex);

	strDir.Format("%s\\*.*", m_pApp->m_strMainCfgDir);
	BOOL bRecipeFind = FileFind.FindFile(strDir);

	while(bRecipeFind) 
	{
		bRecipeFind = FileFind.FindNextFile();
		strExistingFile = FileFind.GetFileName();
		if(FileFind.IsDots())
			continue;

		CString ProductCode;

		nIndex = strExistingFile.Find("_");
		if(nIndex != -1)
			ProductCode = strExistingFile.Left(nIndex);
		else
			ProductCode = strExistingFile;

		if(!ProductCode.Compare(strInputFile))
		{
			bRecipeFound = TRUE;
			break;
		}
	}

	if(bRecipeFound)
	{
		//if(m_pApp->m_pStatsView)
		{
			//m_pApp->RecordLotStats();
			m_pApp->InitializeLotStats();
		}
		m_pApp->m_strProductCode = m_strBarcode;
		
		// Reset Counter
		m_pApp->m_nGoodACounter = 0;
		m_pApp->m_nGoodBCounter = 0;
		m_pApp->m_nFailCounter = 0;

		//Reset Statistic
		m_pApp->m_LotSummary.InitDisplaySummary();
		m_pApp->UpdateStatisticsWindow();
		if(strConfigFile.Compare(strInputFile))
		{
			m_pApp->m_strConfigFile = strExistingFile;
			m_pApp->LoadRecipes();
			//m_pApp->m_LightHandler.LoadLightSettings();
			m_pApp->SaveRecipeInfo();

			strMsg.Format("Recipe Loaded: %s", m_pApp->m_strConfigFile);
			m_pApp->AddLogEvent(strMsg);
		}
		else
			m_pApp->SetRecipeName(m_pApp->m_strConfigFile);
		m_pApp->m_bRecipeFound = TRUE;
	}
	else
		m_pApp->m_bRecipeFound = FALSE;
		//AfxMessageBox("No Recipe Found");

	if(m_pApp->m_bOnline)
		OnBnClickedQuit();

	return bRecipeFound;
}

BOOL CScanBarcode::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CScanBarcode::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
}


void CScanBarcode::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


void CScanBarcode::OnBnClickedQuit()
{
	CDialogEx::OnCancel();
}

void CScanBarcode::DisableQuitButton(BOOL bDisable)
{
	CRect rect;
	GetDlgItem(ID_QUIT)->EnableWindow(!bDisable);
	GetDlgItem(IDC_EDIT_BARCODE)->EnableWindow(TRUE);
	m_editBox.GetWindowRect(&rect);
	m_editBox.ShowWindow(SW_SHOW);
}
