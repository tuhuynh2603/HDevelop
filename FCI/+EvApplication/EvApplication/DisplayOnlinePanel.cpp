// DisplayOnlinePanel.cpp : implementation file
//

#include "stdafx.h"
#include "DisplayOnlinePanel.h"
#include "Application.h"
#include "afxdialogex.h"
#include "Resource.h"

// CDisplayOnlinePanel dialog

IMPLEMENT_DYNAMIC(CDisplayOnlinePanel, CDialogEx)

CDisplayOnlinePanel::CDisplayOnlinePanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDC_ONLINE_DISPLAY_PANEL, pParent)
{
	m_pBrushTemp[0].CreateSolidBrush(RGB(0, 0, 153));
	m_pBrushTemp[1].CreateSolidBrush(RGB(230, 50, 50));
	m_pBrushTemp[2].CreateSolidBrush(RGB(255, 0, 255));
	if (!theApp.m_bIsMasterApp) {
		m_pBrushTemp[3].CreateSolidBrush(RGB(0, 0, 0));
	}
	else {
		m_pBrushTemp[3].CreateSolidBrush(RGB(50, 180, 50));
	}
}

CDisplayOnlinePanel::~CDisplayOnlinePanel()
{
	m_pBrushTemp[0].DeleteObject();
	m_pBrushTemp[1].DeleteObject();
	m_pBrushTemp[2].DeleteObject();
	m_pBrushTemp[3].DeleteObject();
}

void CDisplayOnlinePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ONLINE_RECEIPE_STATUS_STATIC, m_strRecipe);
	DDX_Text(pDX, IDC_ONLINE_LOT_ID_STATIC, m_strLotId);
	DDX_Text(pDX, IDC_ONLINE_MACHINE_ID_STATIC, m_strMachineId);
}


BEGIN_MESSAGE_MAP(CDisplayOnlinePanel, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDisplayOnlinePanel message handlers


BOOL CDisplayOnlinePanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!theApp.m_bIsMasterApp) {
		CRect rectOnlinePanel;
		GetWindowRect(&rectOnlinePanel);
		SetWindowPos(NULL, rectOnlinePanel.left, rectOnlinePanel.top, rectOnlinePanel.Width(), rectOnlinePanel.Height() * 2.2, SWP_NOMOVE | SWP_NOZORDER);
		
		CRect rectOnlineMachineID;
		GetDlgItem(IDC_ONLINE_MACHINE_ID_STATIC)->GetWindowRect(&rectOnlineMachineID);
		GetDlgItem(IDC_ONLINE_MACHINE_ID_STATIC)->SetWindowPos(NULL, rectOnlineMachineID.left, rectOnlineMachineID.top, rectOnlineMachineID.Width(), rectOnlineMachineID.Height(), SWP_NOMOVE | SWP_NOZORDER);
		
		CRect rectOnlineLotID;
		GetDlgItem(IDC_ONLINE_LOT_ID_STATIC)->GetWindowRect(&rectOnlineLotID);
		GetDlgItem(IDC_ONLINE_LOT_ID_STATIC)->SetWindowPos(NULL, rectOnlineLotID.left, rectOnlineLotID.top, rectOnlineLotID.Width(), rectOnlineLotID.Height() * 2.2, SWP_NOMOVE | SWP_NOZORDER);

		CRect rectOnlineReceipe;
		GetDlgItem(IDC_ONLINE_RECEIPE_STATUS_STATIC)->GetWindowRect(&rectOnlineReceipe);
		GetDlgItem(IDC_ONLINE_RECEIPE_STATUS_STATIC)->SetWindowPos(NULL, rectOnlineReceipe.left, rectOnlineReceipe.top, rectOnlineReceipe.Width(), rectOnlineReceipe.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDisplayOnlinePanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	m_pBrush = &m_pBrushTemp[3];
	if (pWnd->GetDlgCtrlID() == IDC_ONLINE_LOT_ID_STATIC) {
		m_pBrush = &m_pBrushTemp[0];
	}
	else if (pWnd->GetDlgCtrlID() == IDC_ONLINE_MACHINE_ID_STATIC) {
		m_pBrush = &m_pBrushTemp[1];
	}
	else if (pWnd->GetDlgCtrlID() == IDC_ONLINE_RECEIPE_STATUS_STATIC) {
		m_pBrush = &m_pBrushTemp[2];
	}
	
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 255, 255));

	return *m_pBrush;
}
