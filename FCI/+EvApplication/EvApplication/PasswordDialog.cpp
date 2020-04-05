// PasswordDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "PasswordDialog.h"
#include "afxdialogex.h"


// CPasswordDialog dialog

IMPLEMENT_DYNAMIC(CPasswordDialog, CDialogEx)

CPasswordDialog::CPasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPasswordDialog::IDD, pParent)
	, m_strPassword(_T(""))
{

}

CPasswordDialog::~CPasswordDialog()
{
}

void CPasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PSWD, m_strPassword);
}


BEGIN_MESSAGE_MAP(CPasswordDialog, CDialogEx)
END_MESSAGE_MAP()

BOOL CPasswordDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// CPasswordDialog message handlers
