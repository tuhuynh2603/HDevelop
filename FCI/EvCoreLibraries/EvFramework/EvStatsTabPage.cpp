/// EvStatsTabPage.cpp : implementation of the CEvStatsTabPage class
/// Udupa; Nov'2012

#include "stdafx.h"
#include "EvStatsTabPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CChartInfo::Initialize(char* pstrTitleText, 
	char* strCharType1, COLORREF colorBar1, BOOL bIsTrackWise, char* pstrListText, 
	char* strCharType2, COLORREF colorBar2, char* strCharType3, COLORREF colorBar3)
{
	strTitleText = pstrTitleText;
	strListText = pstrListText ? pstrListText : strTitleText;

	strcpy_s(strChartTypes[0], strCharType1);
	colorBar[0] = colorBar1;
	bTrackWise = bIsTrackWise;
		
	nEntryCount = 1;
	if(strCharType2) {
		strcpy_s(strChartTypes[1], strCharType2);
		nEntryCount++;
	}
	else
		strChartTypes[1][0] = 0;
	if(strCharType3) {
		strcpy_s(strChartTypes[2], strCharType3);
		nEntryCount++;
	}
	else
		strChartTypes[2][0] = 0;
	colorBar[1] = colorBar2;
	colorBar[2] = colorBar3;

	strTypeAlias[0] = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CEvStatsTabPage dialog

CEvStatsTabPage::CEvStatsTabPage(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
#ifndef _AFX_NO_OCC_SUPPORT
	AfxEnableControlContainer ();
#endif // !_AFX_NO_OCC_SUPPORT
	//{{AFX_DATA_INIT(CEvStatsTabPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nCamera = 0;
	m_nImageIndex = 0;
	m_StartDate = CTime::GetCurrentTime();
	m_EndDate = CTime::GetCurrentTime();
	m_nChartType = 0;

	m_brush.CreateSolidBrush(RGB(50,50,50));
	m_font.CreateFont(16, 0, 0, 0, 400, FALSE, FALSE, FALSE, 
						ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
						PROOF_QUALITY, FIXED_PITCH|FF_MODERN, "Courier New");
}

CEvStatsTabPage::~CEvStatsTabPage()
{
	m_brush.DeleteObject();
	m_font.DeleteObject();
}

BOOL CEvStatsTabPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editData.SetFont(&m_font);
		

	int nIndex = 0;
	m_ChartInfo[nIndex++].Initialize("Total Fail",   "Units Failed",    RGB(150,50,50));
	m_ChartInfo[nIndex++].Initialize("Fail Types",   "Fail Types",      RGB(0,0,0), FALSE);
	m_ChartInfo[nIndex].Initialize("Spot",         "Spot",            RGB(100,100,150));
	strcpy_s(m_ChartInfo[nIndex++].strTypeAlias, "Fastener");
	m_ChartInfo[nIndex++].Initialize("Non-Circular", "Non-Circular",    RGB(150,150,100));
	m_ChartInfo[nIndex++].Initialize("Tear",         "Tear Lens",       RGB(150,100,150));
	m_ChartInfo[nIndex++].Initialize("Gap",          "Gap Lens",        RGB(100,150,150));
	m_ChartInfo[nIndex++].Initialize("No Lens",      "No Lens",         RGB(120,120,120));
	m_ChartInfo[nIndex++].Initialize("No Saline",    "No Saline",       RGB(220,220,220));
	m_ChartInfo[nIndex++].Initialize("Inner Tear",   "Inner Tear",      RGB(150,120,120));
	m_ChartInfo[nIndex++].Initialize("Total Pass",   "Units Passed",    RGB(50,150,50));
	m_ChartInfo[nIndex++].Initialize("Saline Level  [Shortage : In-Range : Excess] ", 
		"Saline Shortage", RGB(150,50,50), TRUE, "Saline Level",
		"Saline In-Range", RGB(50,200,50), 
		"Saline Excess", RGB(250,50,50));

	m_chartControl.SetBKColor(RGB(50,50,50));
	//m_chartControl.SetTextColor(RGB(255,255,255), FALSE);
	//m_chartControl.SetAxisColor(RGB(255,255,255), FALSE);

	for(int i=0; i<nIndex; i++)
		m_listChartType.AddString(m_ChartInfo[i].strListText);
	m_listChartType.SetCurSel(0);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEvStatsTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEvStatsTabPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STATS_IMAGEINDEX_STATIC, m_nImageIndex);
	DDX_Radio(pDX, IDC_STATSIMAGE_CAM1_RADIO, m_nCamera);
	DDX_DateTimeCtrl(pDX, IDC_STATS_START_DATETIMEPICKER, m_StartDate);
	DDX_DateTimeCtrl(pDX, IDC_STATS_END_DATETIMEPICKER, m_EndDate);
	DDX_LBIndex(pDX, IDC_STATS_CHARTTYPE_LIST, m_nChartType);
	DDX_Control(pDX, IDC_STATS_DATATREE, m_treeData);
	DDX_Control(pDX, IDC_STATS_DATA_EDIT, m_editData);
	DDX_Control(pDX, IDC_STATS_BARCHART_STATIC, m_chartControl);
	DDX_Control(pDX, IDC_STATS_CHARTTYPE_LIST, m_listChartType);
}


BEGIN_MESSAGE_MAP(CEvStatsTabPage, CDialog)
	//{{AFX_MSG_MAP(CEvStatsTabPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_STATS_DATATREE, OnCustomDraw)
	ON_NOTIFY(TVN_SELCHANGED, IDC_STATS_DATATREE, OnTvnSelchangedStatsDatatree)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STATS_START_DATETIMEPICKER, OnDtnDatetimechangeStatsStartDatetimepicker)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STATS_END_DATETIMEPICKER, OnDtnDatetimechangeStatsEndDatetimepicker)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvStatsTabPage message handlers

BOOL CEvStatsTabPage::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
#ifndef _AFX_NO_OCC_SUPPORT
	if (nCode == CN_EVENT)
		return GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
#endif // !_AFX_NO_OCC_SUPPORT

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CEvStatsTabPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return (BOOL) GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
//	return CDialog::OnCommand(wParam, lParam);
}

BOOL CEvStatsTabPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//return (BOOL) GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CEvStatsTabPage::PreTranslateMessage(MSG* pMsg) 
{

	// TODO: Add your specialized code here and/or call the base class
	return GetParent()->PreTranslateMessage(pMsg);
//	return CDialog::PreTranslateMessage(pMsg);
}

void CEvStatsTabPage::SetStatsType(int nType) 
{
	int nStartId, nEndId;
	if(nType==0) {
		nStartId = IDC_STATS_IMAGESSTATIC;
		nEndId = IDC_STATSCHILD_CONTROL;
	}
	else if(nType==1) {
		nStartId = IDC_STATS_CHARTSTATIC;
		nEndId = IDC_STATS_BARCHART_STATIC;
	}
	else {
		nStartId = IDC_STATS_TABLESTATIC;
		nEndId = IDC_STATS_DATA_EDIT;
	}

	for(int i=IDC_STATS_IMAGESSTATIC; i<nStartId; i++)
		GetDlgItem(i)->ShowWindow(SW_HIDE);
	for(int i=nStartId; i<=nEndId; i++)
		GetDlgItem(i)->ShowWindow(SW_SHOW);
	for(int i=nEndId+1; i<=IDC_STATS_DATA_EDIT; i++)
		GetDlgItem(i)->ShowWindow(SW_HIDE);
}

void CEvStatsTabPage::AdjustLayout()
{
	CRect rectDest;
	GetDlgItem(IDC_STATS_IMAGESSTATIC)->GetWindowRect(&rectDest);

	CRect rectSource, rectControl;
	int nDiff;

	GetDlgItem(IDC_STATS_CHARTSTATIC)->GetWindowRect(&rectSource);
	nDiff = rectSource.top - rectDest.top;
	for(int i=IDC_STATS_CHARTSTATIC; i<IDC_STATS_BARCHART_STATIC; i++) {
		GetDlgItem(i)->GetWindowRect(&rectControl);
		ScreenToClient(&rectControl);
		GetDlgItem(i)->SetWindowPos(NULL, rectControl.left, rectControl.top - nDiff, 0, 0, SWP_NOSIZE);
	}

	GetDlgItem(IDC_STATS_TABLESTATIC)->GetWindowRect(&rectSource);
	nDiff = rectSource.top - rectDest.top;
	for(int i=IDC_STATS_TABLESTATIC; i<IDC_STATS_DATA_EDIT; i++) {
		GetDlgItem(i)->GetWindowRect(&rectControl);
		ScreenToClient(&rectControl);
		GetDlgItem(i)->SetWindowPos(NULL, rectControl.left, rectControl.top - nDiff, 0, 0, SWP_NOSIZE);
	}

	CWnd* pWnd = GetDlgItem(IDC_STATS_DATATREE);
	CRect rectDlg, rect;
	GetWindowRect(&rectDlg);
	pWnd->GetWindowRect(&rect);
	GetDlgItem(IDC_STATS_DATATREE)->SetWindowPos(NULL, 0, 0, rect.Width(), rectDlg.Height()-130, SWP_NOMOVE);
	GetDlgItem(IDC_STATS_TABLESTATIC)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATS_TABLESTATIC)->SetWindowPos(NULL, 0, 0, rect.Width(), rectDlg.Height()-90, SWP_NOMOVE);
}

HBRUSH CEvStatsTabPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	int nId = pWnd->GetDlgCtrlID();
	if(nId==IDC_STATS_IMAGESSTATIC || 
		nId==IDC_STATS_CHARTSTATIC ||
		nId==IDC_STATS_TABLESTATIC ||
		nId==IDC_STATS_DATA_EDIT ||
		nId==IDC_RADIO1 ||
		nId==IDC_RADIO2 ||
		nId==IDC_RADIO3 ||
		nId==IDC_RADIO4 ||
		nId==IDC_RADIO5 ||
		nId==IDC_COMBO1 ||
		nId==IDC_STATS_ENDLABEL_STATIC2	 ||
		nId==IDC_STATS_CHARTTYPE_LIST)
    {
        pDC->SetTextColor(RGB(255,255,255));
        pDC->SetBkColor(RGB(50,50,50));
    }
	else if(nId==IDC_STATS_STARTLABEL_STATIC ||
		nId==IDC_STATS_ENDLABEL_STATIC ||
		nId==IDC_STATS_TYPELABEL_STATIC ||
		nId==IDC_STATS_IMAGEINDEX_STATIC)
    {
        pDC->SetTextColor(RGB(255,255,255));
		pDC->SetBkMode(TRANSPARENT);
    }

	return m_brush;
}

void CEvStatsTabPage::PopulateDataTree()
{
	m_treeData.DeleteAllItems();
	m_itemRoot = m_treeData.InsertItem("Statistics (YY-MM-DD)");

	CString strLotDir, strDailyDir,strHourlyDir,strHourlyDirPath, strHourleyFile;
	strLotDir.Format("%s\\*.*", *m_pstrLotFolder);
	
	HTREEITEM itemDaily,ItemHourly;
	CFileFind FindDaily, FindHourly, FindHourlyFile;
	BOOL bDailyFound, bHourlyFound, bHourlyFileFound;
	bDailyFound = FindDaily.FindFile(strLotDir);
	while(bDailyFound) {
		bDailyFound = FindDaily.FindNextFile();
		if(FindDaily.IsDots())
			continue;

		if(FindDaily.IsDirectory()) {
			strLotDir = FindDaily.GetFileTitle();
			itemDaily = m_treeData.InsertItem(strLotDir, m_itemRoot);

			strDailyDir.Format("%s\\%s\\*.*", 
				*m_pstrLotFolder, strLotDir);
			bHourlyFound = FindHourly.FindFile(strDailyDir);
			//BOOL bEmpty = TRUE;
			while(bHourlyFound) {
				bHourlyFound = FindHourly.FindNextFile();
				if(FindHourly.IsDots())
					continue;

                if(FindHourly.IsDirectory())
				{
					strHourlyDir = FindHourly.GetFileName();
					ItemHourly = m_treeData.InsertItem(strHourlyDir,itemDaily);

					strHourlyDirPath.Format("%s\\%s\\%s\\*.*",
						                    *m_pstrLotFolder, 
											strLotDir,
											 strHourlyDir);

						bHourlyFileFound = FindHourlyFile.FindFile(strHourlyDirPath);
					while(bHourlyFileFound)
					{
                          bHourlyFileFound = FindHourlyFile.FindNextFile();
						  if(FindHourlyFile.IsDots())
							  continue;
                          strHourleyFile = FindHourlyFile.GetFileTitle();
                          m_treeData.InsertItem(strHourleyFile,ItemHourly);
					}
				}
			}
		}
	}
	m_treeData.Expand(m_itemRoot, TVE_EXPAND);
	m_treeData.SetTextColor(150);

	m_editData.SetWindowText("");
}

void CEvStatsTabPage::DataTreeChanged(LPNMTREEVIEW pNMTreeView)
{
	TVITEM itemSel = pNMTreeView->itemNew;
	m_editData.SetWindowText("");

	CString strLogDirPath,strLoginDir;
	CString str;
	CString strDir;
	CString strFile;

	if(m_treeData.ItemHasChildren(itemSel.hItem)) {
		strDir = m_treeData.GetItemText(itemSel.hItem);
		strFile.Format("%s\\%s\\%s.day", *m_pstrLotFolder,
			strDir, strDir);
	}
	else {
		strFile.Format("%s\\%s\\%s\\%s.hr", *m_pstrLotFolder,
			m_treeData.GetItemText(m_treeData.GetParentItem(m_treeData.GetParentItem(itemSel.hItem))),
			m_treeData.GetItemText(m_treeData.GetParentItem(itemSel.hItem)), 
			m_treeData.GetItemText(itemSel.hItem));
	}

	CFile file;
	CFileException ex;
	if (!file.Open(strFile.GetBuffer(0), CFile::modeRead, &ex)) {
		m_editData.SetWindowText("");
		return;
	}

	int nLength = (int) file.GetLength();
	char* strText = new char[nLength+1];
	file.Read(strText, nLength);
	file.Close();
	strText[nLength] = '\0';

	m_editData.SetWindowText(strText);

	delete[] strText;
}

void CEvStatsTabPage::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVCUSTOMDRAW* pLVCD = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);

	*pResult = 0;		// Initialize with Default processing
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each item
	else if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
		HTREEITEM hTreeItem = (HTREEITEM) pLVCD->nmcd.dwItemSpec;
		pLVCD->clrText = RGB(255,255,255);
		if(m_treeData.GetSelectedItem()==hTreeItem)
			pLVCD->clrTextBk = RGB(150,50,50);
		else
			pLVCD->clrTextBk = RGB(50,50,50);
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}

void CEvStatsTabPage::OnTvnSelchangedStatsDatatree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	DataTreeChanged(pNMTreeView);

	*pResult = 0;
}

void CEvStatsTabPage::Plot()
{
	UpdateData(TRUE);
	if(m_EndDate < m_StartDate) {
		m_EndDate = m_StartDate;
		UpdateData(FALSE);
	}

	m_StartDate = CTime(m_StartDate.GetYear(), m_StartDate.GetMonth(), m_StartDate.GetDay(), 0, 0, 0);
	m_EndDate = CTime(m_EndDate.GetYear(), m_EndDate.GetMonth(), m_EndDate.GetDay(), 23, 59, 59);

	CString strDir, strFile;
	strDir.Format("%s\\*.*", *m_pstrLotFolder);
	
	CFileFind FindDaily;
	BOOL bDailyFound = FindDaily.FindFile(strDir);
	CFile file;
	CFileException ex;
	char* strText = NULL;
	int nMaxLength = 0, nLength;
	int nYear, nMonth, nDay;
	CTime selDate;
	char* strSubstr, *strSub2;
	int nCount, nCounts[5], nTotals[7][3];
	memset(nTotals, 0, sizeof(int)*7*3);
	if(m_nChartType < 0)
		return;
	CChartInfo* pChartInfo = &m_ChartInfo[m_nChartType];
	m_chartControl.SetBarWidth(pChartInfo->nEntryCount > 1 ? BAR_DEFAULT_WIDTH*3/4 : BAR_DEFAULT_WIDTH);

	while(bDailyFound) {
		bDailyFound = FindDaily.FindNextFile();
		if(FindDaily.IsDots())
			continue;

		if(FindDaily.IsDirectory()) {
			strDir = FindDaily.GetFileTitle();
			sscanf_s(strDir, "%d-%d-%d", &nYear, &nMonth, &nDay);
			if(nMonth <= 0)
				continue;
			if(nYear < 100)
				nYear += 2000;
			selDate = CTime(nYear, nMonth, nDay, 0, 0, 0);
			if((selDate < m_StartDate) || (selDate > m_EndDate))
				continue;

			strFile.Format("%s\\%s\\%s.day", *m_pstrLotFolder, strDir, strDir);
			if(!file.Open(strFile.GetBuffer(0), CFile::modeRead, &ex))
				continue;

			nLength = (int) file.GetLength();
			if(nLength < 10)
				continue;

			if(nLength > nMaxLength) {
				if(strText != NULL)
					delete[] strText;
				nMaxLength = nLength;
				strText = new char[nLength+1];
			}
			file.Read(strText, nLength);
			file.Close();
			strText[nLength] = '\0';
			strSubstr = strText;

			while(TRUE) {
				if(pChartInfo->bTrackWise) {
					for(int nType=0; nType<3; nType++) {
						if(pChartInfo->strChartTypes[nType][0]) {
							strSub2 = strstr(strSubstr, pChartInfo->strChartTypes[nType]);
							if(strSub2 == NULL) {
								if(pChartInfo->strTypeAlias[0]) {
									strSubstr = strstr(strSubstr, pChartInfo->strTypeAlias);
									if(strSubstr == NULL)
										break;
									strSubstr += strlen(pChartInfo->strTypeAlias);
								}
								else {
									strSubstr = NULL;
									break;
								}
							}
							else {
								strSubstr = strSub2 + strlen(pChartInfo->strChartTypes[nType]);
							}

							memset(nCounts, 0, sizeof(int)*5);
							sscanf_s(strSubstr, "%d %d %d %d %d %d", &nCount, &nCounts[0], &nCounts[1], &nCounts[2], &nCounts[3], &nCounts[4]);
							for(int nTrack=0; nTrack<5; nTrack++)
								nTotals[nTrack][nType] += nCounts[nTrack];
						}
					}
					if(strSubstr == NULL)
						break;
				}
				else {
					nCount = -1;
					for(int nType=2; nType<9; nType++) {
						strSub2 = strstr(strSubstr, m_ChartInfo[nType].strChartTypes[0]);
						if(strSub2 == NULL) {
							if(m_ChartInfo[nType].strTypeAlias[0]) {
								strSub2 = strstr(strSubstr, m_ChartInfo[nType].strTypeAlias);
								if(strSub2 == NULL)
									continue;
								strSubstr = strSub2 + strlen(m_ChartInfo[nType].strTypeAlias);
							}
							else
								continue;
						}
						else {
							strSubstr = strSub2 + strlen(m_ChartInfo[nType].strChartTypes[0]);
						}

						nCount = 0;
						sscanf_s(strSubstr, "%d", &nCount);
						nTotals[nType-2][0] += nCount;
					}
					if(nCount==-1)
						break;
				}
			}
		}
	}
	if(strText)
		delete[] strText;


	CString str;
	m_chartControl.RemoveAll();
	if(pChartInfo->bTrackWise) {
		for(int nTrack=0; nTrack<5; nTrack++) {
			str.Format("Track%d", nTrack+1);
			m_chartControl.AddBars(str, pChartInfo->nEntryCount, nTotals[nTrack], pChartInfo->colorBar);
		}
	}
	else {
		for(int i=0; i<7; i++)
			m_chartControl.AddBar(nTotals[i][0], m_ChartInfo[i+2].strListText, m_ChartInfo[i+2].colorBar[0]);
	}

	str.Format("%s Count    from %s to %s", pChartInfo->strTitleText, 
		m_StartDate.Format("%d-%m-%Y"), m_EndDate.Format("%d-%m-%Y"));
	m_chartControl.SetTitle(str);
	m_chartControl.Refresh();
}

void CEvStatsTabPage::OnDtnDatetimechangeStatsStartDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	Plot();

	*pResult = 0;
}

void CEvStatsTabPage::OnDtnDatetimechangeStatsEndDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	Plot();

	*pResult = 0;
}
