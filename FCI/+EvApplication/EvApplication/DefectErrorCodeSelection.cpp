// DefectErrorCodeSelection.cpp : implementation file
//

#include "stdafx.h"
#include "DefectErrorCodeSelection.h"
#include "afxdialogex.h"
#include "Application.h"
#include "ImageSelectionDlg.h"

// CDefectErrorCodeSelection dialog

IMPLEMENT_DYNAMIC(CDefectErrorCodeSelection, CDialogEx)

CDefectErrorCodeSelection::CDefectErrorCodeSelection(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DEFECT_ERRORCODE_SELECTION, pParent)
{
	m_bIsConfirm = FALSE;
	m_bIsApplyAllDevice = FALSE;
}

CDefectErrorCodeSelection::~CDefectErrorCodeSelection()
{
}

void CDefectErrorCodeSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ERRORCODE_SELECTION, m_listErrorCodeSelection);
}


BEGIN_MESSAGE_MAP(CDefectErrorCodeSelection, CDialogEx)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_ERRORCODE_SELECTION, OnCustomdrawList1)
	ON_BN_CLICKED(IDOK, &CDefectErrorCodeSelection::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CDefectErrorCodeSelection::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_ALL, &CDefectErrorCodeSelection::OnBnClickedButtonApplyAll)
END_MESSAGE_MAP()


// CDefectErrorCodeSelection message handlers


BOOL CDefectErrorCodeSelection::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CApplication *pApp = (CApplication*)AfxGetApp();

	COLORREF clrList = RGB(50, 50, 50);
	COLORREF clrText = RGB(255, 255, 255);
	CRect rctDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rctDesktop, NULL);
	m_fontList.CreateFont((int)(rctDesktop.Width() / 1280.0 * 15.52), 0, 0, 0, 350, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Segoe UI");


	m_listErrorCodeSelection.SetExtendedStyle(m_listErrorCodeSelection.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_TRANSPARENTBKGND| LVS_EX_FULLROWSELECT);
	m_listErrorCodeSelection.SetBkColor(clrList);
	m_listErrorCodeSelection.SetTextBkColor(clrList);
	m_listErrorCodeSelection.SetTextColor(clrText);

	CImageList imageList;
	imageList.Create(1, 23, ILC_COLOR, 1, 1);
	m_listErrorCodeSelection.SetImageList(&imageList, LVSIL_SMALL);
	m_listErrorCodeSelection.SetFont(&m_fontList);

	CRect rectArea;
	m_listErrorCodeSelection.GetClientRect(&rectArea);
	int nWidth = rectArea.Width();

	m_listErrorCodeSelection.InsertColumn(0, "Defects", LVCFMT_LEFT, int(nWidth*0.5), -1);
	m_listErrorCodeSelection.InsertColumn(1, "ErrorCode", LVCFMT_CENTER, int(nWidth*0.5), -1);

	
	int nTotalDefects = pApp->m_pImageSelectionDlg->m_wndDlg.getDefect().size();
	CString str;
	for (int i = 0; i < nTotalDefects; i++) {
		CString strDefectDisplay = pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[i].description.c_str();

		int nTrackIdx = -1, nDocIdx = -1;
		pApp->GetInformationFromErrorCode(pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[i].id, 
										pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[i].description.c_str(), nTrackIdx, nDocIdx);
		if (nTrackIdx != -1 && nDocIdx != -1) {
			CString strDoc;
			strDoc = nTrackIdx == 0 ? pApp->m_strTopCameraName[nDocIdx] : (nTrackIdx == 1 ? pApp->m_strBottomCameraName[nDocIdx] : pApp->m_strSideCameraName[nDocIdx]);
			strDefectDisplay.Format("%s_%s", strDoc, pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[i].description.c_str());
		}

		AddItem(&m_listErrorCodeSelection, strDefectDisplay, 0);
	}

	for (int i = 0; i < nTotalDefects; i++) {
		m_listErrorCodeSelection.SetItemText(i, 1, pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[i].code.c_str());
	}
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDefectErrorCodeSelection::OnCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = 0;		// Initialize with Default processing
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		*pResult = CDRF_NOTIFYITEMDRAW;		// Request msg for each item
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		*pResult = CDRF_NOTIFYSUBITEMDRAW;	// Request msg for each sub-item
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
		COLORREF clrList1 = RGB(70, 70, 70);
		COLORREF clrList2 = RGB(40, 40, 40);
		//pLVCD->clrText = m_colorInfo1[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem];
		pLVCD->clrTextBk = pLVCD->nmcd.dwItemSpec % 2 ? clrList1 : clrList2;
		*pResult = CDRF_DODEFAULT;			// Req. Windows to paint itself
	}
}


void CDefectErrorCodeSelection::AddItem(CListCtrl* pCtrl, CString strItem, int nIndex)
{
	LV_ITEM lvitem;

	lvitem.iSubItem = nIndex;
	lvitem.pszText = strItem.GetBuffer(0);
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = pCtrl->GetItemCount();
	pCtrl->InsertItem(&lvitem);
}

void CDefectErrorCodeSelection::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int nSel = m_listErrorCodeSelection.GetSelectionMark();
	CString strErrode;
	if (nSel != -1) {
		CApplication *pApp = (CApplication*)AfxGetApp();
		strErrode = m_listErrorCodeSelection.GetItemText(nSel, 1);
		m_nSelectedErrorCode = pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[nSel].id;
	}
	m_bIsConfirm = TRUE;
	m_bIsApplyAllDevice = FALSE;
	CDialogEx::OnOK();
}


void CDefectErrorCodeSelection::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	int nSel = m_listErrorCodeSelection.GetSelectionMark();
	CString strErrode;
	if (nSel != -1) {
		CApplication *pApp = (CApplication*)AfxGetApp();
		strErrode = m_listErrorCodeSelection.GetItemText(nSel, 1);
		m_nSelectedErrorCode = pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[nSel].id;
	}
	m_bIsConfirm = FALSE;
	m_bIsApplyAllDevice = FALSE;
	CDialogEx::OnOK();
}


void CDefectErrorCodeSelection::OnBnClickedButtonApplyAll()
{
	// TODO: Add your control notification handler code here
	int nSel = m_listErrorCodeSelection.GetSelectionMark();
	CString strErrode;
	if (nSel != -1) {
		CApplication *pApp = (CApplication*)AfxGetApp();
		strErrode = m_listErrorCodeSelection.GetItemText(nSel, 1);
		m_nSelectedErrorCode = pApp->m_pImageSelectionDlg->m_wndDlg.getDefect()[nSel].id;
	}
	m_bIsConfirm = FALSE;
	m_bIsApplyAllDevice = TRUE;
	CDialogEx::OnOK();
}
