// StatisticsResetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "StatisticsResetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatisticsResetDlg dialog


CStatisticsResetDlg::CStatisticsResetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatisticsResetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatisticsResetDlg)
	//}}AFX_DATA_INIT

	m_pApp = (CApplication*)AfxGetApp();

	m_brush.CreateSolidBrush(RGB(30, 30, 30));
}

CStatisticsResetDlg::~CStatisticsResetDlg()
{
	m_brush.DeleteObject();
}

void CStatisticsResetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticsResetDlg)
	DDX_Control(pDX, IDC_SELECT_RESET_TIME2, m_comboSelectResetTime2);
	DDX_Control(pDX, IDC_SELECT_RESET_TIME1, m_comboSelectResetTime1);
	DDX_Control(pDX, IDC_SELECT_RESET_NUMBER, m_comboSelectResetNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatisticsResetDlg, CDialog)
	//{{AFX_MSG_MAP(CStatisticsResetDlg)
	ON_CBN_SELCHANGE(IDC_SELECT_RESET_NUMBER, OnSelchangeSelectResetNumber)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticsResetDlg message handlers

BOOL CStatisticsResetDlg::OnInitDialog() 
{

	CDialog::OnInitDialog();

	int nLotResetTime1;
    int nLotResetTime2;
	int nNoOfResetPerDay;
	
	nNoOfResetPerDay = m_pApp->nNoOfResetPerDay;
	nLotResetTime1 = m_pApp->tLotResetTime1.wHour;
	nLotResetTime2 = m_pApp->tLotResetTime2.wHour;
	
	m_comboSelectResetNumber.SetCurSel(nNoOfResetPerDay);
	m_comboSelectResetTime1.SetCurSel(nLotResetTime1);
	m_comboSelectResetTime2.SetCurSel(nLotResetTime2);
	
	
	if(nNoOfResetPerDay == 0){
		GetDlgItem(IDC_RESET_TIME2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_RESET_TIME2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESET_TIME1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_RESET_TIME1)->EnableWindow(FALSE);
	}
	if(nNoOfResetPerDay == 1){
		GetDlgItem(IDC_RESET_TIME2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_RESET_TIME2)->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStatisticsResetDlg::OnSelchangeSelectResetNumber() 
{
	
	int index;
    index = m_comboSelectResetNumber.GetCurSel();
	
	if(index == 0){
		GetDlgItem(IDC_RESET_TIME1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_RESET_TIME1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESET_TIME2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_RESET_TIME2)->EnableWindow(FALSE);
	}
	
	if(index == 1){
		GetDlgItem(IDC_RESET_TIME1)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT_RESET_TIME1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RESET_TIME2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_RESET_TIME2)->EnableWindow(FALSE);
	}
	
	if(index == 2){
		GetDlgItem(IDC_RESET_TIME1)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT_RESET_TIME1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RESET_TIME2)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT_RESET_TIME2)->EnableWindow(TRUE);
	}
}

void CStatisticsResetDlg::OnOK() 
{
	
	int nLotResetTime1;
    int nLotResetTime2;
	int nNoOfResetPerDay;
	CApplication* pApp = (CApplication*) AfxGetApp();

	CSystemLoggingInfo* pSystemLoggingInfo;
    pSystemLoggingInfo = new CSystemLoggingInfo();
	CString strParmSetName, strParmName, strMsg;
	double dOldvalue, dNewValue;
	SYSTEMTIME Stime;
	
	strParmSetName = "STATISTICS RESET";
	GetLocalTime(&Stime);
	
	nNoOfResetPerDay = m_comboSelectResetNumber.GetCurSel();
    nLotResetTime1   = m_comboSelectResetTime1.GetCurSel();
	nLotResetTime2   = m_comboSelectResetTime2.GetCurSel();
	
	strParmName = "Number Of Reset Per Day";
	dOldvalue   = m_pApp->nNoOfResetPerDay;
	dNewValue   = nNoOfResetPerDay;
	m_pApp->nNoOfResetPerDay     = nNoOfResetPerDay; 
	
	
	if(dOldvalue != dNewValue){

		strMsg.Format("%s changed to %d",strParmName, nNoOfResetPerDay);
		pApp->AddLogEvent(strMsg);
		
		dOldvalue	= dOldvalue;
		dNewValue	= dNewValue;
		pSystemLoggingInfo->SaveStatisticsResetChanges(&Stime,
											 strParmName,
											 strParmSetName,
											 dNewValue,
											 dOldvalue,
											 0);
	}
	
	strParmName = "Reset Time1";
	dOldvalue   = m_pApp->tLotResetTime1.wHour;
	dNewValue   = nLotResetTime1;
	m_pApp->tLotResetTime1.wHour = nLotResetTime1;
	
	if(dOldvalue != dNewValue){
		
		dOldvalue	= dOldvalue;
		dNewValue	= dNewValue;
		pSystemLoggingInfo->SaveStatisticsResetChanges(&Stime,
											 strParmName,
											 strParmSetName,
											 dNewValue,
											 dOldvalue,
											 0);
		strMsg.Format("Reset Time 1 is Set at %d", nLotResetTime1);
		pApp->AddLogEvent(strMsg);
	}
	
	strParmName = "Reset Time2";
	dOldvalue   = m_pApp->tLotResetTime2.wHour;
	dNewValue   = nLotResetTime2;
	m_pApp->tLotResetTime2.wHour = nLotResetTime2;
	
	if(dOldvalue != dNewValue){
		
		dOldvalue	= dOldvalue;
		dNewValue	= dNewValue;
		pSystemLoggingInfo->SaveStatisticsResetChanges(&Stime,
											 strParmName,
											 strParmSetName,
											 dNewValue,
											 dOldvalue,
											 0);
		strMsg.Format("Reset Time 2 is Set at %d", nLotResetTime2);
		pApp->AddLogEvent(strMsg);
	}
	           

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "LotNoOfReset",nNoOfResetPerDay);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "LotResetTime1",nLotResetTime1);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "LotResetTime2",nLotResetTime2);
	
	CDialog::OnOK();
}

HBRUSH CStatisticsResetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	
	//if( pWnd->GetDlgCtrlID() == IDC_STATIC_NUMBER_OF_RESET_PER_DAY )
 //   {
 //       // Set color as red.
 //       pDC->SetTextColor( RGB( 80,180,255 ));
 //       pDC->SetBkMode( TRANSPARENT );	
 //   }

	//if( pWnd->GetDlgCtrlID() == IDC_RESET_TIME1 )
 //   {
 //       // Set color as red.
	//	pDC->SetTextColor( RGB( 80,180,255 ));
 //       pDC->SetBkMode( TRANSPARENT );
 //   }

	//if( pWnd->GetDlgCtrlID() == IDC_RESET_TIME2 )
 //   {
 //       // Set color as red.
	//	pDC->SetTextColor( RGB( 80,180,255 ));
 //       pDC->SetBkMode( TRANSPARENT );
 //   }
	//if(pWnd->GetDlgCtrlID() == IDC_SELECT_RESET_TIME1 || pWnd->GetDlgCtrlID() == IDC_SELECT_RESET_TIME2 || pWnd->GetDlgCtrlID() == IDC_SELECT_RESET_NUMBER) {
	//	pDC->SetTextColor(RGB(250,250,250));
	//	pDC->SetBkColor(RGB(30,30,30));
 //   }
	//
	return m_brush;

	//return hbr;
}


