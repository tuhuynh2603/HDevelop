// ADSTwinCATdlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "ADSTwinCATdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CADSTwinCATdlg dialog


CADSTwinCATdlg::CADSTwinCATdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CADSTwinCATdlg::IDD, pParent)
{

	//{{AFX_DATA_INIT(CADSTwinCATdlg)
	m_nNetID0 = 170;
	m_nNetID1 = 236;
	m_nNetID2 = 200;
	m_nNetID3 = 31;
	m_nNetID4 = 1;
	m_nNetID5 = 1;
	m_nADSPortNr = 801;
	m_nCycleTime = 1000000;
	m_nMaxDelay = 2000000;
	m_nTimeout = 1000;
	//}}AFX_DATA_INIT
}


void CADSTwinCATdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CADSTwinCATdlg)
	DDX_Text(pDX, IDC_NETID0, m_nNetID0);
	DDV_MinMaxInt(pDX, m_nNetID0, 0, 999);
	DDX_Text(pDX, IDC_NETID1, m_nNetID1);
	DDV_MinMaxInt(pDX, m_nNetID1, 0, 999);
	DDX_Text(pDX, IDC_NETID2, m_nNetID2);
	DDV_MinMaxInt(pDX, m_nNetID2, 0, 999);
	DDX_Text(pDX, IDC_NETID3, m_nNetID3);
	DDV_MinMaxInt(pDX, m_nNetID3, 0, 999);
	DDX_Text(pDX, IDC_NETID4, m_nNetID4);
	DDV_MinMaxInt(pDX, m_nNetID4, 0, 999);
	DDX_Text(pDX, IDC_NETID5, m_nNetID5);
	DDV_MinMaxInt(pDX, m_nNetID5, 0, 999);
	DDX_Text(pDX, IDC_ADS_PORTNR, m_nADSPortNr);
	DDV_MinMaxInt(pDX, m_nADSPortNr, 0, 99999);
	DDX_Text(pDX, IDC_CYCLE_TIME, m_nCycleTime);
	DDX_Text(pDX, IDC_MAX_DELAY, m_nMaxDelay);
	DDX_Text(pDX, IDC_TIMEOUT, m_nTimeout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CADSTwinCATdlg, CDialog)
	//{{AFX_MSG_MAP(CADSTwinCATdlg)
	//ON_BN_CLICKED(IDC_CHECK_PORTNR, OnCheckPortnr)
	//ON_CBN_SELCHANGE(IDC_COMBO_PORTNR, OnSelchangeComboPortnr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CADSTwinCATdlg message handlers

BOOL CADSTwinCATdlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
