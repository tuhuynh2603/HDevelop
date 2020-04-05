// ProgressBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBarDlg.h"
#include "afxdialogex.h"


// CProgressBarDlg dialog

IMPLEMENT_DYNAMIC(CProgressBarDlg, CDialogEx)

CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGRESS_DLG, pParent)
{
	m_nCurrentPos = 0;
}

CProgressBarDlg::~CProgressBarDlg()
{
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_PROGRESS_BAR, m_ProgressBar);
	DDX_Text(pDX, IDC_STATIC_PROGRESS, m_strProgress);
}


BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// ProgressBarDlg message handlers

BOOL CProgressBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ComputerTime = GetTickCount();
	m_ProgressBar.SetRange(0, 100);
	m_ProgressBar.SetPos(0);
	m_nTimer = SetTimer(123, 1000, NULL);

	m_brush.CreateSolidBrush(RGB(10, 10, 10));

	SLWA pSetLayeredWindowAttributes = NULL;
	HINSTANCE hmodUSER32 = LoadLibrary("USER32.DLL");
	pSetLayeredWindowAttributes = (SLWA)GetProcAddress(hmodUSER32, "SetLayeredWindowAttributes");
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	pSetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), (255 * 95) / 100, LWA_COLORKEY | LWA_ALPHA);

	SetWindowTheme(m_hWnd, L" ", NULL); 

	m_ProgressBar.SetBkColor(RGB(10, 10, 10));
	m_ProgressBar.SetBarColor(RGB(128, 135, 138));

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CProgressBarDlg::SetStatus(CString strStatus, int nPos)
{
	m_strProgress.Format("%s %d%%", strStatus, nPos);
	m_ProgressBar.SetPos(nPos);
	if (nPos >= 0 && nPos <= 100)
		m_nCurrentPos = nPos;
	else
		m_nCurrentPos = 0;

	UpdateData(FALSE);
}	

void CProgressBarDlg::OnOK()
{
}

void CProgressBarDlg::OnCancel()
{
}
 
HBRUSH CProgressBarDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT)//nCtlColor)
{
	/*HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;*/

	int id = pWnd->GetDlgCtrlID();
	if (id == IDC_STATIC_PROGRESS ||
		id == IDC_STATIC)
	{
		pDC->SetTextColor(RGB(210, 210, 210));
		pDC->SetBkMode(TRANSPARENT);

	}
	if (id == IDC_STATIC_PROGRESS_BAR)
	{
		pDC->SetTextColor(RGB(210, 210, 210));
		pDC->SetBkColor(RGB(10, 10, 10));
	}

	return m_brush;
}
