/// StatisticsView.cpp : implementation file
/// Udupa; April'2012

#include "stdafx.h"
#include "Application.h"
#include "StatisticsView.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include <direct.h>
#include "EncryptionCore.h"
#include <iostream>>
#include <fstream>
#include <stdio.h>
#include <string.h>

#include "CommMonitor.h"
#include "CommManager.h"
#include "CommInterface.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"
#include "ImageSelectionDlg.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatisticsView

IMPLEMENT_DYNAMIC(CStatisticsView, CDialogEx)

CStatisticsView::CStatisticsView(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStatisticsView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatisticsView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
	InitializeCriticalSection(&m_csLogFile);
	m_bWriteFirstLot = FALSE;
	//m_brush.CreateSolidBrush(RGB(50, 50, 50));
	//m_font.CreateFont(35, 0, 0, 0, 800, TRUE, FALSE, FALSE, 
	//					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
	//					PROOF_QUALITY, FIXED_PITCH|FF_MODERN, "Segoe UI");
	nPPMDiesNo = 0;
	dAverageDiesTimeMs = 0;
	CRect rctDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rctDesktop, NULL);
	//m_fontList.CreateFont(20, 0, 0, 0, 560, FALSE, FALSE, FALSE,
	m_fontList.CreateFont((int)(rctDesktop.Width() / 1280.0 * 15.52), 0, 0, 0, 350, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Segoe UI");
	InitializeCriticalSection(&m_CriticalSectionUpdate);

}

CStatisticsView::~CStatisticsView()
{
	DeleteCriticalSection(&m_csLogFile);
	m_fontList.DeleteObject();
	DeleteCriticalSection(&m_CriticalSectionUpdate);
	//m_brush.DeleteObject();
	//m_font.DeleteObject();
}

void CStatisticsView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticsView)
	//}}AFX_DATA_MAP
	/*DDX_Control(pDX, IDC_DEFECTS_LIST, m_ctrlDefectsList);
	DDX_Control(pDX, IDC_SUMMARY_LIST, m_ctrlSummaryList);*/
	//DDX_Control(pDX, IDC_TAB_STATISTICS, m_TabControl);
	DDX_Control(pDX, IDC_DEFECTS_LIST, m_DefectList);
	DDX_Control(pDX, IDC_SUMMARY_LIST, m_SummaryList);
	DDX_Control(pDX, IDC_LIST_STATS_INFO, m_ListStatsInfo);
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_comboDeviceList);
	DDX_Control(pDX, IDC_LIST_TOTAL_INFO, m_listTotalInfo);
	DDX_Control(pDX, IDC_LIST_CENTRALIZED_INFO, m_listCentralizedInfo);
	DDX_Control(pDX, IDC_COMBO_POSITION_LIST, m_comboPositionList);
	DDX_Control(pDX, IDC_BUTTON_STOP_MACHINE, m_ctrlMachineStopButton);

}


BEGIN_MESSAGE_MAP(CStatisticsView, CDialogEx)
	//{{AFX_MSG_MAP(CStatisticsView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP

    ON_NOTIFY(NM_CUSTOMDRAW, IDC_DEFECTS_LIST, OnCustomdrawList1)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SUMMARY_LIST, OnCustomdrawList2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_STATS_INFO, OnCustomdrawList3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_TOTAL_INFO, OnCustomdrawList4)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_CENTRALIZED_INFO, OnCustomdrawList5)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SUMMARY_LIST, &CStatisticsView::OnLvnItemchangedSummaryList)
	ON_BN_CLICKED(IDC_BUTTON_SUMMARY_LIST, &CStatisticsView::OnBnClickedButtonSummaryList)
	ON_BN_CLICKED(IDC_BUTTON_DEFECT_LIST, &CStatisticsView::OnBnClickedButtonDefectList)
	ON_BN_CLICKED(IDC_BUTTON_STATS_INFO, &CStatisticsView::OnBnClickedButtonStatsInfo)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CStatisticsView::OnCbnSelchangeComboDeviceList)
	ON_BN_CLICKED(IDC_BUTTON_TOTAL_INFO, &CStatisticsView::OnBnClickedButtonTotalInfo)
	ON_CBN_SELCHANGE(IDC_COMBO_POSITION_LIST, &CStatisticsView::OnCbnSelchangeComboPositionList)
	ON_BN_CLICKED(IDC_ONLINE_RESETSTATSCOUNTER, &CStatisticsView::OnBnClickedOnlineResetstatscounter)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MACHINE, &CStatisticsView::OnBnClickedButtonStopMachine)


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticsView diagnostics

#ifdef _DEBUG
void CStatisticsView::AssertValid() const
{
	CDialogEx::AssertValid();
}

void CStatisticsView::Dump(CDumpContext& dc) const
{
	CDialogEx::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStatisticsView message handlers

void CStatisticsView::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	CApplication* pApp = (CApplication*)AfxGetApp();
	CWnd* pWnd = GetDlgItem(IDC_DEFECTS_LIST);
	int nWidth;
	if(pWnd && IsWindow(pWnd->m_hWnd)) {
	//	//cx = cx >> 1;
		nWidth = (int) (cx/2);
		
			CRect RectButton;
			GetDlgItem(IDC_BUTTON_SUMMARY_LIST)->GetClientRect(&RectButton);
			
			GetDlgItem(ID_ONLINE_RESETSTATSCOUNTER)->SetWindowPos(NULL, 0, 0, RectButton.Width(), RectButton.Height(),
												SWP_NOZORDER);
			GetDlgItem(IDC_BUTTON_SUMMARY_LIST)->SetWindowPos(NULL,0, RectButton.Height(),RectButton.Width(),RectButton.Height(),
												SWP_NOZORDER);
			GetDlgItem(IDC_BUTTON_DEFECT_LIST)->SetWindowPos(NULL,0, RectButton.Height()*2,RectButton.Width(),RectButton.Height(),
												SWP_NOZORDER);
			GetDlgItem(IDC_BUTTON_STATS_INFO)->SetWindowPos(NULL,0, RectButton.Height()*3,RectButton.Width(),RectButton.Height(),
												SWP_NOZORDER);
			GetDlgItem(IDC_BUTTON_TOTAL_INFO)->SetWindowPos(NULL, 0, RectButton.Height() * 4, RectButton.Width(), RectButton.Height(),
												SWP_NOZORDER);
			GetDlgItem(IDC_COMBO_DEVICE_LIST)->SetWindowPos(NULL,0, RectButton.Height()*5,RectButton.Width(),RectButton.Height()*7,
												SWP_NOZORDER);
			GetDlgItem(IDC_COMBO_POSITION_LIST)->SetWindowPos(NULL, 0, RectButton.Height() * 6, RectButton.Width(), RectButton.Height() * 7,
												SWP_NOZORDER);

			GetDlgItem(IDC_BUTTON_STOP_MACHINE)->SetWindowPos(NULL, 20, RectButton.Height() * 7.5, RectButton.Width() - 20, RectButton.Height()*3,
				SWP_NOZORDER);

			m_ctrlMachineStopButton.LoadBitmaps(IDB_STOP_BUTTON, IDB_STOP_BUTTON_PRESSED, 0U, IDB_RETRY_DISABLED);


				if(IsWindow(m_SummaryList.m_hWnd))
					m_SummaryList.SetWindowPos(NULL, 
												RectButton.Width(),
												0,
												cx,
												cy,
												SWP_NOZORDER);

				if(IsWindow(m_DefectList.m_hWnd))
					m_DefectList.SetWindowPos(NULL, 
												RectButton.Width(),
												0,
												cx,
												cy,
												SWP_NOZORDER);

				if(IsWindow(m_ListStatsInfo.m_hWnd))
					m_ListStatsInfo.SetWindowPos(NULL, 
												RectButton.Width(),
												0,
												cx,
												cy,
												SWP_NOZORDER);

				if (IsWindow(m_listTotalInfo.m_hWnd))
					m_listTotalInfo.SetWindowPos(NULL,
						RectButton.Width(),
						0,
						cx,
						cy,
						SWP_NOZORDER);

				if (IsWindow(m_listCentralizedInfo.m_hWnd))
					m_listCentralizedInfo.SetWindowPos(NULL,
						RectButton.Width(),
						0,
						cx,
						cy,
						SWP_NOZORDER);
			
		
		if(m_bInitialized) {
			CRect rect;
			/*m_ctrlSummaryList.GetClientRect(&rect);
			nWidth = rect.Width();
			m_ctrlSummaryList.SetColumnWidth(0, (int)(nWidth*0.30));
			m_ctrlSummaryList.SetColumnWidth(1, (int)(nWidth*0.40));
			m_ctrlSummaryList.SetColumnWidth(2, (int)(nWidth*0.40));

			m_ctrlDefectsList.GetClientRect(&rect);
			nWidth = rect.Width();
			m_ctrlDefectsList.SetColumnWidth(0, (int)(nWidth*0.5));
			m_ctrlDefectsList.SetColumnWidth(1, (int)(nWidth*0.49));*/
		}
	}
}

void CStatisticsView::OnInitialUpdate() 
{
	//Initialize();
	CApplication* pApp = (CApplication*)AfxGetApp();
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	KillTimer(1);
	if(pApp->m_bIsMasterApp)
		SetTimer(1, 60000, NULL);
//	CDialogEx::OnInitialUpdate();
	CString str;
	m_comboDeviceList.ResetContent();
	for (int i = 0; i< m_nNoOfDies; i++) {
		str.Format("THA %d",i + 1);
		m_comboDeviceList.AddString(str);
	}
	m_comboPositionList.ResetContent();
	for (int i = 0; i< m_nNoOfFov; i++) {
		str.Format("Position %d", i + 1);
		m_comboPositionList.AddString(str);
	}
	m_comboDeviceList.SetCurSel(0);
	m_comboPositionList.SetCurSel(0);
}

void CStatisticsView::Initialize(BOOL bRefresh)
{
	//ListView_SetExtendedListViewStyle(m_ctrlSummaryList.m_hWnd, LVS_EX_FULLROWSELECT);// | LVS_EX_GRIDLINES);
	//ListView_SetExtendedListViewStyle(m_ctrlDefectsList.m_hWnd, LVS_EX_FULLROWSELECT);// | LVS_EX_GRIDLINES);
	CApplication* pApp = (CApplication*)AfxGetApp();
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	m_SummaryList.DeleteAllItems();
	m_DefectList.DeleteAllItems();
	m_ListStatsInfo.DeleteAllItems();
	m_listTotalInfo.DeleteAllItems();
	if (bRefresh) {
		for(int i=0;i<MAX_DIES+2;i++)
		m_listTotalInfo.DeleteColumn(0);
		m_listTotalInfo.DeleteColumn(1);
	}
		
	COLORREF clrList = RGB(50,50,50);
	COLORREF clrText = RGB(255,255,255);

	m_listCentralizedInfo.DeleteAllItems();
	if (bRefresh) {
		for (int i = 0; i < NO_OF_PORT_CENTRALIZED; i++)
			m_listCentralizedInfo.DeleteColumn(0);
		m_listCentralizedInfo.DeleteColumn(1);
	}


	m_SummaryList.SetExtendedStyle(m_SummaryList.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_TRANSPARENTBKGND | LVS_EX_FULLROWSELECT);
	m_DefectList.SetExtendedStyle(m_DefectList.GetExtendedStyle()| LVS_EX_DOUBLEBUFFER | LVS_EX_TRANSPARENTBKGND | LVS_EX_FULLROWSELECT);
	m_ListStatsInfo.SetExtendedStyle(m_ListStatsInfo.GetExtendedStyle()| LVS_EX_DOUBLEBUFFER | LVS_EX_TRANSPARENTBKGND | LVS_EX_FULLROWSELECT);
	m_listTotalInfo.SetExtendedStyle(m_ListStatsInfo.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_TRANSPARENTBKGND | LVS_EX_FULLROWSELECT);
	m_listCentralizedInfo.SetExtendedStyle(m_ListStatsInfo.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_TRANSPARENTBKGND | LVS_EX_FULLROWSELECT);

	m_SummaryList.SetBkColor(clrList);
	m_SummaryList.SetTextBkColor(clrList);
	m_SummaryList.SetTextColor(clrText);
	m_DefectList.SetBkColor(clrList);
	m_DefectList.SetTextBkColor(clrList);
	m_DefectList.SetTextColor(clrText);
	m_ListStatsInfo.SetBkColor(clrList);
	m_ListStatsInfo.SetTextBkColor(clrList);
	m_ListStatsInfo.SetTextColor(clrText);
	
	m_listTotalInfo.SetBkColor(clrList);
	m_listTotalInfo.SetTextBkColor(clrList);
	m_listTotalInfo.SetTextColor(clrText);

	m_listCentralizedInfo.SetBkColor(clrList);
	m_listCentralizedInfo.SetTextBkColor(clrList);
	m_listCentralizedInfo.SetTextColor(clrText);

	CImageList imageList; 
	imageList.Create(1, 23, ILC_COLOR, 1, 1);
	m_SummaryList.SetImageList(&imageList, LVSIL_SMALL);
	m_DefectList.SetImageList(&imageList, LVSIL_SMALL);
	m_ListStatsInfo.SetImageList(&imageList, LVSIL_SMALL);
	m_listTotalInfo.SetImageList(&imageList, LVSIL_SMALL);
	m_listCentralizedInfo.SetImageList(&imageList, LVSIL_SMALL);

	m_SummaryList.SetFont(&m_fontList);
	m_DefectList.SetFont(&m_fontList);
	m_ListStatsInfo.SetFont(&m_fontList);
	m_listTotalInfo.SetFont(&m_fontList);
	m_listCentralizedInfo.SetFont(&m_fontList);

	int nCol = 0;
	int nWidth;
	CRect rectArea;
	CString str;
	
	m_SummaryList.GetClientRect(&rectArea);
	nWidth = rectArea.Width();
	if (bRefresh)
		nWidth = nWidth*0.15;
	if (!bRefresh) {
		nCol = 0;
		m_SummaryList.InsertColumn(nCol++, "THA/Pos", LVCFMT_CENTER, int(nWidth*0.5), -1);
		m_SummaryList.InsertColumn(nCol++, "Statistics", LVCFMT_CENTER, int(nWidth*0.5), -1);

		for (int i = 0; i<pApp->m_nTotalViewDocs[0]; i++)
		{
			CString str;
			str = pApp->m_strTopCameraName[i];
			m_SummaryList.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
		}
		for (int i = 0; i<pApp->m_nTotalViewDocs[1]; i++)
		{
			CString str;
			str = pApp->m_strBottomCameraName[i];
			m_SummaryList.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
		}
		/*for (int i = 0; i<pApp->m_nTotalViewDocs[2]; i++)
		{
			CString str;
			str = pApp->m_strSideCameraName[i];
			m_SummaryList.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
		}*/
	}
	
	//for(int i=0;i<pApp->m_nNoOfFov;i++){
	AddItem(&m_SummaryList,(LPCTSTR)" ", 0);
	AddItem(&m_SummaryList,(LPCTSTR)" ", 0);
	AddItem(&m_SummaryList,(LPCTSTR)" ", 0);
	AddItem(&m_SummaryList,(LPCTSTR)" ", 0);
	AddItem(&m_SummaryList, (LPCTSTR)" ", 0);

	m_SummaryList.SetItemText(0, 1, "Inspected");
	m_SummaryList.SetItemText(1, 1, "Passed");
	m_SummaryList.SetItemText(2, 1, "Yield %");
	m_SummaryList.SetItemText(3, 1, "Failed");
	m_SummaryList.SetItemText(4, 1, "User Modified");

	int i;
	for(i=0; i<m_SummaryList.GetItemCount(); i++){
		m_colorInfo2[i][0] = RGB(255,255,255);
	}
	m_colorInfo2[0][1] = RGB(150,150,255);
	m_colorInfo2[1][1] = RGB( 50,255, 50);
	m_colorInfo2[2][1] = RGB( 50,255, 50);
	m_colorInfo2[3][1] = RGB(255,100,100);
	m_colorInfo2[4][1] = RGB(255, 100, 100);
	for (int j = 2; j < pApp->m_nTotalViews+2; j++) {
		m_colorInfo2[0][j] = RGB(150, 150, 255);
		m_colorInfo2[1][j] = RGB(50, 255, 50);
		m_colorInfo2[2][j] = RGB(50, 255, 50);
		m_colorInfo2[3][j] = RGB(255, 100, 100);
		m_colorInfo2[4][j] = RGB(255, 100, 100);
	}
	
	
	m_DefectList.GetClientRect(&rectArea);
	nWidth = rectArea.Width();
	if (!bRefresh) {
		nCol = 0;
		m_DefectList.InsertColumn(nCol++, "THA/Pos", LVCFMT_LEFT, int(nWidth*0.5), -1);
		m_DefectList.InsertColumn(nCol++, "Defects", LVCFMT_LEFT, int(nWidth*0.5), -1);

		for (int i = 0; i<pApp->m_nTotalViewDocs[0]; i++)
		{
			str = pApp->m_strTopCameraName[i];
			m_DefectList.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
		}
		for (int i = 0; i<pApp->m_nTotalViewDocs[1]; i++)
		{
			str = pApp->m_strBottomCameraName[i];
			m_DefectList.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
		}
		for (int i = 0; i<pApp->m_nTotalViewDocs[2]; i++)
		{
			str = pApp->m_strSideCameraName[i];
			m_DefectList.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
		}
	}

	//for (int j = 0; j < pApp->m_nTotalViews; j++) {
		for (int i = 0; i < m_DefectList.GetItemCount(); i++) {
			m_DefectList.DeleteItem(i);
		}
	//}

	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	AddItem(&m_DefectList, (LPCTSTR)" ", 0);
	int k = 0;
	m_DefectList.SetItemText(k++, 1, "Device Location");
	m_DefectList.SetItemText(k++, 1, "Datum Location");
	m_DefectList.SetItemText(k++, 1, "Tilt Inspection");
	m_DefectList.SetItemText(k++, 1, "Cover Layer Inspection");
	m_DefectList.SetItemText(k++, 1, "Cover Layer Alignment");
	m_DefectList.SetItemText(k++, 1, "Encap Insufficient");
	m_DefectList.SetItemText(k++, 1, "Encap Excess");
	m_DefectList.SetItemText(k++, 1, "Encap Crack");
	m_DefectList.SetItemText(k++, 1, "Encap BlowHole");
	m_DefectList.SetItemText(k++, 1, "Encap Contamination");
	m_DefectList.SetItemText(k++, 1, "Slot");
	m_DefectList.SetItemText(k++, 1, "Die Edge");
	m_nTotalStaticDefectsCount = k;

	for (int j = 0; j < pApp->m_nTotalViews; j++) {
		for (int i = m_nTotalStaticDefectsCount; i < m_DefectList.GetItemCount(); i++) {
			m_DefectList.SetItemText(i, j + 1, " ");
		}
	}

	RefreshDefectsList();
	if (!bRefresh) {
		m_ListStatsInfo.InsertColumn(0, "Data", LVCFMT_LEFT, int(nWidth), -1);
		m_ListStatsInfo.InsertColumn(1, "Information", LVCFMT_LEFT, int(nWidth), -1);
	}
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		str.Format("Part %d", nDie + 1);
		AddItem(&m_ListStatsInfo, (LPCTSTR)str, 0);
	}
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		str.Format("Part %d Inspect", nDie + 1);
		AddItem(&m_ListStatsInfo, (LPCTSTR)str, 0);
	}
	AddItem(&m_ListStatsInfo,(LPCTSTR)"Lot Number", 0);
	AddItem(&m_ListStatsInfo,(LPCTSTR)"Recipe", 0);
	AddItem(&m_ListStatsInfo, (LPCTSTR)"PPM", 0);
	AddItem(&m_ListStatsInfo, (LPCTSTR)"UPH", 0);
		
	for(i=0; i<m_ListStatsInfo.GetItemCount(); i++){
		m_colorInfo3[i][0] = RGB(150,150,255);
		m_colorInfo3[i][1] = RGB(50, 255, 50);
	}


	AddItem(&m_listTotalInfo, (LPCTSTR)"Inspected",0);
	AddItem(&m_listTotalInfo, (LPCTSTR)"Passed",0);
	AddItem(&m_listTotalInfo, (LPCTSTR)"Yield %",0);
	AddItem(&m_listTotalInfo, (LPCTSTR)"Failed",0);
	AddItem(&m_listTotalInfo, (LPCTSTR)"False Rejection", 0);
	AddItem(&m_listTotalInfo, (LPCTSTR)" ", 0);

	AddItem(&m_listTotalInfo, (LPCTSTR)"Device Location",0);
	AddItem(&m_listTotalInfo, (LPCTSTR)"Datum Location", 0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Tilt Inspection",0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Cover Layer Inspection",0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Cover Layer Alignment", 0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Encap Insufficient",0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Encap Excess",0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Encap Crack", 0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Encap BlowHole",0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Encap Contamination",0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Slot", 0);
	AddItem(&m_listTotalInfo, (LPCTSTR) "Die Edge",0);

	/*for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
		for (int i = 0; i<pApp->m_strPviDefectsStats[nFov].GetCount(); i++) {
			AddItem(&m_listTotalInfo, (LPCTSTR)pApp->m_strPviDefectsStats[nFov].GetAt(i), 0);
		}
	}*/

	AddItem(&m_listTotalInfo, (LPCTSTR)" ", 0);

	for (std::map<int, CString>::iterator it = pApp->m_mapPviDefectsStatsGeneral.begin(); it != pApp->m_mapPviDefectsStatsGeneral.end(); it++) {
		int nTrackIdx, nDocIdx;
		CString strDefectDisplay = it->second;
		pApp->GetInformationFromErrorCode(it->first, it->second, nTrackIdx, nDocIdx);
		if (nTrackIdx != -1 && nDocIdx != -1) {
			CString strDoc;
			strDoc = nTrackIdx == 0 ? pApp->m_strTopCameraName[nDocIdx] : (nTrackIdx == 1 ? pApp->m_strBottomCameraName[nDocIdx] : pApp->m_strSideCameraName[nDocIdx]);
			strDefectDisplay.Format("%s_%s", strDoc, it->second);
		}

		AddItem(&m_listTotalInfo, (LPCTSTR)strDefectDisplay, 0);
	}

	if (bRefresh)
		nWidth = nWidth*0.5;
	m_listTotalInfo.InsertColumn(nCol++, "Statistics", LVCFMT_CENTER, int(nWidth), -1);
	for (int i = 0; i< m_nNoOfDies; i++)
	{
		CString str;
		str.Format("Device %d", i + 1);
		m_listTotalInfo.InsertColumn(nCol++, str, LVCFMT_CENTER, int(nWidth*0.5), -1);
	}
	m_listTotalInfo.InsertColumn(nCol++, "Total", LVCFMT_CENTER, int(nWidth*0.5), -1);
	
	for (int i = 0; i <= m_nNoOfDies+1; i++) {
		m_colorInfo4[0][i] = RGB(150, 150, 255);
		m_colorInfo4[1][i] = RGB(50, 255, 50);
		m_colorInfo4[2][i] = RGB(50, 255, 50);
		m_colorInfo4[3][i] = RGB(255, 100, 100);
		m_colorInfo4[4][i] = RGB(255, 255, 0);
		m_colorInfo4[5][i] = RGB(255, 100, 100);
		for (int j = 0; j<pApp->m_strDefectErrorCodesPriority.GetCount()+2; j++) {
			m_colorInfo4[j+6][i] = RGB(255, 100, 100);
		}
		
	}

	//Centralized Reject Info
	CImageList imageListCentralized;
	imageListCentralized.Create(1, 50, ILC_COLOR, 1, 1);
	m_listCentralizedInfo.SetImageList(&imageListCentralized, LVSIL_NORMAL);
	AddItem(&m_listCentralizedInfo, (LPCTSTR)"Machine State", 0);
	AddItem(&m_listCentralizedInfo, (LPCTSTR)" ", 0);

	AddItem(&m_listCentralizedInfo, (LPCTSTR)"Inspected", 0);
	AddItem(&m_listCentralizedInfo, (LPCTSTR)"Passed", 0);
	AddItem(&m_listCentralizedInfo, (LPCTSTR)"Yield %", 0);
	AddItem(&m_listCentralizedInfo, (LPCTSTR)"Failed", 0);
	AddItem(&m_listCentralizedInfo, (LPCTSTR)"False Rejection", 0);
	

	nCol = 0;
	m_listCentralizedInfo.InsertColumn(nCol++, "Stats", LVCFMT_CENTER, int(0.3*nWidth), -1);
	for (int i = 0; i < NO_OF_PORT_CENTRALIZED; i++){
		m_listCentralizedInfo.InsertColumn(nCol++, pApp->m_strClientMachineID[i], LVCFMT_CENTER, int(0.5*nWidth), -1);
	}

	if (!m_bInitialized) {
		for (int i = 0; i <= NO_OF_PORT_CENTRALIZED + 1; i++) {
			m_colorInfo5[0][i] = RGB(0, 255, 0);
			m_colorInfo5[1][i] = RGB(255, 255, 255);

			m_colorInfo5[2][i] = RGB(150, 150, 255);//Inspected Row Color
			m_colorInfo5[3][i] = RGB(50, 255, 50);
			m_colorInfo5[4][i] = RGB(50, 255, 50);
			m_colorInfo5[5][i] = RGB(255, 100, 100);
			m_colorInfo5[6][i] = RGB(255, 255, 0);
		}
	}

	m_bInitialized = TRUE;			   

	if (pApp->m_bIsMasterApp) {
		m_ListStatsInfo.ShowWindow(FALSE);
		m_DefectList.ShowWindow(FALSE);
		m_SummaryList.ShowWindow(FALSE);
		m_listTotalInfo.ShowWindow(TRUE);

		m_listCentralizedInfo.ShowWindow(FALSE);
	}
	else {
		m_ListStatsInfo.ShowWindow(FALSE);
		m_DefectList.ShowWindow(FALSE);
		m_SummaryList.ShowWindow(FALSE);
		m_listTotalInfo.ShowWindow(FALSE);

		m_listCentralizedInfo.ShowWindow(TRUE);

		GetDlgItem(IDC_ONLINE_RESETSTATSCOUNTER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_SUMMARY_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_DEFECT_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_STATS_INFO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_TOTAL_INFO)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_DEVICE_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_POSITION_LIST)->ShowWindow(SW_HIDE);
	}
	
	//GetDlgItem(IDC_STATISTICS_CAPTION)->SetFont(&m_font);

}

void CStatisticsView::RefreshDefectsList()
{
	CApplication* pApp = (CApplication*)AfxGetApp();

	int i;	
	m_colorInfo1[0][0] = RGB(255, 255, 255);
	m_colorInfo1[1][0] = RGB(255, 255, 255);
	COLORREF clrEnabled = RGB(255,100,100);
	for (int j = 0; j < pApp->m_nTotalViews+2; j++){
		for (int i = 0; i<pApp->m_strDefectErrorCodesPriority.GetCount()+2; i++) {
			m_colorInfo1[i][j + 1] = clrEnabled;
		}
	}
}

void CStatisticsView::UpdateContents(BOOL bRefresh)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strVal;
	int nIndexListTotal;
	
	int nDevice = m_comboDeviceList.GetCurSel();
	int nPosition = m_comboPositionList.GetCurSel();

	int nTotalStripsInsp=0, nTotalStripsPass=0, nTotalStripsFail=0,
		nTotalUnitsInsp=0, nTotalUnitsPass=0, nTotalUnitsFail=0,
		nTotalUnitsPassGoodA=0, nTotalUnitsPassGoodB=0;

	strVal.Format("THA %d", nDevice+1);
	if (m_SummaryList.GetItemText(0,0).Compare(strVal) != 0)
		m_SummaryList.SetItemText(0,0, strVal);
	if (m_DefectList.GetItemText(0, 0).Compare(strVal) != 0)
		m_DefectList.SetItemText(0, 0, strVal);
	strVal.Format("Position %d", nPosition + 1);
	if (m_SummaryList.GetItemText(1, 0).Compare(strVal) != 0)
		m_SummaryList.SetItemText(1, 0, strVal);
	if (m_DefectList.GetItemText(1, 0).Compare(strVal) != 0)
		m_DefectList.SetItemText(1, 0, strVal);

	int nColNo;
	int k = 1;
	for (int j = 0; j<pApp->m_nTotalViews; j++) {
		k = 0;
		//for (int i = 0; i<pApp->m_nNoOfFov; i++) {
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lTotalUnitsInspected);
			if (m_SummaryList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_SummaryList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lTotalUnitsPass);
			if (m_SummaryList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_SummaryList.SetItemText(k, j + 2, strVal);
			k++;
			if (pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lTotalUnitsInspected) {
				strVal.Format("%0.1f", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].dTotalYield);
			}
			else
				strVal = "0";

			if (m_SummaryList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_SummaryList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lTotalUnitsFail);
			if (m_SummaryList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_SummaryList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lUserModifiedFalseAcceptance);
			if (m_SummaryList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_SummaryList.SetItemText(k, j + 2, strVal);
			k++;
			k++;
		//}
	}

	if (bRefresh) {
		for (int j = 0; j < pApp->m_nTotalViews; j++) {
			for (int i = m_nTotalStaticDefectsCount; i < m_DefectList.GetItemCount(); i++) {
				m_DefectList.DeleteItem(i);
			}
		}
		for (int i = 0; i<pApp->m_strPviDefectsStats[nPosition].GetCount(); i++) {
			AddItem(&m_DefectList, (LPCTSTR)" ", 0);
			m_DefectList.SetItemText(i + m_nTotalStaticDefectsCount, 1, (LPCTSTR)pApp->m_strPviDefectsStats[nPosition].GetAt(i));
		}
	}


	int nCountTemp = 0;
	for(int j=0;j<pApp->m_nTotalViews;j++){
	k=0;
	//for(int i = 0;i<pApp->m_nNoOfFov;i++)
		{
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lDieLocation);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lDatumLocation);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lTiltInsp);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++; 
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lCoverLayerInspection);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lCoverLayerAlignment);
			if (m_DefectList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lEncapInsufficient);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lEncapExcess);
			if (m_DefectList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lEncapCrack);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lEncapBlowHole);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lEncapContamination);
			if (m_DefectList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lSlot);
			if (m_DefectList.GetItemText(k, j + 2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j + 2, strVal);
			k++;
			strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lDieEdge);
			if(m_DefectList.GetItemText(k, j+2).Compare(strVal) != 0)
				m_DefectList.SetItemText(k, j+2, strVal);
			k += (nCountTemp+1);

			for (int x = 0; x < pApp->m_nPviNumberOfArea[nPosition][j]; x++) {
				for (int y = 0; y < pApp->m_nPviNoOfDefects[nPosition][j][x]; y++) {
					for (int z = 0; z < pApp->m_nTotalViews; z++) {
						strVal.Format("-");
						if (m_DefectList.GetItemText(k, z + 2).Compare(strVal) != 0)
							m_DefectList.SetItemText(k, z + 2, strVal);
					}
					strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][j].lPviDefectCounter[x][y]);
					if (m_DefectList.GetItemText(k, j + 2).Compare(strVal) != 0)
						m_DefectList.SetItemText(k, j + 2, strVal);
					
					k++;
					nCountTemp++;
				}
			}
		}
	}
	
}

void CStatisticsView::UpdateCentralizedSummary(int nStationId)
{
	if (nStationId < 0 || nStationId >= NO_OF_PORT_CENTRALIZED)
		return;

	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strVal;
	int nIndexListTotal;
	int nTotalCount[5];

	nTotalCount[0] = pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.lTotalUnitsInspected;
	nTotalCount[1] = pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.lTotalUnitsPass;
	if (nTotalCount[0] > 0)
		nTotalCount[2] = double(nTotalCount[1]) / double(nTotalCount[0]) * 100 + 0.04;
	else
		nTotalCount[2] = 0;
	nTotalCount[3] = pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.lTotalUnitsFail;
	nTotalCount[4] = pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.lTotalUnitsFalseRejection;
	for (nIndexListTotal = 0; nIndexListTotal < 5; nIndexListTotal++) {
		strVal.Format("%d", nTotalCount[nIndexListTotal]);
		if (m_listCentralizedInfo.GetItemText(nIndexListTotal + 2, nStationId + 1).Compare(strVal) != 0)
			m_listCentralizedInfo.SetItemText(nIndexListTotal + 2, nStationId + 1, strVal);
	}
}

void CStatisticsView::UpdateCentralizedMachineState(int nStationId)
{
	if (nStationId < 0 || nStationId >= NO_OF_PORT_CENTRALIZED)
		return;

	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strMachineState = pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.strMachineState;
	if (m_listCentralizedInfo.GetItemText(0, nStationId + 1).Compare(strMachineState) != 0) {
		m_listCentralizedInfo.SetItemText(0, nStationId + 1, strMachineState);
	}
}

void CStatisticsView::UpdateCentralizedMachineState(int nStationId, COLORREF colorState)
{
	if (nStationId < 0 || nStationId >= NO_OF_PORT_CENTRALIZED)
		return;

	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strMachineState = pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.strMachineState;
	if (m_listCentralizedInfo.GetItemText(0, nStationId + 1).Compare(strMachineState) != 0) {
		m_listCentralizedInfo.SetItemText(0, nStationId + 1, strMachineState);
	}
	m_colorInfo5[0][nStationId + 1] = colorState;
}

void CStatisticsView::UpdateCentralizedMachineID(int nStationId)
{
	if (nStationId < 0 || nStationId >= NO_OF_PORT_CENTRALIZED)
		return;

	CApplication* pApp = (CApplication*)AfxGetApp();
	if (pApp->m_strClientMachineID[nStationId] != "") {
		TCHAR name[40];
		LVCOLUMN col;
		// Get column name
		col.mask = LVCF_TEXT;
		col.pszText = name;
		col.cchTextMax = 40;
		m_listCentralizedInfo.GetColumn(0, &col);

		// Set column name
		_tcscpy_s(name, sizeof(name) / sizeof(TCHAR), _T(pApp->m_strClientMachineID[nStationId]));
		m_listCentralizedInfo.SetColumn(nStationId + 1, &col);
	}
}

void CStatisticsView::UpdateCentralizedContents()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	for (int i = 0; i < NO_OF_PORT_CENTRALIZED; i++) {
		if (pApp->m_strClientMachineID[i] != "") {
			UpdateCentralizedMachineID(i);
			UpdateCentralizedMachineState(i);
			UpdateCentralizedSummary(i);
		}
	}
}
void CStatisticsView::UpdateTotalContents()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strVal;
	int nIndexListTotal;
	int *nTotalCount;
	int *nDieTotalCount;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	nTotalCount = new int[pApp->m_nNo_of_Defects];
	nDieTotalCount = new int[pApp->m_nNo_of_Defects];

	int nIndex = 0;
	int nTempCountIndex = 0;
	for (int nDevice = 0; nDevice < m_nNoOfDies; nDevice++) {
		for (int j = 0; j < pApp->m_nNo_of_Defects; j++)
		{
			nTotalCount[j] = 0;
			nDieTotalCount[j] = 0;
		}
		int nColNo;
		
		nTotalCount[0] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalUnitsInspected;
		nTotalCount[1] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalUnitsPass;
		if (nTotalCount[0] > 0)
			nTotalCount[2] = double(nTotalCount[1]) / double(nTotalCount[0]) * 100 + 0.04;
		else
			nTotalCount[2] = 0;
		nTotalCount[3] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalUnitsFail;
		nTotalCount[4] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lUserModifiedFalseRejection;
		
		for (nIndexListTotal = 0; nIndexListTotal < 5; nIndexListTotal++) {
			strVal.Format("%d", nTotalCount[nIndexListTotal]);
			if (m_listTotalInfo.GetItemText(nIndexListTotal, nDevice + 1).Compare(strVal) != 0)
				m_listTotalInfo.SetItemText(nIndexListTotal, nDevice + 1, strVal);
		}
		
		nTotalCount[5] =  pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lDieLocation;
		nTotalCount[6] =  pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lDatumLocation;
		nTotalCount[7] =  pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTiltInsp;
		nTotalCount[8] =  pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lCoverLayerInspection;
		nTotalCount[9] =  pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lCoverLayerAlignment;
		nTotalCount[10] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapInsufficient;
		nTotalCount[11] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapCrack;
		nTotalCount[12] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapExcess;
		nTotalCount[13] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapBlowHole;
		nTotalCount[14] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapContamination;
		nTotalCount[15] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lSlot;
		nTotalCount[16] = pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lDieEdge;
				/*for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
					for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
						strVal.Format("%d", pApp->m_LotSummary.m_DisplaySummary[nDevice][i][j].lPviDefectCounter[x][y]);
					}
				}*/
		
		int nRowIndex =0;
		for (nRowIndex = nIndexListTotal; nRowIndex < 17; nRowIndex++) {
			strVal.Format("%d", nTotalCount[nRowIndex]);
			if (m_listTotalInfo.GetItemText(nRowIndex + 1, nDevice + 1).Compare(strVal) != 0)
				m_listTotalInfo.SetItemText(nRowIndex + 1, nDevice + 1, strVal);
		}
		nRowIndex++;

		/*for (int j = 0; j < pApp->m_nTotalViews; j++) {
			for (int i = 0; i < m_nNoOfFov; i++) {
				for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
					for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
						strVal.Format("%d", pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalPviDefectCounter[i][j][x][y]);
						if (m_listTotalInfo.GetItemText(nRowIndex, nDevice + 1).Compare(strVal) != 0)
							m_listTotalInfo.SetItemText(nRowIndex, nDevice + 1, strVal);
						nRowIndex++;
						nTempCountIndex++;
					}
				}
			}
		}*/

		if (pApp->m_LotSummary.m_mapIndividualPviDefectCounter.size() > 0) {
			for (std::map<int, long>::iterator it = pApp->m_LotSummary.m_mapIndividualPviDefectCounter.begin();
				it != pApp->m_LotSummary.m_mapIndividualPviDefectCounter.end(); it++) {
				strVal.Format("%d", it->second);
				if (m_listTotalInfo.GetItemText(nRowIndex + 1, nDevice + 1).Compare(strVal) != 0)
					m_listTotalInfo.SetItemText(nRowIndex + 1, nDevice + 1, strVal);
				nRowIndex++;
				nTempCountIndex++;
			}
		}
	}

	int nRowIndex = 0;
	nTempCountIndex = 0;
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		nRowIndex = 0;
		nDieTotalCount[0] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lTotalUnitsInspected;
		strVal.Format("%d", nDieTotalCount[0]);
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);

		nRowIndex++;
		nDieTotalCount[1] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lTotalUnitsPass;
		strVal.Format("%d", nDieTotalCount[1]);
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		if (nDieTotalCount[0] > 0)
			nDieTotalCount[2] = double(nDieTotalCount[1]) / double(nDieTotalCount[0]) * 100 + 0.04;
		else
			nDieTotalCount[2] = 0;
		strVal.Format("%d", nDieTotalCount[2]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[3] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lTotalUnitsFail;
		strVal.Format("%d", nDieTotalCount[3]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[4] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lUserModifiedFalseRejection;
		strVal.Format("%d", nDieTotalCount[4]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[5] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lDieLocation;
		strVal.Format("%d", nDieTotalCount[5]);
		nRowIndex++;
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);

		nDieTotalCount[6] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lDatumLocation;

		strVal.Format("%d", nDieTotalCount[6]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[7] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lTiltInsp;
		strVal.Format("%d", nDieTotalCount[7]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[8] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lCoverLayerInspection;
		strVal.Format("%d", nDieTotalCount[8]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[8] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lCoverLayerAlignment;
		strVal.Format("%d", nDieTotalCount[9]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[9] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lEncapInsufficient;
		strVal.Format("%d", nDieTotalCount[10]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[10] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lEncapExcess;
		strVal.Format("%d", nDieTotalCount[11]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[11] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lEncapCrack;
		strVal.Format("%d", nDieTotalCount[12]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[12] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lEncapBlowHole;
		strVal.Format("%d", nDieTotalCount[13]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[13] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lEncapContamination;
		strVal.Format("%d", nDieTotalCount[14]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[14] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lSlot;
		strVal.Format("%d", nDieTotalCount[15]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		nDieTotalCount[15] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lDieEdge;
		strVal.Format("%d", nDieTotalCount[16]);
		nRowIndex++;
		if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
			m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);

		//nRowIndex++;
		////nRowIndex = nRowIndex + nTempCountIndex;
		//for (int j = 0; j < pApp->m_nTotalViews; j++) {
		//	for (int i = 0; i < m_nNoOfFov; i++){
		//		for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
		//			for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
		//				nDieTotalCount[nRowIndex] += pApp->m_LotSummary.m_TotalDisplaySummary[nDie].lTotalPviDefectCounter[i][j][x][y];
		//				strVal.Format("%d", nDieTotalCount[nRowIndex]);
		//				if (m_listTotalInfo.GetItemText(nRowIndex, m_nNoOfDies + 1).Compare(strVal) != 0)
		//					m_listTotalInfo.SetItemText(nRowIndex, m_nNoOfDies + 1, strVal);
		//				nRowIndex++;
		//			}
		//		}
		//	}
		//}
	}
	delete[] nTotalCount;
	delete[] nDieTotalCount;
}

void CStatisticsView::AddItem(CListCtrl* pCtrl, CString strItem, int nIndex)
{
	LV_ITEM lvitem;

	lvitem.iSubItem = nIndex;
	lvitem.pszText = strItem.GetBuffer(0); 
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = pCtrl->GetItemCount();
	pCtrl->InsertItem(&lvitem);
}

void CStatisticsView::AddColumn(CListCtrl* pCtrl, CString strHeader, int nIndex, int nWidth)
{
	LVCOLUMN	lvColumn;
	memset(&lvColumn, 0, sizeof(lvColumn));

	lvColumn.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCF_ORDER | LVCF_FMT;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = nWidth;
	lvColumn.pszText = strHeader.GetBuffer(0);
	lvColumn.cchTextMax = strHeader.GetLength();
	lvColumn.iSubItem = -1;
	lvColumn.iOrder = nIndex;
	pCtrl->InsertColumn(0, &lvColumn);
}

void CStatisticsView::OnCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	*pResult = 0;		// Initialize with Default processing
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each item
	else if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		*pResult = CDRF_NOTIFYSUBITEMDRAW;	// Request msg for each sub-item
	else if(pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
		COLORREF clrList1 = RGB( 70, 70, 70);
		COLORREF clrList2 = RGB( 40, 40, 40);
		pLVCD->clrText = m_colorInfo1[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem];
		pLVCD->clrTextBk = pLVCD->nmcd.dwItemSpec % 2 ? clrList1 : clrList2;
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}

void CStatisticsView::OnCustomdrawList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	*pResult = 0;		// Initialize with Default processing
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each ite
	else if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		*pResult = CDRF_NOTIFYSUBITEMDRAW;	// Request msg for each sub-item
	else if(pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
		COLORREF clrList1 = RGB( 70, 70, 70);
		COLORREF clrList2 = RGB( 40, 40, 40);

		pLVCD->clrText = m_colorInfo2[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem];
		pLVCD->clrTextBk = pLVCD->nmcd.dwItemSpec % 2 ? clrList1 : clrList2;
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}


void CStatisticsView::OnCustomdrawList3(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	*pResult = 0;		// Initialize with Default processing
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each ite
	else if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		*pResult = CDRF_NOTIFYSUBITEMDRAW;	// Request msg for each sub-item
	else if(pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
		COLORREF clrList1 = RGB( 70, 70, 70);
		COLORREF clrList2 = RGB( 40, 40, 40);

		pLVCD->clrText = m_colorInfo3[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem];
		pLVCD->clrTextBk = pLVCD->nmcd.dwItemSpec % 2 ? clrList1 : clrList2;
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}

void CStatisticsView::OnCustomdrawList4(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = 0;		// Initialize with Default processing
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each ite
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		*pResult = CDRF_NOTIFYSUBITEMDRAW;	// Request msg for each sub-item
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
		COLORREF clrList1 = RGB(70, 70, 70);
		COLORREF clrList2 = RGB(40, 40, 40);

		pLVCD->clrText = m_colorInfo4[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem];
		pLVCD->clrTextBk = pLVCD->nmcd.dwItemSpec % 2 ? clrList1 : clrList2;
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}

void CStatisticsView::OnCustomdrawList5(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = 0;		// Initialize with Default processing
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each ite
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		*pResult = CDRF_NOTIFYSUBITEMDRAW;	// Request msg for each sub-item
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
		COLORREF clrList1 = RGB(70, 70, 70);
		COLORREF clrList2 = RGB(40, 40, 40);

		pLVCD->clrText = m_colorInfo5[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem];
		pLVCD->clrTextBk = pLVCD->nmcd.dwItemSpec % 2 ? clrList1 : clrList2;
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}

void CStatisticsView::InitializeLotStats()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	if(!pApp->m_bIsMasterApp)
		return;

	LOTSUMMARY* pLotSummary = &pApp->m_LotSummary;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strCurDate;
	strCurDate.Format("%02u-%02u-%04u", sysTime.wMonth, sysTime.wDay, sysTime.wYear);
	if(strCurDate == pLotSummary->strDate)
		return;

	pLotSummary->strDate = strCurDate;

	time(&pLotSummary->tmStartTimeHr);
	GetLocalTime(&pLotSummary->timePrevRecord);

	pLotSummary->strStartTimeHr.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	pLotSummary->strStartTimeDay = pLotSummary->strStartTimeHr;

	CString str = pApp->m_LotSummaryRootDir;

	//////////For UserDefined Statistics reset
	/*
	pApp->m_LotSummaryLotIDDir.Format("%s\\xx_%02u-%02u-%02u", pApp->m_LotSummaryRootDir.GetBuffer(0),
		sysTime.wYear%100, sysTime.wMonth, sysTime.wDay);
		*/
	/*pApp->m_LotSummaryLotIDDir.Format("%s\\%02u-%02u-%02u", pApp->m_LotSummaryRootDir.GetBuffer(0),
		sysTime.wYear%100, sysTime.wMonth, sysTime.wDay);*/

	pApp->m_LotSummaryLotIDDir.Format("%s", pApp->m_LotSummaryRootDir.GetBuffer(0));

	OutputDebugLogTo(9,TRUE,"InitializeLotStats: m_LotSummaryLotIDDir[%s]", pApp->m_LotSummaryLotIDDir);
	//_mkdir(pApp->m_LotSummaryLotIDDir);
	
	//_mkdir(pApp->m_LotSummaryLotIDDir + "\\Hourly Stats");

	//CreateHourlyFolder(sysTime);

	/////////////////////////////
}

//void CStatisticsView::RecordLotStats(BOOL bForced)
//{
//	CApplication* pApp = (CApplication*)AfxGetApp();
//	if(!pApp->m_bIsMasterApp)
//		return;
//
//	if(pApp->m_LotSummaryLotIDDir.IsEmpty())
//		return;
//
//	LOTSUMMARY* pLotSummary = &pApp->m_LotSummary;
//
//	SYSTEMTIME sysTime;
//	GetLocalTime(&sysTime);
//
//	time_t timeCurrent;
//	time(&timeCurrent);
//	//if(difftime(timeCurrent, pLotSummary->tmStartTimeHr) >= 3600 || sysTime.wDay != pLotSummary->timePrevRecord.wDay || bForced) {
//	//	pLotSummary->strEndTimeHr.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
//	//	CreateHourlyFolder(sysTime);
// //       /////////////////// 
//	//	UpdateLotHourlyInfo();
//	//	if(!bForced) {
//	//		for (int nDie = 0; nDie<MAX_DIES; nDie++)
//	//			for (int j = 0; j<MAX_FOV; j++)
//	//				for (int i = 0; i<MAX_VIEWS; i++)
//	//					pLotSummary->m_HourlySummary[nDie][j][i].Init();
//	//	}
//	//	Sleep(50);
//
//	//	time(&pLotSummary->tmStartTimeHr);
//	//	pLotSummary->strStartTimeHr = pLotSummary->strEndTimeHr;
//	//}
//
////	if(bForced || sysTime.wDay != pLotSummary->timePrevRecord.wDay) {
////		pLotSummary->strEndTimeDay.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
////		UpdateLotDailyInfo();
////		Sleep(50);
////		if(!bForced) {
////			for (int nDie = 0; nDie<MAX_DIES; nDie++)
////				for (int j = 0; j<MAX_FOV; j++)
////					for (int i = 0; i<MAX_VIEWS; i++)
////						pLotSummary->m_DailySummary[nDie][j][i].Init();
////
////			pLotSummary->strDate.Format("%02u-%02u-%04u", sysTime.wMonth, sysTime.wDay, sysTime.wYear);
////			pLotSummary->strStartTimeDay = pLotSummary->strEndTimeDay;
////
////			//////////////////////////////For UserDefined Statistics reset
////			/*
////			pApp->m_LotSummaryLotIDDir.Format("%s\\xx_%02u-%02u-%02u", pApp->m_LotSummaryRootDir.GetBuffer(0),
////				sysTime.wYear%100, sysTime.wMonth, sysTime.wDay);
////
////			*/
////			pApp->m_LotSummaryLotIDDir.Format("%s\\%02u-%02u-%02u", pApp->m_LotSummaryRootDir.GetBuffer(0),
////				sysTime.wYear%100, sysTime.wMonth, sysTime.wDay);
////
////
////			_mkdir(pApp->m_LotSummaryLotIDDir);
////			///_mkdir(pApp->m_LotSummaryLotIDDir + "\\Hourly Stats");
////
////////////////////////////////////////////////////////////
////
////
////			CFileFind FindDaily;
////			CString strLotDir;
////			strLotDir.Format("%s\\*.*", pApp->m_LotSummaryRootDir.GetBuffer(0));
////			BOOL bDailyFound = FindDaily.FindFile(strLotDir);
////
////			CTime curTime = CTime::GetCurrentTime();
////			CTime fileTime;
////			CTimeSpan timeSpan;
////			while(bDailyFound) {
////				bDailyFound = FindDaily.FindNextFile();
////				if(FindDaily.IsDots())
////					continue;
////
////				if(FindDaily.IsDirectory()) {
////					FindDaily.GetCreationTime(fileTime);
////					timeSpan = curTime - fileTime;
////					if(timeSpan.GetDays() >= pLotSummary->nLotRecycleDays) {
////						strLotDir = pApp->m_LotSummaryRootDir + "\\" + FindDaily.GetFileTitle();
////						DeleteDir(strLotDir);
////					}
////				}
////			}
////		}
////	}
//
//	int nCurMins = sysTime.wHour*60 + sysTime.wMinute;
//	int nPrevMins = pLotSummary->timePrevRecord.wHour*60 + pLotSummary->timePrevRecord.wMinute;
//	if(nCurMins >= pLotSummary->nDisplayStatsResetMin &&
//			nPrevMins < pLotSummary->nDisplayStatsResetMin) {
//		pLotSummary->InitDisplaySummary();
//	}
//
//	pLotSummary->timePrevRecord = sysTime;
//}

//void CStatisticsView::UpdateLotHourlyInfo()
//{
//	CApplication* pApp = (CApplication*)AfxGetApp();
//	LOTSUMMARY* pLotSummary = &((CApplication*)AfxGetApp())->m_LotSummary;
//
//	int nTotalUnitsInsp = 0, nTotalUnitsPass = 0, nTotalUnitsFail = 0, nGrabRetry = 0;
//	int nBottom = 0;
//	int nSide = 0;
//	int nTotalDefectCount[5][8][12][25];
//	int nDefects = 25;
//
//	
//	CString strStartTime, strEndTime, strHourlyLotFile;
//	if (!pLotSummary->strStartTimeHr.IsEmpty())
//		strStartTime.Format("%c%c%c%c", pLotSummary->strStartTimeHr[0], pLotSummary->strStartTimeHr[1],
//			pLotSummary->strStartTimeHr[3], pLotSummary->strStartTimeHr[4]);
//	if (!pLotSummary->strEndTimeHr.IsEmpty())
//		strEndTime.Format("%c%c%c%c", pLotSummary->strEndTimeHr[0], pLotSummary->strEndTimeHr[1],
//			pLotSummary->strEndTimeHr[3], pLotSummary->strEndTimeHr[4]);
//
//	/////////////////////For UserDefined Statistics reset
//	//strHourlyLotFile.Format(pApp->m_LotSummaryLotIDDir + "\\Hourly Stats\\%s-%s.hr", 
//	//strStartTime, strEndTime);
//
//	strHourlyLotFile.Format(pApp->m_LotSummaryLotIDDir + "\\" + m_strHourlyFolder + "\\%s-%s.csv",
//		strStartTime, strEndTime);
//	//////////////////////////////////
//
//	OutputDebugLog("UpdateLotHourlyInfo:strHourlyLotFile = %s", strHourlyLotFile);
//	FILE* file;
//	errno_t err = fopen_s(&file, strHourlyLotFile, "a+");
//	if (err || file == NULL)
//		return;
//
//	for (int k = 0; k < pApp->m_nNoOfDies; k++)
//	{
//		nBottom = 0;
//		nSide = 0;
//		fprintf(file, "\n\nDevice %d\n", k + 1);
//		for (int i = 0; i < pApp->m_nNoOfFov; i++)
//		{
//			fprintf(file, "\n,Position %9d\n", i + 1);
//			fprintf(file, ",,,Total units Inspected,Total Units Pass,Total Yield,Total Unit Fail,Die Location,Datum Location,Tilt Inspection,Cover Layer Inspection,Encap Insufficient,Encap Excess,Die Inspection,Pvi Inspection");
//			nBottom = 0;
//			nSide = 0;
//			for (int j = 0; j<pApp->m_nTotalViews; j++)
//			{
//
//				if (j < pApp->m_nTotalViewDocs[0]) {
//					fprintf(file, "\n,,%s", pApp->m_strTopCameraName[j]);
//				}
//				else if (j >= pApp->m_nTotalViewDocs[0] && j < (pApp->m_nTotalViewDocs[1] + pApp->m_nTotalViewDocs[0])) {
//					fprintf(file, "\n,,%s", pApp->m_strBottomCameraName[nBottom]);
//					nBottom++;
//				}
//				else
//				{
//					fprintf(file, "\n,,%s", pApp->m_strSideCameraName[nSide]);
//					nSide++;
//				}
//				nTotalDefectCount[k][i][j][0] = pApp->m_LotSummary.m_DailySummary[k][i][j].lTotalUnitsInspected;
//				nTotalDefectCount[k][i][j][1] = pApp->m_LotSummary.m_DailySummary[k][i][j].lTotalUnitsPass;
//				nTotalDefectCount[k][i][j][2] = pApp->m_LotSummary.m_DailySummary[k][i][j].dTotalYield;
//				nTotalDefectCount[k][i][j][3] = pApp->m_LotSummary.m_DailySummary[k][i][j].lTotalUnitsFail;
//				nTotalDefectCount[k][i][j][4] = pApp->m_LotSummary.m_DailySummary[k][i][j].lDieLocation;
//				nTotalDefectCount[k][i][j][5] = pApp->m_LotSummary.m_DailySummary[k][i][j].lDatumLocation;
//				nTotalDefectCount[k][i][j][6] = pApp->m_LotSummary.m_DailySummary[k][i][j].lTiltInsp;
//				nTotalDefectCount[k][i][j][7] = pApp->m_LotSummary.m_DailySummary[k][i][j].lCoverLayerInspection;
//				nTotalDefectCount[k][i][j][8] = pApp->m_LotSummary.m_DailySummary[k][i][j].lEncapInsufficient;
//				nTotalDefectCount[k][i][j][9] = pApp->m_LotSummary.m_DailySummary[k][i][j].lEncapExcess;
//				nTotalDefectCount[k][i][j][10] = pApp->m_LotSummary.m_DailySummary[k][i][j].lDieEdge;
//
//
//
//				fprintf(file, ",%9d,%9d,%9d,%9d,%9d,%9d,%9d,%9d,%9d,%9d,%9d,%9d", nTotalDefectCount[k][i][j][0],
//					nTotalDefectCount[k][i][j][1], nTotalDefectCount[k][i][j][2], nTotalDefectCount[k][i][j][3], nTotalDefectCount[k][i][j][4],
//					nTotalDefectCount[k][i][j][5], nTotalDefectCount[k][i][j][6], nTotalDefectCount[k][i][j][7], nTotalDefectCount[k][i][j][8],
//					nTotalDefectCount[k][i][j][9], nTotalDefectCount[k][i][j][10], nTotalDefectCount[k][i][j][11]);
//			}
//		}
//	}
//	fclose(file);
//}

void CStatisticsView::UpdateLotInfo()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	LOTSUMMARY* pLotSummary = &((CApplication*)AfxGetApp())->m_LotSummary;
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	
	int nBottom = 0;
	int nSide = 0;
	int *nTotalDefectCount[5][8][12];

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 12; k++) {
				nTotalDefectCount[i][j][k] = new int[pApp->m_nNo_of_Defects];
			}
		}
	}

	int nDefects = 25;
	
	CString strLotFile;

	CString strTemp;
	if (!pApp->m_strLotInfo.IsEmpty()) {
		strTemp.Format("%s\\%s", pApp->m_LotSummaryRootDir, pApp->m_strLotConfigFile);
		_mkdir(strTemp);
		strLotFile = pApp->m_strLotInfo;
	}
	else
	{
		strTemp.Format("%s\\DUMMY LOT CONFIG", pApp->m_LotSummaryRootDir);
		_mkdir(strTemp);
		strLotFile = "DUMMY LOT";
	}
	CString strTempDir;
	strTempDir.Format("%s\\%s", strTemp, strLotFile);
	_mkdir(strTempDir);

	strLotFile = strTempDir + "\\" + strLotFile + ".csv";
	FILE* file;
	errno_t err = fopen_s(&file, strLotFile, "w");
	if (err || file == NULL)
		return;

	fprintf(file, "Lot ID,,[%s]\n", pApp->m_strLotInfo);
	fprintf(file, "Operator ID,,%s\n", pApp->m_UserAccInfo.m_strCurUserName);
	fprintf(file, "Recipe Name,,%s\n", pApp->m_strLotConfigFile);
	fprintf(file, "System ID,,%s\n", pLotSummary->strMachineID);
	fprintf(file, "Start Time,,[%s]\n", pLotSummary->strStartLotTime);
	fprintf(file, "End Time,,[%s]\n", pLotSummary->strEndLotTime);
	//fprintf(file, "Comments,,%s\n", pLotSummary->strComments);

	for (int k = 0; k < m_nNoOfDies; k++)
	{
		nBottom = 0;
		nSide = 0;
		fprintf(file, "\n\nTHA %d\n", k + 1);
		for (int i = 0; i < m_nNoOfFov; i++)
		{
			fprintf(file, "\n,Position %9d\n", i + 1);
			strTemp.Format(",,,Total Inspected,Total Pass,Total Yield(%%),Total Fail,User Modified,Die Location,Datum Location,Tilt Inspection,Cover Layer Inspection,Cover Layer Alignment,Encap Insufficient,Encap Excess,Encap Crack,Encap BlowHole,Encap Contamination,Slot,Die Edge");
			int nCount = pApp->m_strPviDefectsStats[i].GetCount();
			for (int nTempIndex = 0; nTempIndex < nCount; nTempIndex++) {
				strTemp.Format("%s,%s", strTemp, pApp->m_strPviDefectsStats[i].GetAt(nTempIndex));
			}
			fprintf(file, strTemp);
			nBottom = 0;
			nSide = 0;
			int nTempCountPviIndex=0;
			for (int j = 0; j<pApp->m_nTotalViews; j++)
			{

				if (j < pApp->m_nTotalViewDocs[0]) {
					fprintf(file, "\n,,%s", pApp->m_strTopCameraName[j]);
				}
				else if (j >= pApp->m_nTotalViewDocs[0] && j < (pApp->m_nTotalViewDocs[1] + pApp->m_nTotalViewDocs[0])) {
					fprintf(file, "\n,,%s", pApp->m_strBottomCameraName[nBottom]);
					nBottom++;
				}
				else
				{
					fprintf(file, "\n,,%s", pApp->m_strSideCameraName[nSide]);
					nSide++;
				}
				for (int nTemp = 0; nTemp < pApp->m_nNo_of_Defects; nTemp++) {
					nTotalDefectCount[k][i][j][nTemp] = -1;
				}

				int nDefectIndexTemp = 0;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lTotalUnitsInspected;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lTotalUnitsPass;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].dTotalYield;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lTotalUnitsFail;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lUserModifiedFalseAcceptance;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lDieLocation;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lDatumLocation;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lTiltInsp;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lCoverLayerInspection;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lCoverLayerAlignment;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lEncapInsufficient;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lEncapExcess;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lEncapCrack;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lEncapBlowHole;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lEncapContamination;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lSlot;
				nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lDieEdge;
				int nTempCount = nDefectIndexTemp;
				nDefectIndexTemp = nDefectIndexTemp + nTempCountPviIndex;
				for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
					for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
						nTotalDefectCount[k][i][j][nDefectIndexTemp++] = pApp->m_LotSummary.m_DisplaySummary[k][i][j].lPviDefectCounter[x][y];
						nTempCountPviIndex++;
					}
				}

				for (int nTempIndex = 0; nTempIndex < nTempCount+nCount; nTempIndex++) {
					fprintf(file, ",%9d", nTotalDefectCount[k][i][j][nTempIndex]);
				}
			}
		}
	}
	fclose(file);
	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 12; k++) {
				delete[] nTotalDefectCount[i][j][k];
			}
		}
	}
}


void CStatisticsView::UpdateLotInfoMID(int nDie)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	LOTSUMMARY* pLotSummary = &((CApplication*)AfxGetApp())->m_LotSummary;
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
 
	
	CString strLotFile;

	CString strTemp1;
	if (!pApp->m_strLotInfo.IsEmpty()) {
		strTemp1.Format("%s\\%s", pApp->m_LotSummaryRootDir, pApp->m_strLotConfigFile);
		_mkdir(strTemp1);
		strLotFile = pApp->m_strLotInfo;
	}
	else
	{
		strTemp1.Format("%s\\DUMMY LOT CONFIG", pApp->m_LotSummaryRootDir);
		_mkdir(strTemp1);
		strLotFile = "DUMMY LOT";
	}
	CString strTempDir;
	strTempDir.Format("%s\\%s", strTemp1, strLotFile);
	_mkdir(strTempDir);

	strLotFile = strTempDir + "\\" + strLotFile + "_Info.csv";
	
	FILE* file;
	//First time create file or not
	BOOL bEmptyFile = FALSE;
	char ch;
	errno_t err = fopen_s(&file, strLotFile, "r");
	if (err || file == NULL) {
		bEmptyFile = TRUE;
	}
	else {
		if (fscanf(file, "%c", &ch) == EOF)
			bEmptyFile = TRUE;
		fclose(file);
	}

	err = fopen_s(&file, strLotFile, "a+");
	if (err || file == NULL)
		return;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strCurDate, strCurTime;
	strCurDate.Format("%02u-%02u-%04u", sysTime.wMonth, sysTime.wDay, sysTime.wYear);
	strCurTime.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	if (!m_bWriteFirstLot || bEmptyFile) {
		fprintf(file, "%s\n", "\n\nLOT SUMMARY");
		fprintf(file, "%s\n", "----------------------");

		fprintf(file, " Lot ID,,[%s]\n", pApp->m_strLotInfo);
		fprintf(file, " Recipe Name,,%s\n", pApp->m_strLotConfigFile);
		fprintf(file, " Machine ID  %s\n", pLotSummary->strMachineID);
		fprintf(file, " Start Date  %s\n", pLotSummary->strDate);

		fprintf(file, "-----------------------------------\n");
	}
	
	std::fprintf(file, "\n\n,,,Date,Time,MID,FOV,FOV Fail Code,FOV Fail Defect Name,Die Fail Code,Die Fail Defect Name\n");
	for (int i = 0; i < m_nNoOfFov; i++) {

		int nFOVDefectCode = pApp->m_pImageSelectionDlg->m_wndDlg.getErrorDefectCode(pApp->m_nFovPartErrorCode[nDie][i]);
		CString strErrorDefectName;
		if (pApp->m_nFovPartErrorCode[nDie][i] < GOOD_UNIT) {
			if (pApp->m_nFovPartErrorCode[nDie][i] > -ERROR_CODES || pApp->m_nFovPartErrorCode[nDie][i] == -PROCESS_ERROR) {
				strErrorDefectName.Format("%s ", pApp->GetFailedStr(pApp->m_nFovPartErrorCode[nDie][i], FALSE));
			}
			else {
				strErrorDefectName = "PVI Defect";
				if (pApp->m_mapPviDefectsStatsGeneral.find(pApp->m_nFovPartErrorCode[nDie][i]) != pApp->m_mapPviDefectsStatsGeneral.end()) {
					strErrorDefectName.Format("%s", pApp->m_mapPviDefectsStatsGeneral[pApp->m_nFovPartErrorCode[nDie][i]]);
				}
			}

		}
		else {
			strErrorDefectName = "PASS";
		}

		int nDieDefectCode = pApp->m_pImageSelectionDlg->m_wndDlg.getErrorDefectCode(pApp->m_nPartErrorCode[nDie]);
		CString strDieErrorDefectName;
		if (pApp->m_nPartErrorCode[nDie] < GOOD_UNIT) {
			if (pApp->m_nPartErrorCode[nDie] > -ERROR_CODES || pApp->m_nPartErrorCode[nDie] == -PROCESS_ERROR) {
				strDieErrorDefectName.Format("%s ", pApp->GetFailedStr(pApp->m_nPartErrorCode[nDie], FALSE));
			}
			else {
				strDieErrorDefectName = "PVI Defect";
				if (pApp->m_mapPviDefectsStatsGeneral.find(pApp->m_nPartErrorCode[nDie]) != pApp->m_mapPviDefectsStatsGeneral.end()) {
					strDieErrorDefectName.Format("%s", pApp->m_mapPviDefectsStatsGeneral[pApp->m_nPartErrorCode[nDie]]);
				}
			}

		}
		else {
			strDieErrorDefectName = "PASS";
		}

		fprintf(file, ",,,%s,%s,%s,%d,%d,%s,%d,%s\n", strCurDate, strCurTime, pApp->m_strPartFlex[nDie], i + 1,
			nFOVDefectCode, strErrorDefectName, nDieDefectCode,strDieErrorDefectName);
	}
	
	std::fprintf(file, "\n");
	fclose(file);
	
}

void CStatisticsView::CreateHourlyFolder(SYSTEMTIME sysTime)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	int nNoOfResetPerDay,nLotResetTime1,nLotResetTime2;
	
    nLotResetTime1   = pApp->tLotResetTime1.wHour;
	nLotResetTime2	 = pApp->tLotResetTime2.wHour;
	nNoOfResetPerDay = pApp->nNoOfResetPerDay;
	
	if(0 == nNoOfResetPerDay)
	{
		m_strHourlyFolder.Format("HourlyStats_0.00-23.59");
	}
	
	if(1 == nNoOfResetPerDay)
	{
		if(sysTime.wHour < nLotResetTime1)
			m_strHourlyFolder.Format("HourlyStats_0.00-%d.00",nLotResetTime1);
		
		if(sysTime.wHour >= nLotResetTime1)
			m_strHourlyFolder.Format("HourlyStats_%d.00-23.59",nLotResetTime1);
		
	}
	
	if(2 == nNoOfResetPerDay)
	{
		if(nLotResetTime1 < nLotResetTime2)
		{
			if(sysTime.wHour < nLotResetTime1)
				m_strHourlyFolder.Format("HourlyStats_0.00-%d.00",nLotResetTime1);
			
			if((sysTime.wHour >= nLotResetTime1)&&(sysTime.wHour < nLotResetTime2))
				m_strHourlyFolder.Format("HourlyStats_%d.00-%d.00",nLotResetTime1,nLotResetTime2);
			
			if(sysTime.wHour >= nLotResetTime2)
				m_strHourlyFolder.Format("HourlyStats_%d.00-23.59",nLotResetTime2);
		}
		
		if(nLotResetTime1 > nLotResetTime2)
		{
			if(sysTime.wHour < nLotResetTime2)
				m_strHourlyFolder.Format("HourlyStats_0.00-%d.00",nLotResetTime2);
			
			if((sysTime.wHour >= nLotResetTime2)&&(sysTime.wHour < nLotResetTime1))
				m_strHourlyFolder.Format("HourlyStats_%d.00-%d.00",nLotResetTime2,nLotResetTime1);
			
			if(sysTime.wHour >= nLotResetTime1)
				m_strHourlyFolder.Format("HourlyStats_%d.00-23.59",nLotResetTime1);
		}
		
		if(nLotResetTime1 == nLotResetTime2)
		{
			if(sysTime.wHour < nLotResetTime1)
				m_strHourlyFolder.Format("HourlyStats_0.00-%d.00",nLotResetTime1);
			
			if(sysTime.wHour >= nLotResetTime1)
				m_strHourlyFolder.Format("HourlyStats_%d.00-23.59",nLotResetTime1);
		}
	}
	_mkdir(pApp->m_LotSummaryLotIDDir + "\\" + m_strHourlyFolder);
	
    
}

void CStatisticsView::CheckAutoLogOff()
{
	time_t timeCurrent;
	time(&timeCurrent);
	CApplication* pApp = (CApplication*)AfxGetApp();
	
	// no auto log off for operator to prevent no operator recorded in report
	if(pApp->m_UserAccInfo.m_nCurrentAccessLevel == 0)
		return;

	if(difftime(timeCurrent, pApp->m_UserAccInfo.tmStartLogoutTimeHr) >= (60 * pApp->m_UserAccInfo.m_nLogoutTime)) {
		if(pApp->m_UserAccInfo.m_nCurrentAccessLevel>0){ // operator will not auto log out
			if(!pApp->m_bOnline){ // only auto log off in offline to prevent inspection run withour userID
				pApp->OnUserLogOut();
				time(&pApp->m_UserAccInfo.tmStartLogoutTimeHr);
			}
		}	
	}
}

void CStatisticsView::OnTimer(UINT_PTR nIDEvent)
{
	CheckAutoLogOff();
	//RecordLotStats();
	CDialogEx::OnTimer(nIDEvent);
}

HBRUSH CStatisticsView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//if(pWnd->GetDlgCtrlID() == IDC_STATISTICS_CAPTION) {
 //       pDC->SetTextColor(RGB(80,180,255));
 //       pDC->SetBkMode(TRANSPARENT);
	//}
 // 
	//return m_brush;
	return 0;
}

void CStatisticsView::OnLvnItemchangedSummaryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//// TODO: Add your control notification handler code here
	//*pResult = 0;
}

BOOL CStatisticsView::OnEraseBkgnd(CDC* pDC) 
{
	//CRect rect;
 //   GetClientRect(&rect);
 //   pDC->FillRect(&rect, &m_brush);
	CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
	
	//return CFormView::OnEraseBkgnd(pDC);
}

///Udupa; Jan'2013
int CStatisticsView::UpdateAuditTrail(CString strEvent, BOOL bSave)
{
	CApplication* pApp = (CApplication*) AfxGetApp();

	CStdioFile txtFile;
	CFileException e;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strDate, strTime, strOutput, strEncr, strLogFile, strMd5;
	strDate.Format("%04u-%02u-%02u", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	strTime.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	strOutput.Format("%s\t  %s     \t%s\t  %s\n", strTime, pApp->m_UserAccInfo.m_strCurrentAccessLevel, pApp->m_UserAccInfo.m_strCurUserName, strEvent);
	CString strLog = strOutput;
	int nIndex = strLog.Find('%');
	if(nIndex >= 0)
		strLog.Replace("%", "%%");
	OutputDebugLogTo(0, TRUE, "%s", strLog);

	EnterCriticalSection(&m_csLogFile);

	try {
		if(bSave) {
			strLogFile.Format("%s\\LogFilesDir\\LogFile_%s.txt", pApp->m_strParmFolder, strDate);
			_mkdir(pApp->m_strParmFolder + "\\" + "LogFilesDir");

			SetFileAttributes(strLogFile, FILE_ATTRIBUTE_NORMAL);
			BOOL bOpen = txtFile.Open(strLogFile, CFile::modeReadWrite | CFile::shareDenyWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText, &e);

			if(!bOpen) {
				//if(!m_bAuditTrailValidated && !pApp->m_bOnline) {
					AfxMessageBox("Audit Trail Log History is disabled. Please provide valid path for FCI Stats");
					//m_bAuditTrailValidated = TRUE;
				//}
			}
			else {
				strEncr = "";
				int nLength = (int) txtFile.GetLength();
				if(nLength > HASHLEN) {
					char strTemp[HASHLEN+1];
					txtFile.SeekToBegin();
					txtFile.Read(strTemp, HASHLEN);
					strTemp[HASHLEN] = 0;
					CString strSavedMd5 = strTemp;

					txtFile.ReadString(strEncr);
					strMd5 = md5(strEncr);
					if(strMd5.Compare(strSavedMd5)) {
						CString strTemp;
						strTemp.Format("%s  %s>  %s:  Audit Trail Log file is corrupt; Contents flushed\r\n", strDate, strTime, pApp->m_UserAccInfo.m_strCurUserName);
						OutputDebugLogTo(0, TRUE, "%s", strTemp);
						//m_EditInfo.SetSel(0,-1, TRUE);
						//m_EditInfo.ReplaceSel(strTemp);
						strEncr = EncryptDecrypt(strTemp);
						txtFile.SetLength(HASHLEN);
						txtFile.SeekToEnd();
						txtFile.WriteString(strEncr);
					}
				}

				strOutput = EncryptDecrypt(strOutput);

				strEncr += strOutput;
				strMd5 = md5(strEncr);

				txtFile.SeekToBegin();
				txtFile.WriteString(strMd5);
				txtFile.SeekToEnd();
				txtFile.WriteString(strOutput);
				txtFile.Flush();
				txtFile.Close();	

				txtFile.Open(strLogFile, CFile::modeRead | CFile::shareDenyWrite , &e);
				txtFile.Close();

				SetFileAttributes(strLogFile, FILE_ATTRIBUTE_READONLY);
			}
		}

		///Udupa; June'2015
	/*	if(pApp->m_pAuditTrailDialog)
			pApp->m_pAuditTrailDialog->SendMessage(WM_AUDITTRAIL_UPDATE, (WPARAM) &sysTime, (LPARAM) &strEvent);*/
	}
	catch(...) {
		OutputDebugLogTo(9, TRUE, "Exception in UpdateAuditTrail");
	}
	LeaveCriticalSection(&m_csLogFile);

	return 0;
}




IMPLEMENT_DYNAMIC(CDialogPaneStatsview,CDockablePane)
BEGIN_MESSAGE_MAP(CDialogPaneStatsview,CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CDialogPaneStatsview::CDialogPaneStatsview(){}
CDialogPaneStatsview::~CDialogPaneStatsview(){}

int CDialogPaneStatsview::OnCreate(LPCREATESTRUCT lp)
{
	
	if(CDockablePane::OnCreate(lp)==-1)
		return -1;
	if(!m_wndStatsDlg.Create(CStatisticsView::IDD,this))
		return -1;
	//m_wndDlg.Initialize();
	bIsClosed = FALSE;
	bRefreshPane = FALSE;
	m_wndStatsDlg.OnInitialUpdate();
	m_wndStatsDlg.Initialize();
	m_wndStatsDlg.InitializeLotStats();
	m_wndStatsDlg.UpdateContents();
	m_wndStatsDlg.UpdateAuditTrail("FCI Vision Application started",TRUE);
	m_wndStatsDlg.ShowWindow(SW_SHOWDEFAULT);
	return  0;
}
void CDialogPaneStatsview::OnSize(UINT nType,int cx,int cy)
{
	CDockablePane::OnSize(nType,cx,cy);
	//m_wndStatsDlg.SetSize(cx,cy);
	m_wndStatsDlg.SetWindowPos(NULL,0,0,cx,cy,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndStatsDlg.Invalidate();
}

void CDialogPaneStatsview::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();
	bIsClosed = TRUE;
}


void CDialogPaneStatsview::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	return;
}


void CStatisticsView::OnBnClickedButtonSummaryList()
{
	// TODO: Add your control notification handler code here
	UpdateContents();
	m_DefectList.ShowWindow(FALSE);
	m_SummaryList.ShowWindow(TRUE);
	m_ListStatsInfo.ShowWindow(FALSE);
	m_listTotalInfo.ShowWindow(FALSE);
}


void CStatisticsView::OnBnClickedButtonDefectList()
{
	// TODO: Add your control notification handler code here
	UpdateContents();
	m_DefectList.ShowWindow(TRUE);
	m_SummaryList.ShowWindow(FALSE);
	m_ListStatsInfo.ShowWindow(FALSE);
	m_listTotalInfo.ShowWindow(FALSE);

}


void CStatisticsView::OnBnClickedButtonStatsInfo()
{
	// TODO: Add your control notification handler code here
	m_DefectList.ShowWindow(FALSE);
	m_SummaryList.ShowWindow(FALSE);
	m_ListStatsInfo.ShowWindow(TRUE);
	m_listTotalInfo.ShowWindow(FALSE);
}



void CStatisticsView::LoadLotInfo()
{
	 
	CApplication* pApp = (CApplication*)AfxGetApp();
	LOTSUMMARY* pLotSummary = &((CApplication*)AfxGetApp())->m_LotSummary;
	CString strLotFile;
	CString strTemp;
	if (pApp->m_bLotOpened) {
		
		if (!pApp->m_strLotInfo.IsEmpty()) {
			strTemp.Format("%s\\%s\\%s", pApp->m_LotSummaryRootDir, pApp->m_strLotConfigFile, pApp->m_strLotInfo);
			strLotFile = pApp->m_strLotInfo;
		}
		else
		{
			strTemp.Format("%s\\DUMMY LOT", pApp->m_LotSummaryRootDir);
			strLotFile = "DUMMY LOT";
		}
		strLotFile = strTemp + "\\" + strLotFile + ".csv";
		OutputDebugLog("LoadLotInfo %s", strLotFile);
	}	
	else
		return;

	CAtlString resToken;
	CStringArray respone;
	int curPos = 0;
	int nLineCount = 0;
	CString strLine;
	char c[5000];
	int nDevice = -1;
	int nPosition = -1;
	int nView = -1;
	std::ifstream file(strLotFile);
	if (!file.good())
		return;
	while (!file.eof()) {
		
		file.getline(c,5000);
		strLine = c;
		nLineCount = 0;
		curPos = 0;
		respone.RemoveAll();
		do
		{
			resToken = strLine.Tokenize(_T(","), curPos);
			respone.Add((CString)resToken);
			nLineCount++;
		} while (resToken != "");

		//for (int i = 0; i < nLineCount; i++)
		{
			if (!respone[0].Find("THA", 0))
			{
				int nLength = respone[0].GetLength();
				char cDevice = respone[0].GetAt(nLength - 1);
				CString strDevice;
				strDevice.Format("%c", cDevice);
				nDevice = atoi(strDevice);
				/*if (nDevice == 24)
				{
					OutputDebugLogTo(9, TRUE, "BEFORE nDevice = %d ", nDevice);
				}
				OutputDebugLogTo(9, TRUE, "BEFORE nDevice = %d ", nDevice);*/
				nDevice = nDevice - 1;
				nPosition = -1;
			//	OutputDebugLogTo(9, TRUE, "nDevice = %d cDevice = %c", nDevice, cDevice);
			}
			if (!respone[0].Find("Position", 0))
			{
				int nLength = respone[0].GetLength();
				char cDevice = respone[0].GetAt(nLength - 1);
				CString strDevice;
				strDevice.Format("%c", cDevice);
				nPosition = atoi(strDevice);
				nPosition = nPosition - 1;
				nView = -1;;
			}


			for (int nTop = 0; nTop < pApp->m_nTotalViewDocs[0]; nTop++) {
				if (!respone[0].Compare(pApp->m_strTopCameraName[nTop])!=0)
				{
					nView = nTop ;
				}
			}
			
			for (int nTop = 0; nTop < pApp->m_nTotalViewDocs[1]; nTop++) {
				if (!respone[0].Compare(pApp->m_strBottomCameraName[nTop]) != 0)
				{
					nView = nTop + pApp->m_nTotalViewDocs[0];
				}
			}
			for (int nTop = 0; nTop < pApp->m_nTotalViewDocs[2]; nTop++) {
				if (!respone[0].Compare(pApp->m_strSideCameraName[nTop]) != 0)
				{
					nView = nTop + pApp->m_nTotalViewDocs[0] + pApp->m_nTotalViewDocs[1];
				}
			}
			int nData = 1;
			if (nDevice != -1 && nPosition != -1 && nView != -1) {
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lTotalUnitsInspected = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lTotalUnitsPass = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].dTotalYield = atof(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lTotalUnitsFail = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lUserModifiedFalseAcceptance = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lDieLocation = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lDatumLocation = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lTiltInsp = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lCoverLayerInspection = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lCoverLayerAlignment = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lEncapInsufficient = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lEncapExcess = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lEncapCrack = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lEncapBlowHole = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lEncapContamination = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lSlot = atoi(respone[nData++]);
				pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lDieEdge = atoi(respone[nData++]);
				/*while (1) {
					if (atoi(respone[nData])!= -1)
						break;
					else
						nData++;
				}
				for (int x = 0; x < pApp->m_nPviNumberOfArea[nPosition][nView]; x++) {
					for (int y = 0; y < pApp->m_nPviNoOfDefects[nPosition][nView][x]; y++) {
						pApp->m_LotSummary.m_DisplaySummary[nDevice][nPosition][nView].lPviDefectCounter[x][y] = atoi(respone[nData++]);
					}
				}*/
				nView = -1;
			}
		}
	}
	respone.RemoveAll();
}


void CStatisticsView::UpdateStatsInfo()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int j = 0;
	CString strTempVal;
	for (int i = 0; i < m_nNoOfDies; i++,j++) {
		m_ListStatsInfo.SetItemText(j, 1, (LPCTSTR)pApp->m_strPartFlex[i]);
	}	
	for (int i = 0; i < m_nNoOfDies; i++, j++) {
		if (pApp->m_bPartInspect[i])
			strTempVal = "1-Inspect";
		else
			strTempVal = "0-No Inspect";
		m_ListStatsInfo.SetItemText(j, 1, strTempVal);
	}

	m_ListStatsInfo.SetItemText(j++, 1, (LPCTSTR)pApp->m_strLotInfo);
	m_ListStatsInfo.SetItemText(j++, 1, (LPCTSTR)pApp->m_strConfigFile);
	if (nPPMDiesNo > 0)
		strTempVal.Format("%2.02f", static_cast<double>(60000 / dAverageDiesTimeMs));
	else
		strTempVal.Format("0");

	m_ListStatsInfo.SetItemText(j++, 1, (LPCTSTR)strTempVal);
	if (nPPMDiesNo > 0)
		strTempVal.Format("%2.02f", static_cast<double>((60000*60) / dAverageDiesTimeMs));
	else 
		strTempVal.Format("0");

	m_ListStatsInfo.SetItemText(j++, 1, strTempVal);
}


void CStatisticsView::OnCbnSelchangeComboDeviceList()
{
	// TODO: Add your control notification handler code here
	UpdateContents(TRUE);
}


void CStatisticsView::OnBnClickedButtonTotalInfo()
{
	// TODO: Add your control notification handler code here
	UpdateTotalContents();
	m_SummaryList.ShowWindow(FALSE);
	m_DefectList.ShowWindow(FALSE);
	m_ListStatsInfo.ShowWindow(FALSE);
	m_listTotalInfo.ShowWindow(TRUE);
}


void CStatisticsView::OnCbnSelchangeComboPositionList()
{
	// TODO: Add your control notification handler code her
	UpdateContents(TRUE);
}


void CStatisticsView::UpdateTotalLotInfo()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	LOTSUMMARY* pLotSummary = &((CApplication*)AfxGetApp())->m_LotSummary;
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	int nBottom = 0;
	int nSide = 0;
	CArray<int> nTotalDefectCount[5];
	CArray<int> nTotalDefectCountTotal;
	int nDefects = 25;

	/*for (int i = 0; i < 5; i++) {
		nTotalDefectCount[i] = new int[pApp->m_nNo_of_Defects];
	}*/
	//nTotalDefectCountTotal = new int[pApp->m_nNo_of_Defects];
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < pApp->m_nNo_of_Defects; j++) {
			nTotalDefectCount[i].Add(0);
			nTotalDefectCountTotal.Add(0);
		}
	}

	CString strLotFile;

	CString strTemp;
	if (!pApp->m_strLotInfo.IsEmpty()) {
		strTemp.Format("%s\\%s", pApp->m_LotSummaryRootDir, pApp->m_strLotConfigFile);
		_mkdir(strTemp);
		strLotFile = pApp->m_strLotInfo;
	}
	else
	{
		strTemp.Format("%s\\DUMMY LOT CONFIG", pApp->m_LotSummaryRootDir);
		_mkdir(strTemp);
		strLotFile = "DUMMY LOT";
	}
	CString strTempDir;
	strTempDir.Format("%s\\%s", strTemp, strLotFile);
	_mkdir(strTempDir);

	strLotFile = strTempDir + "\\" + strLotFile + "_Total.csv";
	FILE* file;
	errno_t err = fopen_s(&file, strLotFile, "w");
	if (err || file == NULL)
		return;

	fprintf(file, "Lot ID,,[%s]\n", pApp->m_strLotInfo);
	fprintf(file, "Operator ID,,%s\n", pApp->m_UserAccInfo.m_strCurUserName);
	fprintf(file, "Recipe Name,,%s\n", pApp->m_strLotConfigFile);
	fprintf(file, "System ID,,%s\n", pLotSummary->strMachineID);
	fprintf(file, "Start Time,,[%s]\n", pLotSummary->strStartLotTime);
	fprintf(file, "End Time,,[%s]\n\n\n", pLotSummary->strEndLotTime);
	//fprintf(file, "Comments,,%s\n", pLotSummary->strComments);
	for (int k = 0; k < m_nNoOfDies; k++)
	{
		strTemp.Format(",,,Total Inspected,Total Pass,Total Yield,Total Fail,False Rejection,Die Location,Datum Location,Tilt Inspection,Cover Layer Inspection,Cover Layer Alignment,Encap Insufficient,Encap Excess,Encap Crack,Encap BlowHole,Encap Contamination,Slot,Die Edge");
		for (int i = 0; i < m_nNoOfFov; i++)
		{
			int nCount = pApp->m_strPviDefectsStats[i].GetCount();
			for (int nTempIndex = 0; nTempIndex < nCount; nTempIndex++) {
				strTemp.Format("%s,%s", strTemp, pApp->m_strPviDefectsStats[i].GetAt(nTempIndex));
			}
		}
	}
	fprintf(file, strTemp);
	for (int k = 0; k < m_nNoOfDies; k++)
	{
		fprintf(file, "\n,,THA %d", k + 1);
		int nDefectIndexTemp = 0;
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalUnitsInspected);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalUnitsPass);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].dTotalYield);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalUnitsFail);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lUserModifiedFalseRejection);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lDieLocation);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lDatumLocation);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lTiltInsp);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lCoverLayerInspection);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lCoverLayerAlignment);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapInsufficient);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapExcess);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapCrack);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapBlowHole);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapContamination);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++, pApp->m_LotSummary.m_TotalDisplaySummary[k].lSlot);
		nTotalDefectCount[k].SetAt(nDefectIndexTemp++,pApp->m_LotSummary.m_TotalDisplaySummary[k].lDieEdge);

		/*for (int j = 0; j < pApp->m_nTotalViews; j++) {
			for (int i = 0; i < m_nNoOfFov; i++)
			{
				for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
					for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
						nTotalDefectCount[k].SetAt(nDefectIndexTemp++, pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalPviDefectCounter[i][j][x][y]);
					}
				}
			}
		}*/

		if (pApp->m_LotSummary.m_mapIndividualPviDefectCounter.size() > 0) {
			for (std::map<int, long>::iterator it = pApp->m_LotSummary.m_mapIndividualPviDefectCounter.begin();
				it != pApp->m_LotSummary.m_mapIndividualPviDefectCounter.end(); it++) {
				nTotalDefectCount[k].SetAt(nDefectIndexTemp++, it->second);
			}
		}

		for (int nTempIndex = 0; nTempIndex < nDefectIndexTemp; nTempIndex++) {
			fprintf(file, ",%9d", nTotalDefectCount[k].GetAt(nTempIndex));
		}
	}
	fprintf(file, "\n,,Total");
	int nDefectIndexTemp = 0;
	for (int k = 0; k < m_nNoOfDies; k++)
	{
		nDefectIndexTemp = 0;
		int nTemp = 0;
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalUnitsInspected);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalUnitsPass);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].dTotalYield);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalUnitsFail);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lUserModifiedFalseRejection);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lDieLocation);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lDatumLocation);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lTiltInsp);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lCoverLayerInspection);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lCoverLayerAlignment);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapInsufficient);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapExcess);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapCrack);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapBlowHole);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lEncapContamination);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lSlot);
		nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
		nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lDieEdge);

		/*for (int j = 0; j < pApp->m_nTotalViews; j++) {
			for (int i = 0; i < m_nNoOfFov; i++)
			{
				for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
					for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
						nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
						nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + pApp->m_LotSummary.m_TotalDisplaySummary[k].lTotalPviDefectCounter[i][j][x][y]);
					}
				}
			}
		}*/
		if (pApp->m_LotSummary.m_mapIndividualPviDefectCounter.size() > 0) {
			for (std::map<int, long>::iterator it = pApp->m_LotSummary.m_mapIndividualPviDefectCounter.begin();
				it != pApp->m_LotSummary.m_mapIndividualPviDefectCounter.end(); it++) {
				nTemp = nTotalDefectCountTotal.GetAt(nDefectIndexTemp);
				nTotalDefectCountTotal.SetAt(nDefectIndexTemp++, nTemp + it->second);
			}
		}
	}
	for (int nTempIndex = 0; nTempIndex < nDefectIndexTemp; nTempIndex++) {
		fprintf(file, ",%9d", nTotalDefectCountTotal[nTempIndex]);
	}
	fclose(file);

	for (int i = 0; i < 5; i++) {
		nTotalDefectCount[i].RemoveAll();
	}
	nTotalDefectCountTotal.RemoveAll();
}



void CStatisticsView::LoadTotalLotInfo()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	LOTSUMMARY* pLotSummary = &((CApplication*)AfxGetApp())->m_LotSummary;
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	CString strLotFile;
	CString strTemp;
	if (pApp->m_bLotOpened) {
		if (!pApp->m_strLotInfo.IsEmpty()) {
			strTemp.Format("%s\\%s\\%s", pApp->m_LotSummaryRootDir, pApp->m_strLotConfigFile, pApp->m_strLotInfo);
			strLotFile = pApp->m_strLotInfo;
		}
		else
		{
			strTemp.Format("%s\\DUMMY LOT", pApp->m_LotSummaryRootDir);
			strLotFile = "DUMMY LOT";
		}
		strLotFile = strTemp + "\\" + strLotFile + "_Total.csv";
		OutputDebugLog("LoadLotInfo %s", strLotFile);
	}
	else
		return;
	CAtlString resToken;
	CStringArray respone;
	int curPos = 0;
	int nLineCount = 0;
	CString strLine;
	char c[5000];
	int nDevice = -1;
	std::ifstream file(strLotFile);
	if (!file.good())
		return;

	while (!file.eof()) {
		file.getline(c, 5000);
		strLine = c;
		nLineCount = 0;
		curPos = 0;
		respone.RemoveAll();
		do
		{
			resToken = strLine.Tokenize(_T(","), curPos);
			respone.Add((CString)resToken);
			nLineCount++;
		} while (resToken != "");
		{
			if (!respone[0].Find("THA", 0))
			{
				int nLength = respone[0].GetLength();
				char cDevice = respone[0].GetAt(nLength - 1);
				CString strDevice;
				strDevice.Format("%c", cDevice);
				nDevice = atoi(strDevice);
				nDevice = nDevice - 1;
			}
			int nData = 1;
			if (nDevice != -1) {
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalUnitsInspected = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalUnitsPass = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].dTotalYield = atof(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalUnitsFail = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lUserModifiedFalseRejection = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lDieLocation = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lDatumLocation = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTiltInsp = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lCoverLayerInspection = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lCoverLayerAlignment = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapInsufficient = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapExcess = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapCrack = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapBlowHole = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lEncapContamination = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lSlot = atoi(respone[nData++]);
				pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lDieEdge = atoi(respone[nData++]);
				
				/*for (int j = 0; j < pApp->m_nTotalViews; j++) {
					for (int i = 0; i < m_nNoOfFov; i++)
					{
						for (int x = 0; x < pApp->m_nPviNumberOfArea[i][j]; x++) {
							for (int y = 0; y < pApp->m_nPviNoOfDefects[i][j][x]; y++) {
								pApp->m_LotSummary.m_TotalDisplaySummary[nDevice].lTotalPviDefectCounter[i][j][x][y] = atoi(respone[nData++]);
							}
						}
					}
				}*/

				if (pApp->m_LotSummary.m_mapIndividualPviDefectCounter.size() > 0) {
					for (std::map<int, long>::iterator it = pApp->m_LotSummary.m_mapIndividualPviDefectCounter.begin();
						it != pApp->m_LotSummary.m_mapIndividualPviDefectCounter.end(); it++) {
						it->second = atoi(respone[nData++]);
					}
				}

				nDevice = -1;
			}
		}
	}
	respone.RemoveAll();
}

void CStatisticsView::UpdateUPH(double dStripTime, int nDieNo)
{
	if (nDieNo < 1) return;

	EnterCriticalSection(&m_CriticalSectionUpdate);

	dAverageDiesTimeMs = (dAverageDiesTimeMs * nPPMDiesNo + dStripTime) / (nPPMDiesNo + nDieNo);
	nPPMDiesNo += nDieNo;

	//dNetAverageStripTime = (dNetAverageStripTime * nPPMNetStripNo + dNetStripTime) / (nPPMNetStripNo + 1);
	//nPPMNetStripNo++;

//	UpdateTotalContents();

	LeaveCriticalSection(&m_CriticalSectionUpdate);
}

void CStatisticsView::OnBnClickedOnlineResetstatscounter()
{
	// TODO: Add your control notification handler code here
	if (AfxMessageBox("Confirm to Reset Statistical Counters", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	//CSystemLoggingInfo* pSystemLoggingInfo;
	//   pSystemLoggingInfo = new CSystemLoggingInfo();
	//SYSTEMTIME Stime;
	//GetLocalTime(&Stime);
	//pSystemLoggingInfo->SaveStatisticsCounterReset(&Stime);
	dAverageDiesTimeMs = 0;
	nPPMDiesNo = 0;
	theApp.m_LotSummary.InitDisplaySummary();
	theApp.UpdateStatisticsWindow(TRUE);
	//m_pStatsView->RecordLotStats();
	//((CMainFrame*) m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.RecordLotStats();
//	AddLogEvent("Statistical Counters are Reset");
}



void CStatisticsView::OnBnClickedButtonStopMachine()
{
	// TODO: Add your control notification handler code here

	//If we're using Centralized Reject PC
	if (!theApp.m_bIsMasterApp && theApp.m_bCentralizedVerificationMode && theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
		if (theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.m_bConnected) {
			int nError = theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendStopMachine();
			if (nError) {
				AfxMessageBox("Failed when send stop message to the machine", MB_ICONERROR);
			}
		}
		else {
			AfxMessageBox("Disconnected with centralized reject PC", MB_ICONERROR);
		}
	}
	else {
		int nCmd = STOPMACHINE;
		CString strMsg, strCommEventMsg;
		strMsg.Format("%d", nCmd);
		int nError = theApp.m_CommInterface.m_pCommManager[0]->SendDataMessage(nCmd, strMsg);
		strCommEventMsg.Format("Vision : %s", strMsg);
		theApp.AddLogEvent(strCommEventMsg, COMM_LOG);
	}
}

