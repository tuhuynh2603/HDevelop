// DefectPrioritySelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DefectPrioritySelDlg.h"
#include "afxdialogex.h"
#include "Application.h"


// CCustomListBox

IMPLEMENT_DYNAMIC(CCustomListBox, CListBox)
CCustomListBox::CCustomListBox()
{
	m_nMovingIndex = -1;
	m_nMoveToIndex = -1;
	m_bCaptured = FALSE;
	m_dwInterval = 0;
	bCallNavigate = FALSE;
}

CCustomListBox::~CCustomListBox()
{
}

BEGIN_MESSAGE_MAP(CCustomListBox, CListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CCustomListBox message handlers

void CCustomListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);

	m_nMovingIndex = -1;
	m_nMoveToIndex = -1;
	m_bCaptured = FALSE;
	m_dwInterval = 0;

	BOOL Outside;
	int Index = ItemFromPoint(point,Outside);
	if(Index != -1 && !Outside) {
		m_nMovingIndex = Index;
		SetCurSel(Index);
	}
}

void CCustomListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_nMovingIndex != -1 && m_bCaptured) {
		KillTimer(LB_SCROLLDOWN);
		KillTimer(LB_SCROLLUP);
		m_dwInterval = 0;
		m_bCaptured = FALSE;
		ReleaseCapture();

		CRect Rect;
		GetClientRect(&Rect);
		if(Rect.PtInRect(point)) {
			InsertDraggedItem();
		}
		else {
			Invalidate();
			UpdateWindow();
		}
		m_nMovingIndex = -1;
		m_nMoveToIndex = -1;
	}

	CListBox::OnLButtonUp(nFlags, point);
}


void CCustomListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	CListBox::OnMouseMove(nFlags, point);

	if(nFlags & MK_LBUTTON) {
		if(m_nMovingIndex != -1 && !m_bCaptured) {
			SetCapture();
			m_bCaptured = TRUE;
		}
      
		BOOL Outside;
		int Index = ItemFromPoint(point, Outside);
		if(Outside) {
			CRect ClientRect;
			GetClientRect(&ClientRect);

			if(ClientRect.PtInRect(point)) {
				KillTimer(LB_SCROLLDOWN);
				KillTimer(LB_SCROLLUP);
				m_dwInterval = 0;
				Index = -1;
				Outside = FALSE;
			}
			else {
				DoTheScrolling(point, ClientRect);
			}
		}
		else {
			KillTimer(LB_SCROLLDOWN);
			KillTimer(LB_SCROLLUP);
			m_dwInterval = 0;
		}
      
		if(Index != m_nMoveToIndex && !Outside)
			DrawTheLines(Index);
	}
}

void CCustomListBox::DrawTheLines(int Index)
{
	CRect ClientRect;
	GetClientRect(&ClientRect);

	CDC *pDC = GetDC();

	CRect Rect;
	int Width = 2;
	if(m_nMoveToIndex != m_nMovingIndex) {
		CPen Pen(PS_SOLID, Width, RGB(255,255,255));
		CPen *pOldPen = pDC->SelectObject(&Pen);
		if(m_nMoveToIndex != -1) {
			GetItemRect(m_nMoveToIndex, &Rect);
			if(ClientRect.PtInRect(Rect.TopLeft())) {
				pDC->MoveTo(Rect.left, Rect.top+1);
				pDC->LineTo(Rect.right-(Width/2), Rect.top+1);
			}
		}
		else {
			GetItemRect(GetCount()-1, &Rect);
			if(ClientRect.PtInRect(CPoint(0, Rect.bottom))){
				pDC->MoveTo(Rect.left, Rect.bottom);
				pDC->LineTo(Rect.right-(Width/2), Rect.bottom);
			}
		}
		pDC->SelectObject(pOldPen);
	}

	m_nMoveToIndex = Index;
	if(m_nMoveToIndex != m_nMovingIndex){
		CPen Pen(PS_SOLID, Width, RGB(0,0,0));
		CPen *pOldPen = pDC->SelectObject(&Pen);
		if (m_nMoveToIndex != -1) {
			GetItemRect(Index, &Rect);
			if(ClientRect.PtInRect(Rect.TopLeft())) {
				pDC->MoveTo(Rect.left, Rect.top+1);
				pDC->LineTo(Rect.right-(Width/2), Rect.top+1);
			}
		}
		else {
			GetItemRect(GetCount()-1, &Rect);
			if(ClientRect.PtInRect(CPoint(0, Rect.bottom))) {
				pDC->MoveTo(Rect.left, Rect.bottom);
				pDC->LineTo(Rect.right-(Width/2), Rect.bottom);
			}
		}
		pDC->SelectObject(pOldPen);
	}
	ReleaseDC(pDC);
}

void CCustomListBox::InsertDraggedItem()
{
	CString Text;
	DWORD_PTR ItemData;
	GetText(m_nMovingIndex,Text);
	ItemData = GetItemData(m_nMovingIndex);
	if(m_nMovingIndex < m_nMoveToIndex) {
		SetRedraw(FALSE);
		int TopIndex = GetTopIndex();
		int NewIndex = InsertString(m_nMoveToIndex, Text);
		SetItemData(NewIndex, ItemData);
		DeleteString(m_nMovingIndex);
		SetCurSel(NewIndex-1);
		SetTopIndex(TopIndex);
		SetRedraw(TRUE);
		Invalidate();
		UpdateWindow();
	}
	else if(m_nMovingIndex > m_nMoveToIndex) {
		SetRedraw(FALSE);
		int TopIndex = GetTopIndex();
		DeleteString(m_nMovingIndex);
		int NewIndex = InsertString(m_nMoveToIndex, Text);
		SetItemData(NewIndex, ItemData);
		SetCurSel(NewIndex);
		SetTopIndex(TopIndex);
		SetRedraw(TRUE);
		Invalidate();
		UpdateWindow();
	}
	bCallNavigate = TRUE;
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd);
}

void CCustomListBox::DoTheScrolling(CPoint Point,CRect ClientRect)
{
	if(Point.y > ClientRect.Height()){
		DWORD Interval = 250 / (1+ ((Point.y-ClientRect.Height())/GetItemHeight(0)));
		if(Interval != m_dwInterval) {
			m_dwInterval = Interval;
			SetTimer(LB_SCROLLDOWN, Interval, NULL);
			OnTimer(LB_SCROLLDOWN);
		}
	}
	else if (Point.y < 0) {
		DWORD Interval = 250 / (1+(abs(Point.y)/GetItemHeight(1)));
		if(Interval != m_dwInterval){
			m_dwInterval = Interval;
			SetTimer(LB_SCROLLUP, Interval, NULL);
			OnTimer(LB_SCROLLUP);
		}
	}
	else {
		KillTimer(LB_SCROLLDOWN);
		KillTimer(LB_SCROLLUP);
		m_dwInterval = 0;
	}
}

//// **** ------------------------------------------------------------------------------ **** ////

// CDefectPrioritySelDlg dialog

IMPLEMENT_DYNAMIC(CDefectPrioritySelDlg, CDialogEx)

CDefectPrioritySelDlg::CDefectPrioritySelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDefectPrioritySelDlg::IDD, pParent)
{
	pApp = (CApplication*) AfxGetApp();
}

CDefectPrioritySelDlg::~CDefectPrioritySelDlg()
{
}

void CDefectPrioritySelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEFECT_PRIORITY_SEL_LIST, m_ctrlListBox);
}


BEGIN_MESSAGE_MAP(CDefectPrioritySelDlg, CDialogEx)
	ON_WM_VKEYTOITEM()
	ON_BN_CLICKED(IDC_DEFECT_PRIORITY_SEL_UP_MFCBUTTON, &CDefectPrioritySelDlg::OnBnClickedDefectPrioritySelUpMfcbutton)
	ON_BN_CLICKED(IDC_DEFECT_PRIORITY_SEL_DOWN_MFCBUTTON, &CDefectPrioritySelDlg::OnBnClickedDefectPrioritySelDownMfcbutton)
	ON_BN_CLICKED(IDOK, &CDefectPrioritySelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDefectPrioritySelDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDefectPrioritySelDlg message handlers

BOOL CDefectPrioritySelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitializePriorityList();
	for(int i=0; i<pApp->m_strDefectErrorCodesPriority.GetCount(); i++) {
		m_ctrlListBox.AddString(pApp->m_strDefectErrorCodesPriority.GetAt(i));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDefectPrioritySelDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	// TODO: Add your message handler code here and/or call default
	int nRow = pListBox->GetCurSel();

	if(nKey == VK_UP)
		NavigateListBoxContents(nRow, FALSE, TRUE);
	else if(nKey == VK_DOWN)
		NavigateListBoxContents(nRow, TRUE, TRUE);

	return CDialogEx::OnVKeyToItem(nKey, pListBox, nIndex);
}


void CDefectPrioritySelDlg::OnBnClickedDefectPrioritySelUpMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nRow = m_ctrlListBox.GetCurSel();
	NavigateListBoxContents(nRow, FALSE);

	UpdateData(FALSE);
}


void CDefectPrioritySelDlg::OnBnClickedDefectPrioritySelDownMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nRow = m_ctrlListBox.GetCurSel();
	NavigateListBoxContents(nRow, TRUE);

	UpdateData(FALSE);
}


void CDefectPrioritySelDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	DefectPrioritySelectionFile(TRUE);

	CDialogEx::OnOK();
}


void CDefectPrioritySelDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CDefectPrioritySelDlg::NavigateListBoxContents(int nRow, BOOL bDown, BOOL bKeyDown)
{
	CString strListBox;
	int nDefectErrorCode;
	int nRowChange = bDown ? nRow+1 : nRow-1;
	BOOL bNavigate = FALSE;
	
	if(!bDown && nRow > 0 && nRow <= pApp->m_strDefectErrorCodesPriority.GetCount()-1)
		bNavigate = TRUE;
	else if(bDown && nRow >= 0 && nRow < pApp->m_strDefectErrorCodesPriority.GetCount()-1)
		bNavigate = TRUE;

	if(bNavigate) {
		strListBox = pApp->m_strDefectErrorCodesPriority[nRow];
		pApp->m_strDefectErrorCodesPriority[nRow] = pApp->m_strDefectErrorCodesPriority[nRowChange];
		pApp->m_strDefectErrorCodesPriority[nRowChange] = strListBox;
		
		nDefectErrorCode = pApp->m_nDefectErrorCodesPriority[nRow];
		pApp->m_nDefectErrorCodesPriority[nRow] = pApp->m_nDefectErrorCodesPriority[nRowChange];
		pApp->m_nDefectErrorCodesPriority[nRowChange] = nDefectErrorCode;

		m_ctrlListBox.ResetContent();
		for(int i=0; i<pApp->m_strDefectErrorCodesPriority.GetCount(); i++) {
			m_ctrlListBox.AddString(pApp->m_strDefectErrorCodesPriority[i]);
		}
		m_ctrlListBox.SetCurSel(bKeyDown ? nRow : nRowChange);
	}
}


void CDefectPrioritySelDlg::LoadDefaultDefectSelection()
{
	//m_strListBoxContents.Add("Device Location");
	//m_strListBoxContents.Add("Datum Location");
	//m_strListBoxContents.Add("Tilt Inspection");
	//m_strListBoxContents.Add("Cover Layer Inspection");
	//m_strListBoxContents.Add("Encap Insufficent");
	//m_strListBoxContents.Add("Encap Excess");
	//m_strListBoxContents.Add("Die Inspection");
	//m_strListBoxContents.Add("Pvi Inspection");
}


void CDefectPrioritySelDlg::DefectPrioritySelectionFile(BOOL bSave)
{
	CIniFile iniFile;
	CFileException e;

	if (m_ctrlListBox.bCallNavigate) {
		for (int i = 0; i < m_ctrlListBox.GetCount(); i++) {
			CString strTemp;
			m_ctrlListBox.GetText(i, strTemp);
			for (int j = 0; j < m_ctrlListBox.GetCount(); j++) {
				if (pApp->m_strDefectErrorCodesPriority.GetAt(j).CompareNoCase(strTemp) == 0) {
					int nErr = pApp->m_nDefectErrorCodesPriority[i];
					pApp->m_nDefectErrorCodesPriority[i] = pApp->m_nDefectErrorCodesPriority[j];
					pApp->m_nDefectErrorCodesPriority[j] = nErr;
				}

			}
			pApp->m_strDefectErrorCodesPriority.SetAt(i, strTemp);
		}
		for (int i = 0; i < m_ctrlListBox.GetCount(); i++) {
			CString strTemp;
			m_ctrlListBox.GetText(i, strTemp);
			pApp->m_strDefectErrorCodesPriority.SetAt(i, strTemp);
		}
		m_ctrlListBox.bCallNavigate = FALSE;
	}



	CString strFile, strKey, strTemp, strGetVal="";
	int nGetVal = 0;
	int nCount = 0;
	//UpdateData(TRUE);

	strFile.Format("%s\\DefectPrioritySelection.dat", pApp->m_strConfigDir);
	iniFile.SetPath(strFile);
	
	strKey.Format("Defect Priority Selection");
	if(!bSave && iniFile.ReadFile()) {
		pApp->m_strDefectErrorCodesPriority.RemoveAll();
		strTemp.Format("Defect Count");
		nCount = iniFile.GetInteger(strKey, strTemp, nCount);
		for(int a=0; a<nCount; a++) {
			strTemp.Format("Defect Contents[%d]", a+1);
			strGetVal = iniFile.GetCString(strKey, strTemp, strGetVal);
			pApp->m_strDefectErrorCodesPriority.Add(strGetVal);

			strTemp.Format("Defect ErrorCodes[%d]", a + 1);
			nGetVal = iniFile.GetInteger(strKey, strTemp, nGetVal);
			pApp->m_nDefectErrorCodesPriority.Add(nGetVal);
			
		}
	}
	else {
		strTemp.Format("Defect Count");
		nCount = pApp->m_strDefectErrorCodesPriority.GetCount();
		iniFile.SetInteger(strKey, strTemp, nCount);
		for(int b=0; b<nCount; b++) {
			strTemp.Format("Defect Contents[%d]", b+1);
			strGetVal = pApp->m_strDefectErrorCodesPriority.GetAt(b);
			iniFile.SetCString(strKey, strTemp, strGetVal);

			strTemp.Format("Defect ErrorCodes[%d]", b + 1);
			nGetVal = pApp->m_nDefectErrorCodesPriority[b];
			iniFile.SetInteger(strKey, strTemp, nGetVal);
		}
		iniFile.WriteFile();

		//Update for the defect priority map
		for (int i = 0; i < pApp->m_nDefectErrorCodesPriority.GetCount(); i++) {
			int nErrorCode = pApp->m_nDefectErrorCodesPriority.GetAt(i);
			if (pApp->m_mapDefectErrorCodesPriority.find(nErrorCode) != pApp->m_mapDefectErrorCodesPriority.end()) {
				pApp->m_mapDefectErrorCodesPriority[nErrorCode] = i;
			}
			else {
				pApp->m_mapDefectErrorCodesPriority.insert(std::pair<int, int>(nErrorCode, i));
			}
		}
	}
//	UpdateData(FALSE);
}

void CDefectPrioritySelDlg::InitializePriorityList()
{
	BOOL bCopy = FALSE;
	// TODO:  Add extra initialization here
	DefectPrioritySelectionFile(FALSE);
	int nCountTempPriorityList = pApp->m_strDefectErrorCodesPriorityTemp.GetCount();
	int nCountPrioritylist = pApp->m_strDefectErrorCodesPriority.GetCount();
	if (nCountPrioritylist <= nCountTempPriorityList) {
		for (int i = 0; i < nCountTempPriorityList; i++) {
			bCopy = FALSE;
			CString str1 = pApp->m_strDefectErrorCodesPriorityTemp.GetAt(i);
			for (int j = 0; j < nCountPrioritylist; j++) {
				CString str2 = pApp->m_strDefectErrorCodesPriority.GetAt(j);
				if (pApp->m_strDefectErrorCodesPriority.GetAt(j).CompareNoCase(pApp->m_strDefectErrorCodesPriorityTemp.GetAt(i)) == 0)
				{
					pApp->m_nDefectErrorCodesPriority.GetAt(j) = pApp->m_nDefectErrorCodesPriorityTemp.GetAt(i);
					bCopy = TRUE;
					break;
				}
			}
			if (!bCopy) {
				pApp->m_strDefectErrorCodesPriority.Add(pApp->m_strDefectErrorCodesPriorityTemp.GetAt(i));
				pApp->m_nDefectErrorCodesPriority.Add(pApp->m_nDefectErrorCodesPriorityTemp.GetAt(i));
			}
		}
	}
	if (nCountPrioritylist >= nCountTempPriorityList)
	{
		CArray<int> m_nDefectErrorCodesPriorityFiltered;
		CStringArray m_strDefectErrorCodesPriorityFiltered;		
		
		for (int i = 0; i < nCountPrioritylist; i++) {
			bCopy = FALSE;
			CString str1 = pApp->m_strDefectErrorCodesPriority.GetAt(i);
			for (int j = 0; j < nCountTempPriorityList; j++) {
				CString str2 = pApp->m_strDefectErrorCodesPriorityTemp.GetAt(j);
				if (pApp->m_strDefectErrorCodesPriorityTemp.GetAt(j).CompareNoCase(pApp->m_strDefectErrorCodesPriority.GetAt(i)) == 0)
				{
					bCopy = TRUE;
					m_nDefectErrorCodesPriorityFiltered.Add(pApp->m_nDefectErrorCodesPriority.GetAt(i));
					m_strDefectErrorCodesPriorityFiltered.Add(pApp->m_strDefectErrorCodesPriority.GetAt(i));
					break;
				}
			}
		}

		pApp->m_nDefectErrorCodesPriority.RemoveAll();
		pApp->m_strDefectErrorCodesPriority.RemoveAll();
		pApp->m_nDefectErrorCodesPriority.Copy(m_nDefectErrorCodesPriorityFiltered);
		pApp->m_strDefectErrorCodesPriority.Copy(m_strDefectErrorCodesPriorityFiltered);
	}

	DefectPrioritySelectionFile(TRUE);
}