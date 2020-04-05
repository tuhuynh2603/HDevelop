// NewRecipeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "NewRecipeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewRecipeDialog dialog


CNewRecipeDialog::CNewRecipeDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewRecipeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewRecipeDialog)
	m_strNewFileName = _T("");
	//}}AFX_DATA_INIT
}

CNewRecipeDialog::~CNewRecipeDialog()
{
	///m_brush.DeleteObject();
}

void CNewRecipeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewRecipeDialog)
	DDX_Text(pDX, IDC_NEW_FILENAME, m_strNewFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewRecipeDialog, CDialogEx)
	//{{AFX_MSG_MAP(CNewRecipeDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRecipeDialog message handlers

BOOL CNewRecipeDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	///m_brush.CreateSolidBrush(RGB(30,30,30));

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CNewRecipeDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	//if(pWnd->GetDlgCtrlID() == IDC_STATIC_FILE_NAME) {
	//	pDC->SetTextColor(RGB(80,180,255));
	//	pDC->SetBkMode(TRANSPARENT);
	//}
	//if(pWnd->GetDlgCtrlID() == IDC_NEW_FILENAME) {
	//	pDC->SetTextColor(RGB(255,255,255));
	//	pDC->SetBkColor(RGB(30,30,30));
	//}

	//return m_brush;
	return hbr;
}

void CNewRecipeDialog::OnCancel() 
{

	CDialogEx::OnCancel();
}

void CNewRecipeDialog::OnOK() 
{

	CDialogEx::OnOK();
}
