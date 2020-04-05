// AuditTrailDlg.cpp : implementation file

#include "stdafx.h"
#include "Application.h"
#include "EvMainFrame.h"
#include "StatisticsView.h"
#include "AuditTrailDlg.h"
#include "afxdialogex.h"
#include "atltime.h"
#include "Encryption.h"
#include <iostream>
#include <fstream>
#include "hpdf.h"
#include <direct.h>

IMPLEMENT_DYNAMIC(CAuditTrailDlg, CDialogEx)

CAuditTrailDlg::CAuditTrailDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAuditTrailDlg::IDD, pParent)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	m_StartDate = CTime::GetCurrentTime();
	m_EndDate = CTime::GetCurrentTime();
	m_strFolder = pApp->m_strAuditLoggingDir;

	m_brush.CreateSolidBrush(RGB(50,50,50));
}

CAuditTrailDlg::~CAuditTrailDlg()
{
	m_brush.DeleteObject();
}

void CAuditTrailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_StartDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndDate);
	DDX_Check(pDX, IDC_PDF, m_bPdf);
	DDX_Check(pDX, IDC_CSV, m_bCsv);
	DDX_Control(pDX, IDC_LIST2, m_ctrlAuditTrailList);
	DDX_Text(pDX, IDC_FOLDERPATH, m_strFolder);
}


BEGIN_MESSAGE_MAP(CAuditTrailDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, &CAuditTrailDlg::OnDtnDatetimechangeDatetimepicker2)
	ON_BN_CLICKED(IDC_BEXPTTOPDF, &CAuditTrailDlg::OnBnClickedBexpttopdf)
	ON_BN_CLICKED(IDC_BROWSE, &CAuditTrailDlg::OnBnClickedBrowse)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CAuditTrailDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_MESSAGE(WM_AUDITTRAIL_UPDATE, OnUpdateAuditTrail)
END_MESSAGE_MAP()


BOOL CAuditTrailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ctrlAuditTrailList.SetExtendedStyle(m_ctrlAuditTrailList.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_TRANSPARENTBKGND);
	CApplication* pApp = (CApplication*) AfxGetApp();
	pApp->m_pAuditTrailDialog = this;
	m_bPdf = TRUE;

	COLORREF clrList = RGB(40,40,40);
	m_ctrlAuditTrailList.SetBkColor(clrList);
	m_ctrlAuditTrailList.SetTextBkColor(clrList);
	m_ctrlAuditTrailList.SetTextColor(RGB(200,200,200));

	CImageList imageList; 
	imageList.Create(1, 23, ILC_COLOR, 1, 1);
	m_ctrlAuditTrailList.SetImageList(&imageList, LVSIL_SMALL);

	CRect rectArea;
	m_ctrlAuditTrailList.GetClientRect(&rectArea);
	int nWidth = rectArea.Width();

	m_ctrlAuditTrailList.InsertColumn(0, "Date", LVCFMT_LEFT, -1, -1);
	m_ctrlAuditTrailList.InsertColumn(1, "Time", LVCFMT_LEFT, -1, -1);
	m_ctrlAuditTrailList.InsertColumn(2, "Access Level", LVCFMT_LEFT, -1, -1);
	m_ctrlAuditTrailList.InsertColumn(3, "User Name", LVCFMT_LEFT, -1, -1);
	m_ctrlAuditTrailList.InsertColumn(4, "Event Log", LVCFMT_LEFT, -1, -1);

	m_ctrlAuditTrailList.SetColumnWidth(0, 90);
	m_ctrlAuditTrailList.SetColumnWidth(1, 90);
	m_ctrlAuditTrailList.SetColumnWidth(2, 100);
	m_ctrlAuditTrailList.SetColumnWidth(3, 130);
	m_ctrlAuditTrailList.SetColumnWidth(4, 720);

	m_colorInfo[0] = RGB(255,255,255);
	m_colorInfo[1] = RGB(150,150,255);

	DisplayLogInList();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control

	// EXCEPTION: OCX Property Pages should return FALSE
}

///Udupa; June'2015
HBRUSH CAuditTrailDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(pWnd == this)
		return m_brush;
	else
		return NULL;
}

void CAuditTrailDlg::OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	CTime m_DateEnd = m_EndDate;
	UpdateData(TRUE);

	if(m_EndDate < m_StartDate)
		m_EndDate = m_StartDate;

	if(m_EndDate != m_DateEnd) {
		m_ctrlAuditTrailList.DeleteAllItems();
		DisplayLogInList();
	}

	UpdateData(FALSE);
	*pResult = 0;
}

void CAuditTrailDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	CTime m_DateStart = m_StartDate;
	UpdateData(TRUE);

	if(m_StartDate > m_EndDate)
		m_StartDate = m_EndDate;

	if(m_StartDate != m_DateStart) {
		m_ctrlAuditTrailList.DeleteAllItems();
		DisplayLogInList();
	}

	UpdateData(FALSE);
	*pResult = 0;
}

void CAuditTrailDlg::DisplayLogInList()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CStdioFile txtFile;
	CFileException e;
	int nRow=0, nColumn=0, curPos=0;
	CString strStartDate,restoken,strSaveLog,strOutput,strEncr,strMd5,strTime;
	CTimeSpan timeSpan(1, 0, 0, 0);
	LONGLONG ndays;

	CTime m_DateStart = m_StartDate;
	ndays = ((m_EndDate - m_StartDate).GetDays()) + 1;

	while(ndays--) {
		strStartDate.Format("%2d-%02d-%02d",m_DateStart.GetYear(),m_DateStart.GetMonth(),m_DateStart.GetDay());
		strSaveLog.Format("%s\\LogFilesDir\\LogFile_%s.txt", pApp->m_strParmFolder, strStartDate);
		_mkdir(pApp->m_strParmFolder + "\\" + "LogFilesDir");
		SetFileAttributes(strSaveLog, FILE_ATTRIBUTE_NORMAL);
		BOOL bSave = txtFile.Open(strSaveLog, CFile::modeRead | CFile::shareDenyWrite , &e);

		if(!bSave);
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
				txtFile.Close();
				strOutput = EncryptDecrypt(strEncr);
				restoken = strOutput.Tokenize(" ",curPos);
				while(restoken != "") {
					if(nColumn == 0)
						AddItem(&m_ctrlAuditTrailList, strStartDate, 0);

					if(nColumn < 3) {
						m_ctrlAuditTrailList.SetItemText(nRow, nColumn+1, restoken);
						if(nColumn == 2)
							restoken = strOutput.Tokenize("\n",curPos);
						else
							restoken = strOutput.Tokenize(" ",curPos);
						nColumn++;
					}
					else {
						m_ctrlAuditTrailList.SetItemText(nRow, nColumn+1, restoken);
						restoken = strOutput.Tokenize(" ",curPos);
						nRow++;
						nColumn = 0;
					}
				}
			}
		}
		m_DateStart = m_DateStart + timeSpan;
		curPos=0;
	}
	m_ctrlAuditTrailList.EnsureVisible(m_ctrlAuditTrailList.GetItemCount()-1, FALSE);
	UpdateData(FALSE);
}

void CAuditTrailDlg::OnBnClickedBexpttopdf()
{
	UpdateData(TRUE);

	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strPeriod, strTitle, strFile;

	if(!PathFileExists(m_strFolder))
		MessageBox("Please enter the valid Path to Save the file");
	else {
		if((m_EndDate - m_StartDate).GetDays() > 0) {
			strPeriod.Format("%04u-%02u-%02u to %04u-%02u-%02u",
			m_StartDate.GetYear(), m_StartDate.GetMonth(), m_StartDate.GetDay(),
			m_EndDate.GetYear(), m_EndDate.GetMonth(), m_EndDate.GetDay());
			strTitle.Format("Period: %s", strPeriod);
			strFile.Format("%s\\MV_%s_Audit_Trail_%s", m_strFolder, pApp->m_strMachineID, strPeriod);
		}
		else {
			strPeriod.Format("%04u-%02u-%02u", 
			m_StartDate.GetYear(), m_StartDate.GetMonth(), m_StartDate.GetDay());
			strTitle.Format("Date: %s", strPeriod);
			strFile.Format("%s\\MV_%s_Audit_Trail_%s", m_strFolder, pApp->m_strMachineID, strPeriod);
		}

		if(m_bPdf == TRUE)
			WriteToPdf(strFile + ".pdf", strTitle);

		if(m_bCsv == TRUE)
			WriteToCsv(strFile + ".csv");

		if(m_bPdf == FALSE && m_bCsv == FALSE)
			MessageBox("Please Select Either PDF or CSV");

		pApp->m_strAuditLoggingDir = m_strFolder;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Folder: AuditTrail", pApp->m_strAuditLoggingDir);
	}
}

void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
				(HPDF_UINT)detail_no);
}

void CAuditTrailDlg::WriteToPdf(CString strFile, CString strTitle)
{
	CString str, strPage, strList, strVersion;

	HPDF_Doc pdf;
	pdf = HPDF_New(error_handler, NULL);
	if(!pdf) {
		str.Format("Failed to create Audit Trail PDF object");
		OutputDebugLogTo(0, TRUE, "%s", str);
	}

	HPDF_STATUS status;
	status = HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	status = HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_NONE);
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font fontBold = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page page;

	strVersion = GetCurrentVersion("FCI_4X Vision System Audit Trail - Emage Equipment- ");

	int nPage=0;
	float dWidth, dHeight, dLinePos = -1, dTextWidth;
	int nLines = m_ctrlAuditTrailList.GetItemCount();
	for(int nLineNo=0; nLineNo<nLines; nLineNo++) {
		if(dLinePos < 50) {
			if(dLinePos!=-1) {
				strPage.Format("Page %d", ++nPage);
				dTextWidth = HPDF_Page_TextWidth(page, strPage);
				HPDF_Page_TextOut(page, (dWidth - dTextWidth) / 2, 30, strPage);
				status = HPDF_Page_EndText(page);
			}
			page = HPDF_AddPage(pdf);
			status = HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
			dWidth = HPDF_Page_GetWidth(page);
			dHeight = HPDF_Page_GetHeight(page);
			status = HPDF_Page_SetFontAndSize(page, fontBold, 10);
			status = HPDF_Page_BeginText(page);

			dLinePos = dHeight - 40;
			dTextWidth = HPDF_Page_TextWidth(page, strVersion);
			HPDF_Page_TextOut(page, (dWidth - dTextWidth) / 2, dLinePos, strVersion);

			dLinePos -= 14;
			status = HPDF_Page_SetFontAndSize(page, fontBold, 9.5);
			dTextWidth = HPDF_Page_TextWidth(page, strTitle);
			HPDF_Page_TextOut(page, (dWidth - dTextWidth) / 2, dLinePos, strTitle);

			status = HPDF_Page_SetFontAndSize(page, fontBold, 8.5);
			dLinePos = dHeight - 80;
			HPDF_Page_TextOut(page, 30, dLinePos, "DATE");
			HPDF_Page_TextOut(page, 90, dLinePos, "TIME");
			HPDF_Page_TextOut(page, 140, dLinePos, "USER ACCESS ");
			HPDF_Page_TextOut(page, 155, dLinePos - 10, "LEVEL");
			HPDF_Page_TextOut(page, 210, dLinePos, "USER NAME");
			HPDF_Page_TextOut(page, 280, dLinePos, "LOG EVENT");

			dLinePos -= 28;
			status = HPDF_Page_SetFontAndSize(page, font, 8.5);
		}
		CString strTemp;
		strTemp.Format("%s",m_ctrlAuditTrailList.GetItemText(nLineNo,0));
		status = HPDF_Page_TextOut(page,30, dLinePos,strTemp);

		strTemp.Format("%s",m_ctrlAuditTrailList.GetItemText(nLineNo,1));
		status = HPDF_Page_TextOut(page,90, dLinePos,strTemp);

		strTemp.Format("%s",m_ctrlAuditTrailList.GetItemText(nLineNo,2));
		status = HPDF_Page_TextOut(page,140, dLinePos,strTemp);

		strTemp.Format("%s",m_ctrlAuditTrailList.GetItemText(nLineNo,3));
		status = HPDF_Page_TextOut(page,210, dLinePos,strTemp);

		strTemp.Format("%s",m_ctrlAuditTrailList.GetItemText(nLineNo,4));
		int nPos = 0;
		int nLength = (int) strlen(strTemp);
		if(nLength > 55) {
			int curpos=0;
			CString strFound, strLine, strToken;
			strFound = strTemp.GetAt(55);
			while(curpos < 55) {
				strToken = strTemp.Tokenize(strFound,curpos);
				strLine += strToken + strFound;
			}
			if(nLength > 65) {
				strToken = strTemp.Tokenize(" ",curpos);
				strLine += strToken;
				status = HPDF_Page_TextOut(page,280, dLinePos,strLine);
				strLine = "";
				nPos = 5;
			}	
			strToken = strTemp.Tokenize("\n",curpos);
			strLine += strToken;
			if(strLine != "") {
				if(nPos == 5)
					dLinePos -= 12;
				status = HPDF_Page_TextOut(page,280+(float)nPos, dLinePos,strLine);
			}
		}
		else
			status = HPDF_Page_TextOut(page,280, dLinePos,strTemp);
		
		status = HPDF_Page_ShowTextNextLine(page, strList);
		status = HPDF_Page_MoveTextPos(page, 0, -12);
		dLinePos -= 12;
	}
	strPage.Format("Page %d", ++nPage);
	dTextWidth = HPDF_Page_TextWidth(page, strPage);
	HPDF_Page_TextOut(page, (dWidth - dTextWidth) / 2, 30, strPage);

	status = HPDF_Page_EndText(page);

	CFileFind fileFind;
	if(fileFind.FindFile(strFile))
		SetFileAttributes(strFile, FILE_ATTRIBUTE_NORMAL);
	status = HPDF_SaveToFile(pdf, strFile); OutputDebugLog("%d", status);

	if(!status)
		SetFileAttributes(strFile, FILE_ATTRIBUTE_READONLY);
	HPDF_Free(pdf);

	if(!status) {
		str.Format("Audit Trail PDF file saved to:\n\n\n%s", strFile);
		MessageBox("PDF Exported Successfully");
		OutputDebugLogTo(0, TRUE, "%s", str);
	}
	else
		MessageBox("Error in Exporting");
}

void CAuditTrailDlg::WriteToCsv(CString strFile)
{
	CString strList, str;
	CStdioFile CTxtFile;
	CFileException e;

	BOOL bSave = CTxtFile.Open(strFile, CFile::modeReadWrite | CFile::shareDenyWrite | CFile::modeCreate | CFile::typeText, &e);
	if(!bSave)
		return;

	CTxtFile.SeekToBegin();

	strList = ("DATE,TIME,USER ACCESS LEVEL,USER NAME,LOG EVENT\n");
	CTxtFile.WriteString(strList);

	for(int nLineNo = 0; nLineNo < m_ctrlAuditTrailList.GetItemCount(); nLineNo++ ) {
		for(int j=0; j<5;j++)
			CTxtFile.WriteString(m_ctrlAuditTrailList.GetItemText(nLineNo, j) + ",");
		CTxtFile.WriteString("\n");
	}

	CTxtFile.Close();

	if(bSave) {
		str.Format("Audit Trail CSV file saved to:\n\n\n%s", strFile);
		MessageBox("CSV Exported Successfully");
		OutputDebugLogTo(0, TRUE, "%s", str);
	}
	else
		MessageBox("Error in Exporting");
}

void CAuditTrailDlg::AddItem(CListCtrl* pCtrl, CString strItem, int nIndex)
{
	LV_ITEM lvitem;

	lvitem.iSubItem = nIndex;
	lvitem.pszText = strItem.GetBuffer(0); 
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = pCtrl->GetItemCount();
	pCtrl->InsertItem(&lvitem);
}

void CAuditTrailDlg::OnBnClickedBrowse()
{
	UpdateData(TRUE);
	LPMALLOC pMalloc;
	CString d1,d2;

	if (::SHGetMalloc(&pMalloc) == NOERROR) {
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;
		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;

		bi.lpszTitle = _T("Select a directory...");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL) {
			if (::SHGetPathFromIDList(pidl, pszBuffer))
				m_strFolder = pszBuffer;
			pMalloc->Free(pidl);
		}
		pMalloc->Release();
	} 
	GetDlgItem(IDC_FOLDERPATH)->SetWindowText(m_strFolder);
}

///Udupa; June'2015
LRESULT CAuditTrailDlg::OnUpdateAuditTrail(WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME* pTime = (SYSTEMTIME*) wParam;
	CString* pstrMessage = (CString*) lParam;
	if(pTime == NULL || pstrMessage == NULL || pstrMessage->GetLength()==0)
		return -1;
	
	CTime time = *pTime;
	int nStartDiff = (int) (time - m_StartDate).GetDays();
	int nEndDiff = (int) (m_EndDate - time).GetDays();
	if(nStartDiff >= 0 && nEndDiff >= 0) {
		CString strDate, strTime;
		strDate.Format("%04u-%02u-%02u", pTime->wYear, pTime->wMonth, pTime->wDay);
		strTime.Format("%02u:%02u:%02u", pTime->wHour, pTime->wMinute, pTime->wSecond);

		AddItem(&m_ctrlAuditTrailList, strDate, 0);

		CApplication* pApp = (CApplication*) AfxGetApp();
		int nItem = m_ctrlAuditTrailList.GetItemCount() - 1;
		m_ctrlAuditTrailList.SetItemText(nItem, 1, strTime);
		m_ctrlAuditTrailList.SetItemText(nItem, 2, pApp->m_UserAccInfo.m_strCurrentAccessLevel);
		m_ctrlAuditTrailList.SetItemText(nItem, 3, pApp->m_UserAccInfo.m_strCurUserName);
		m_ctrlAuditTrailList.SetItemText(nItem, 4, *pstrMessage);
		m_ctrlAuditTrailList.EnsureVisible(m_ctrlAuditTrailList.GetItemCount()-1, FALSE);
	}

	return 0;
}