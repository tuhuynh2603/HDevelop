// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "LoginDlg.h"
#include "Mainframe.h"
 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strPassword = _T("");
	m_strUsername = _T("");
	//}}AFX_DATA_INIT
	m_strUserLevel.Empty();
}

CLoginDlg::~CLoginDlg()
{
	///m_brush.DeleteObject();
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)

	DDX_Control(pDX, IDC_COMBO1, m_ctrlLevel);
	DDX_Text(pDX, IDC_EDIT_PSWD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUsername);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeAccessLevel)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnSelchangeAccessLevel() 
{
	int nPos = m_ctrlLevel.GetCurSel();
	m_ctrlLevel.GetLBText(nPos,m_strUserLevel);	
}

BOOL CLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	///m_brush.CreateSolidBrush(RGB(30, 30, 30));
	
	m_strUserLevel = "Administrator";
	FillComboBoxList();
	m_ctrlLevel.SelectString(-1, m_strUserLevel);
	GetDlgItem(IDC_EDIT_USER)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CLoginDlg::FillComboBoxList()
{
	m_ctrlLevel.ResetContent();

	m_ctrlLevel.AddString("Administrator");
	m_ctrlLevel.AddString("Technician");
	m_ctrlLevel.AddString("Operator");
}



void CLoginDlg::OnCancel() 
{
	CDialog::OnCancel();
}

HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//if(pWnd->GetDlgCtrlID() == IDC_EDIT_PSWD) {
	//	pDC->SetTextColor( RGB( 255,255,255 ));
	//	pDC->SetBkColor( RGB( 30,30,30 ) );	
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_EDIT_USER) {
	//	pDC->SetTextColor( RGB( 255,255,255 ));
	//	pDC->SetBkColor( RGB( 30,30,30 ) );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_USERNAME) {
	//	pDC->SetTextColor( RGB(  80,180,255 ));
	//	pDC->SetBkMode( TRANSPARENT );	
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_PASSWORD) {
	//	pDC->SetTextColor( RGB(  80,180,255 ));
	//	pDC->SetBkMode( TRANSPARENT );	
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_ACCESS_LEVEL) {
	//	pDC->SetTextColor( RGB(  80,180,255));
	//	pDC->SetBkMode( TRANSPARENT );	
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_COMBO1) {
	//	pDC->SetTextColor(RGB(250,250,250));
	//	pDC->SetBkColor(RGB(30,30,30));
	//}
	//	
	//return m_brush;

	return hbr;
}


void CLoginDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}
