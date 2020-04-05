// OpticsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "OpticsDialog.h"
#include "afxdialogex.h"
#include "MainFrame.h"
#include "TrackHandler.h"

// COpticsDialog dialog

IMPLEMENT_DYNAMIC(COpticsDialog, CDialogEx)

COpticsDialog::COpticsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpticsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpticsDlg)
	m_nAvSize = 10;
	m_nTopOffset=20;
	m_nBottomOffset=20;
	m_nCircular=1;
	
	//}}AFX_DATA_INIT
	for(int i=0; i<8; i++) {
		m_dOpticsMean[i] = 0.0;
		m_dOpticsSNR[i] = 0.0;
		m_dOpticsFocus[i] = 0.0;
		m_dMeanAccumulator[i] = 0;
		m_dFocusAccumulator[i] = 0;
		m_dSNRAccumulator[i] = 0;
		m_nAccCount[i] = 0;
	}
	m_clrROI[0] = RGB(255,  0,  0);
	m_clrROI[1] = RGB(  0,255,  0);
	m_clrROI[2] = RGB(  0,  0,255);
	m_clrROI[3] = RGB(255,255,  0);
	m_clrROI[4] = RGB(0,  255,255);
	m_clrROI[5] = RGB(255,  0,255);
	m_clrROI[6] = RGB(255,255,255);
	m_clrROI[7] = RGB(128,128,128);

	m_bCheckROI=false;

	m_brush1.CreateSolidBrush(RGB(100, 100, 100));
}

COpticsDialog::~COpticsDialog()
{
	m_brush1.DeleteObject();
	m_brush2.DeleteObject();
}

void COpticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAMECOUNT_COMBO, m_ctrlFrameCount);
	DDX_Text(pDX, IDC_FOCUSCOUNT_EDIT, m_nAvSize);
	DDX_Text(pDX, IDC_TOP_OFFSET_EDIT, m_nTopOffset);
	DDX_Text(pDX, IDC_BOTTOM_OFFSET_EDIT, m_nBottomOffset);
	DDX_Radio(pDX, IDC_RADIO_OPTS_CIRCULAR, m_nCircular);

	DDX_Text(pDX, IDC_MEAN1_EDIT, m_dOpticsMean[0]);
	DDX_Text(pDX, IDC_MEAN2_EDIT, m_dOpticsMean[1]);
	DDX_Text(pDX, IDC_MEAN3_EDIT, m_dOpticsMean[2]);
	DDX_Text(pDX, IDC_MEAN4_EDIT, m_dOpticsMean[3]);
	DDX_Text(pDX, IDC_MEAN5_EDIT, m_dOpticsMean[4]);
	DDX_Text(pDX, IDC_MEAN6_EDIT, m_dOpticsMean[5]);
	DDX_Text(pDX, IDC_MEAN7_EDIT, m_dOpticsMean[6]);
	DDX_Text(pDX, IDC_MEAN8_EDIT, m_dOpticsMean[7]);
	DDX_Text(pDX, IDC_NOISE1_EDIT, m_dOpticsSNR[0]);
	DDX_Text(pDX, IDC_NOISE2_EDIT, m_dOpticsSNR[1]);
	DDX_Text(pDX, IDC_NOISE3_EDIT, m_dOpticsSNR[2]);
	DDX_Text(pDX, IDC_NOISE4_EDIT, m_dOpticsSNR[3]);
	DDX_Text(pDX, IDC_NOISE5_EDIT, m_dOpticsSNR[4]);
	DDX_Text(pDX, IDC_NOISE6_EDIT, m_dOpticsSNR[5]);
	DDX_Text(pDX, IDC_NOISE7_EDIT, m_dOpticsSNR[6]);
	DDX_Text(pDX, IDC_NOISE8_EDIT, m_dOpticsSNR[7]);
	DDX_Text(pDX, IDC_FOCUS1_EDIT, m_dOpticsFocus[0]);
	DDX_Text(pDX, IDC_FOCUS2_EDIT, m_dOpticsFocus[1]);
	DDX_Text(pDX, IDC_FOCUS3_EDIT, m_dOpticsFocus[2]);
	DDX_Text(pDX, IDC_FOCUS4_EDIT, m_dOpticsFocus[3]);
	DDX_Text(pDX, IDC_FOCUS5_EDIT, m_dOpticsFocus[4]);
	DDX_Text(pDX, IDC_FOCUS6_EDIT, m_dOpticsFocus[5]);
	DDX_Text(pDX, IDC_FOCUS7_EDIT, m_dOpticsFocus[6]);
	DDX_Text(pDX, IDC_FOCUS8_EDIT, m_dOpticsFocus[7]);
}


BEGIN_MESSAGE_MAP(COpticsDialog, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_OPTS_RECT, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO_OPTS_CIRCULAR, OnRadio1)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OPTICS_SETPARM, SetParameters)
	ON_MESSAGE(WM_OPTICS_REFRESHGRAPH, RefreshGraph)
	ON_CBN_SELCHANGE(IDC_FRAMECOUNT_COMBO, &COpticsDialog::OnCbnSelchangeFramecountCombo)
	ON_BN_CLICKED(IDOK, &COpticsDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COpticsDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// COpticsDialog message handlers

LRESULT COpticsDialog::SetParameters(WPARAM wParam, LPARAM lParam)
{
	OPTICS_PARM* pParm = (OPTICS_PARM*) wParam;
	//if(m_hWnd==NULL)
	//	return 0;
	//if(m_hWnd && ::IsWindow(m_hWnd) && IsVisible())
		UpdateData(TRUE);

	if(pParm->nCalcMode == 1) {
		m_nAccCount[pParm->nIndex]++;
		m_dMeanAccumulator[pParm->nIndex] += pParm->dMean;
		m_dSNRAccumulator[pParm->nIndex] += pParm->dSNR;
		m_dFocusAccumulator[pParm->nIndex] += pParm->dFocus;
		
		if(m_nAccCount[pParm->nIndex] >= m_nAvSize) {
			m_dOpticsMean[pParm->nIndex] = floor(m_dMeanAccumulator[pParm->nIndex]/m_nAccCount[pParm->nIndex]*10)/10;
			m_dOpticsSNR[pParm->nIndex] = floor(m_dSNRAccumulator[pParm->nIndex]/m_nAccCount[pParm->nIndex]*10)/10;
			m_dOpticsFocus[pParm->nIndex] = floor(m_dFocusAccumulator[pParm->nIndex]/m_nAccCount[pParm->nIndex]*10)/10;
			
			m_nAccCount[pParm->nIndex] = 0;
			m_dMeanAccumulator[pParm->nIndex] = 0;
			m_dSNRAccumulator[pParm->nIndex] = 0;
			m_dFocusAccumulator[pParm->nIndex] = 0;
		}
	}
	else {
		m_dOpticsMean[pParm->nIndex] = floor(pParm->dMean*10)/10;
		m_dOpticsSNR[pParm->nIndex] = floor(pParm->dSNR*10)/10;
		m_dOpticsFocus[pParm->nIndex] = floor(pParm->dFocus*10)/10;
		
		if(pParm->nCalcMode == 2) {
			m_nAccCount[pParm->nIndex] = 0;
			m_dMeanAccumulator[pParm->nIndex] = 0;
			m_dSNRAccumulator[pParm->nIndex] = 0;
			m_dFocusAccumulator[pParm->nIndex] = 0;
		}
	}

	if(m_nCircular){
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(TRUE);
		GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(FALSE);
	}
	else{	
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
	}

	//if(::IsWindow(m_hWnd) && IsVisible())
		UpdateData(FALSE);

	return 0;
}

LRESULT COpticsDialog::RefreshGraph(WPARAM wParam, LPARAM lParam)
{
	int nFrames = (int) wParam;
	int nMaxUpper=-1, nMinLower=INT_MAX;
	if(m_nCircular){
		int nIndex;
		for(nIndex=0; nIndex<nFrames; nIndex++) {
			if(nMaxUpper < m_dOpticsFocus[nIndex])
				nMaxUpper = Round(m_dOpticsFocus[nIndex]);
			if(nMinLower > m_dOpticsFocus[nIndex])
				nMinLower = Round(m_dOpticsFocus[nIndex]);
		}
		//m_wndLineChart.SetGlobalBounds(nMaxUpper, nMinLower);

		//for(nIndex=0; nIndex<nFrames; nIndex++)
		//	m_wndLineChart.SetPos(nIndex, Round(m_dOpticsFocus[nIndex]));
		//m_wndLineChart.DrawSpikeGlobal();
	}
	else{
		if(nMaxUpper < m_dOpticsFocus[0])
			nMaxUpper = Round(m_dOpticsFocus[0]);
		if(nMinLower > m_dOpticsFocus[0])
			nMinLower = Round(m_dOpticsFocus[0]);
		//m_wndLineChart.SetGlobalBounds(nMaxUpper, nMinLower);
		//
		//m_wndLineChart.SetPos(0, Round(m_dOpticsFocus[0]));
		//m_wndLineChart.DrawSpikeGlobal();


	}

	return 0;
}

void COpticsDialog::SetStats(int nFrames, int nFocusImages, int nTopOffset,int nBottomOffset)
{
	//m_nAvSize = nFocusImages;
	//m_nTopOffset = nTopOffset;
	//m_nBottomOffset = nBottomOffset;
	((CComboBox*)GetDlgItem(IDC_FRAMECOUNT_COMBO))->SetCurSel(nFrames-1);

	UpdateData(FALSE);
	RepositionWindow(nFrames);
}

void COpticsDialog::GetStats(int& nFrames, int& nFocusImages, int& nTopOffset,int& nBottomOffset)
{
	UpdateData(TRUE);
	
	nFrames = m_ctrlFrameCount.GetCurSel() + 1;
	//nFocusImages = m_nAvSize;
	//nTopOffset =m_nTopOffset;
	//nBottomOffset =m_nBottomOffset;
}

void COpticsDialog::OnResizeFrames(int& nFrames, int& nFocusImages,int& nTopOffset,int& nBottomOffset)
{
	int nFramesBk = nFrames;
	GetStats(nFrames, nFocusImages,nTopOffset,nBottomOffset);

	if(nFramesBk != nFrames) {
		RepositionWindow(nFrames);
		//if(nFrames < nFramesBk) {
		//	for(int nIndex=nFrames; nIndex<nFramesBk; nIndex++)
		//		m_wndLineChart.ShowLine(nIndex, FALSE);
		//}
		//else {
		//	for(int nIndex=nFramesBk; nIndex<nFrames; nIndex++)
		//		m_wndLineChart.ShowLine(nIndex, TRUE);
		//}
		//m_wndLineChart.DrawSpikeGlobal();
	}
}

void COpticsDialog::RepositionWindow(int nFrames)
{
	int i;
	
	for(i=0; i<nFrames; i++) {
		GetDlgItem(IDC_FRAME1_STATIC+i)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MEAN1_EDIT+i)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_NOISE1_EDIT+i)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_FOCUS1_EDIT+i)->ShowWindow(SW_SHOW);
	}

	for(i=nFrames; i<8; i++) {
		GetDlgItem(IDC_FRAME1_STATIC+i)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MEAN1_EDIT+i)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NOISE1_EDIT+i)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FOCUS1_EDIT+i)->ShowWindow(SW_HIDE);
	}

	//CRect rect1, rect2, rect3, rect4;

	//GetDlgItem(IDC_DATAGRID_STATIC)->GetWindowRect(&rect1);
	//GetDlgItem(IDC_FOCUS1_EDIT+nFrames-1)->GetWindowRect(&rect2);
	//GetDlgItem(IDC_DATAGRID_STATIC)->SetWindowPos(NULL, 0, 0, rect1.Width(), rect2.bottom + 10 - rect1.top, SWP_NOMOVE);

	//GetDlgItem(IDC_DATAGRID_STATIC)->GetWindowRect(&rect1);
	//ScreenToClient(rect1);
	//GetDlgItem(IDC_FOCUS_LINECHART)->GetWindowRect(&rect2);
	//ScreenToClient(rect2);
	//GetDlgItem(IDC_FOCUS_LINECHART)->SetWindowPos(NULL, rect2.left, rect1.bottom + 5, 0, 0, SWP_NOSIZE);

	//GetWindowRect(&rect1);
	//GetDlgItem(IDC_BOTTOM_STATIC)->GetWindowRect(&rect2);

	//GetDlgItem(IDC_FOCUS1_EDIT)->GetWindowRect(&rect3);
	//GetDlgItem(IDC_FOCUS2_EDIT)->GetWindowRect(&rect4);

	//int nHeight = rect2.bottom - rect1.top;
	//nHeight -= (8-nFrames)*(rect4.top - rect3.top);

	//CalcDynamicLayout(nHeight, LM_LENGTHY|LM_COMMIT);
	///Udupa; TODO
	///((CMainFrame*)AfxGetMainWnd())->DisplayOpticsPanel(TRUE, nHeight);
}

BOOL COpticsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	if(m_nCircular){
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(TRUE);
		GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(FALSE);
		
	}
	else{
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
	}
	
	
	//m_wndLineChart.SubclassDlgItem(IDC_FOCUS_LINECHART, this);
	//m_wndLineChart.m_bIsDialogBar = TRUE;
	//for(int nIndex=0; nIndex<8; nIndex++)
	//	m_wndLineChart.Add(m_clrROI[nIndex], 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COpticsDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetDlgItem(IDC_FRAMECOUNT_COMBO)->SetFocus();
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

HBRUSH COpticsDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{	
	int Id = pWnd->GetDlgCtrlID();
	
	if( Id == IDC_FRAME1_STATIC || Id == IDC_FRAME2_STATIC ||
		Id == IDC_FRAME3_STATIC || Id == IDC_FRAME4_STATIC ||
		Id == IDC_FRAME5_STATIC || Id == IDC_FRAME6_STATIC ||
		Id == IDC_FRAME7_STATIC || Id == IDC_FRAME8_STATIC){
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	//HBRUSH hbr = CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if( Id == IDC_MEAN1_EDIT || Id == IDC_MEAN2_EDIT ||
		Id == IDC_MEAN3_EDIT || Id == IDC_MEAN4_EDIT ||
		Id == IDC_MEAN5_EDIT || Id == IDC_MEAN6_EDIT ||
		Id == IDC_MEAN7_EDIT || Id == IDC_MEAN8_EDIT ||
		Id == IDC_NOISE1_EDIT || Id == IDC_NOISE2_EDIT ||
		Id == IDC_NOISE3_EDIT || Id == IDC_NOISE4_EDIT ||
		Id == IDC_NOISE5_EDIT || Id == IDC_NOISE6_EDIT ||
		Id == IDC_NOISE7_EDIT || Id == IDC_NOISE8_EDIT ||
		Id == IDC_FOCUS1_EDIT || Id == IDC_FOCUS2_EDIT ||
		Id == IDC_FOCUS3_EDIT || Id == IDC_FOCUS4_EDIT ||
		Id == IDC_FOCUS5_EDIT || Id == IDC_FOCUS6_EDIT ||
		Id == IDC_FOCUS7_EDIT || Id == IDC_FOCUS8_EDIT)
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	if( pWnd->GetDlgCtrlID() == IDC_GROUPBOX_ROI )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE1_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }

	if(nCtlColor==CTLCOLOR_STATIC && pWnd->GetDlgCtrlID()>=IDC_FRAME1_STATIC && pWnd->GetDlgCtrlID()<=IDC_FRAME8_STATIC) {
		m_brush2.DeleteObject();
		m_brush2.CreateSolidBrush(m_clrROI[pWnd->GetDlgCtrlID()-IDC_FRAME1_STATIC]);
		return m_brush2;
	}
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_ACTIVE_FRAME )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
    if( pWnd->GetDlgCtrlID() == IDC_STATIC_AVERAGING_SIZE )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUSCOUNT_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB( 30,30,30 ));
    }
	
	if( Id == IDC_STATIC_FRAME || Id == IDC_STATIC_MEAN || Id == IDC_STATIC_SNR || Id == IDC_STATIC_FOCUS)
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
		
	
	if( Id == ID_VIEW_OPTICS_ROI || Id == IDC_STATIC_TOP_OFFSET || Id == IDC_STATIC_BOTTOM_OFFSET)
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( Id == IDC_TOP_OFFSET_EDIT || Id == IDC_BOTTOM_OFFSET_EDIT)
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB( 30,30,30 ));
    }

	//if( pWnd->GetDlgCtrlID() == IDC_STATIC_MESSAGE )
 //   {
 //       // Set color as red.
 //       pDC->SetTextColor( RGB( 255,100,100 ));
 //       pDC->SetBkMode( TRANSPARENT );
 //   }

	//if( pWnd->GetDlgCtrlID() == IDC_STATIC_MESSAGE2 )
 //   {
 //       // Set color as red.
 //       pDC->SetTextColor( RGB( 255,255,255 ));
 //       pDC->SetBkMode( TRANSPARENT );
 //   }

	if(pWnd->GetDlgCtrlID() == IDC_FRAMECOUNT_COMBO) {
        pDC->SetTextColor(RGB(250,250,250));
        pDC->SetBkColor(RGB(30,30,30));
	}
	
	//----------------------------------------
	//pDC->SetBkMode( TRANSPARENT );
	
	return m_brush1;
}


void COpticsDialog::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	m_nCircular = 1;
	EnableEditControls();
}

void COpticsDialog::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	m_nCircular = 0;	
	EnableEditControls();
}

void COpticsDialog::EnableEditControls()
{
	if(m_nCircular){
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(TRUE);
		GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(FALSE);
	}
	else{
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
	}

}

void COpticsDialog::DisplayMessage(bool m_nFlag)
{
	if(m_nFlag){
		GetDlgItem(IDC_FRAME1_STATIC)->ShowWindow(FALSE);
		GetDlgItem(IDC_MEAN1_EDIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_NOISE1_EDIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FOCUS1_EDIT)->ShowWindow(FALSE);
		//GetDlgItem(IDC_STATIC_FRAME_TITLE)->ShowWindow(FALSE);
		//GetDlgItem(IDC_STATIC_MESSAGE)->ShowWindow(TRUE);
		//GetDlgItem(IDC_STATIC_MESSAGE2)->ShowWindow(TRUE);
		
	}
	else{
		GetDlgItem(IDC_FRAME1_STATIC)->ShowWindow(TRUE);
		GetDlgItem(IDC_MEAN1_EDIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_NOISE1_EDIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_FOCUS1_EDIT)->ShowWindow(TRUE);
		//GetDlgItem(IDC_STATIC_FRAME_TITLE)->ShowWindow(TRUE);
		//GetDlgItem(IDC_STATIC_MESSAGE)->ShowWindow(FALSE);
		//GetDlgItem(IDC_STATIC_MESSAGE2)->ShowWindow(FALSE);
		
	}

}



void COpticsDialog::OnCbnSelchangeFramecountCombo()
{
	UpdateData(TRUE);
	//SendMessage(WM_OPTICS_FRAMECHANGED, 0, 0);
	m_pDoc->OnSelchangeFramecountCombo();
}


void COpticsDialog::OnBnClickedOk()
{
	if(m_pDoc->m_pTrack->m_bOpticsRoi)
		m_pDoc->OnViewOpticsRoi();
	m_pDoc->m_pTrack->m_bViewOpticsPanel = FALSE;
	CDialogEx::OnOK();
}


void COpticsDialog::OnBnClickedCancel()
{
	if(m_pDoc->m_pTrack->m_bOpticsRoi)
		m_pDoc->OnViewOpticsRoi();
	m_pDoc->m_pTrack->m_bViewOpticsPanel = FALSE;
	CDialogEx::OnCancel();
}
