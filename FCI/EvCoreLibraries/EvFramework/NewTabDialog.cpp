/// NewTabDialog.cpp: implementation of the CNewTabDialog class.
/// Udupa; August'2012

#include "stdafx.h"
#include "EvMainFrame.h"
#include "NewTabDialog.h"
#include "afxdialogex.h"


// CNewTabDialog dialog

IMPLEMENT_DYNAMIC(CNewTabDialog, CDialogEx)

CNewTabDialog::CNewTabDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewTabDialog::IDD, pParent)
{
	m_backColor = RGB(70,70,70);
	m_brush.CreateSolidBrush(RGB(100,100,100));
}

CNewTabDialog::~CNewTabDialog()
{
	m_brush.DeleteObject();
}

void CNewTabDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGEHISTORY_MFCBUTTON, m_buttonImageHistory);
	DDX_Control(pDX, IDC_MESSAGELOGGER_MFCBUTTON, m_buttonMessageLog);
	DDX_Control(pDX, IDC_CPULOAD_MFCBUTTON, m_buttonCpuLoad);
	DDX_Control(pDX, IDC_IMAGEBROWSER_MFCBUTTON, m_buttonImageBrowser);
	DDX_Control(pDX, IDC_VALIDATIONDOC_MFCBUTTON, m_buttonValidationDoc);
	DDX_Control(pDX, IDC_STATISTICS_MFCBUTTON, m_buttonStatistics);
	DDX_Control(pDX, IDC_WATCHDOG_MFCBUTTON, m_buttonWatchdog);
	DDX_Control(pDX, IDC_MULTICAST_MFCBUTTON, m_buttonMultiCast);
}


BEGIN_MESSAGE_MAP(CNewTabDialog, CDialogEx)
	ON_BN_CLICKED(IDC_MESSAGELOGGER_MFCBUTTON, OnBnClickedMessageloggerMfcbutton)
	ON_BN_CLICKED(IDC_IMAGEHISTORY_MFCBUTTON, OnBnClickedImagehistoryMfcbutton)
	ON_BN_CLICKED(IDC_MULTICAST_MFCBUTTON, OnBnClickedMulticastMfcbutton)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CNewTabDialog message handlers

BOOL CNewTabDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
	m_buttonImageHistory.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonMessageLog.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonCpuLoad.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonImageBrowser.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonValidationDoc.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonStatistics.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonWatchdog.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	m_buttonMultiCast.m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;

	m_buttonImageHistory.SetImage(IDB_IMAGEHISTORY_BITMAP);
	m_buttonMessageLog.SetImage(IDB_MESSAGELOGGER_BITMAP);
	m_buttonMultiCast.SetImage(IDB_MULTICAST_PNG);

	//m_buttonImageHistory.SetFaceColor(m_backColor);
	//m_buttonMessageLog.SetFaceColor(m_backColor);
	//m_buttonCpuLoad.SetFaceColor(m_backColor);

	//COLORREF textColor = RGB(255,255,255);
	//m_buttonImageHistory.SetTextColor(textColor);
	//m_buttonMessageLog.SetTextColor(textColor);
	//m_buttonCpuLoad.SetTextColor(textColor);

	///CEvMainFrame* pParent = (CEvMainFrame*) GetParent();
	///CRect rectParent, rectDlg;
	///pParent->GetCanvasRect(rectParent);
	///GetWindowRect(&rectDlg);

	//SetWindowPos(NULL, 
	//	rectParent.left,
	//	rectParent.top,
	//	0, 0, SWP_NOSIZE);

	//SetWindowPos(NULL, 
	//	rectParent.left + rectParent.Width()/2 - rectDlg.Width()/2,
	//	rectParent.top + rectParent.Height()/2 - rectDlg.Height()/2,
	//	0, 0, SWP_NOSIZE | SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CNewTabDialog::OnBnClickedMessageloggerMfcbutton()
{
	//CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
	//pFrame->OnNewwindowMessagelogger();
	//OnCancel();
}

void CNewTabDialog::OnBnClickedImagehistoryMfcbutton()
{
	CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
	pFrame->OnNewwindowImageHistory();
	OnCancel();
}

void CNewTabDialog::OnBnClickedMulticastMfcbutton()
{
	CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
	pFrame->OnNewwindowMulticast();
	OnCancel();
}

HBRUSH CNewTabDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    pDC->SetTextColor(RGB(255,255,255));
	pDC->SetBkColor(RGB(100,100,100));

	return m_brush;
}

void CNewTabDialog::OnCancel() 
{
	CDialogEx::OnCancel();
}

void CNewTabDialog::OnClose() 
{
	//CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
	//pFrame->m_wndRibbonBar.SetActiveCategory(pFrame->m_wndRibbonBar.GetCategory(0), TRUE);

	CDialogEx::OnClose();
}
