// OpticsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "OpticsDlg.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COpticsDlg::COpticsDlg(CWnd* pParent /*=NULL*/)
	: CDialogBar()
{
	//{{AFX_DATA_INIT(COpticsDlg)
	m_nAvSize = 10;
	m_nTopOffset=20;
	m_nBottomOffset=20;
	m_nCircular=1;
	
	m_dAutoFocusStepSizeMM = 1;
	m_nAutoFocusStepNo = 20;
	m_bAutoFocusing = TRUE;
	m_nCurrentTrack = 0;
	m_nCurrentDoc = 0;

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

	m_brush1.CreateSolidBrush(RGB(30, 30, 30));
}

COpticsDlg::~COpticsDlg()
{
	m_brush1.DeleteObject();
	m_brush2.DeleteObject();
}

void COpticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpticsDlg)
	DDX_Control(pDX, IDC_FRAMECOUNT_COMBO, m_ctrlFrameCount);

	DDX_Text(pDX, IDC_FOCUSCOUNT_EDIT, m_nAvSize);

	DDX_Text(pDX, IDC_TOP_OFFSET_EDIT, m_nAutoFocusStepNo);
	DDX_Text(pDX, IDC_BOTTOM_OFFSET_EDIT, m_dAutoFocusStepSizeMM);

	//DDX_Text(pDX, IDC_RADIO1, m_nCircular);
	
	//DDX_Control(pDX, IDC_RADIO1, radio_ROI_Circular);
	//DDX_Control(pDX, IDC_RADIO2, radio_ROI_Rectangular);
	
	//}}AFX_DATA_MAP
//	DDV_MinMaxInt(pDX, m_nAvSize, 1, 100);
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


BEGIN_MESSAGE_MAP(COpticsDlg, CDialogBar)
	//{{AFX_MSG_MAP(COpticsDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OPTICS_SETPARM, SetParameters)
	ON_MESSAGE(WM_OPTICS_REFRESHGRAPH, RefreshGraph)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_FOCUSING, &COpticsDlg::OnBnClickedButtonAutoFocusing)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_AUTO_FOCUSING, OnUpdateButtonAutoFocusing)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpticsDlg message handlers


LRESULT COpticsDlg::SetParameters(WPARAM wParam, LPARAM lParam)
{
	OPTICS_PARM_DLG* pParm = (OPTICS_PARM_DLG*) wParam;
	if(m_hWnd==NULL)
		return 0;
	if(m_hWnd && ::IsWindow(m_hWnd) && IsVisible())
		UpdateData(TRUE);

	if(pParm->nCalcMode == 1) {
		m_nAccCount[pParm->nIndex]++;
		m_dMeanAccumulator[pParm->nIndex] += pParm->dMean;
		m_dSNRAccumulator[pParm->nIndex] += pParm->dSNR;
		m_dFocusAccumulator[pParm->nIndex] += pParm->dFocus;
		
		if(/*m_nAccCount[pParm->nIndex] >= m_nAvSize*/TRUE) {
			m_dOpticsMean[pParm->nIndex] = floor(m_dMeanAccumulator[pParm->nIndex]/m_nAccCount[pParm->nIndex]*10)/10;
			m_dOpticsSNR[pParm->nIndex] = floor(m_dSNRAccumulator[pParm->nIndex]/m_nAccCount[pParm->nIndex]*10)/10;
			m_dOpticsFocus[pParm->nIndex] = floor(m_dFocusAccumulator[pParm->nIndex]/m_nAccCount[pParm->nIndex]*100);
			
			m_nAccCount[pParm->nIndex] = 0;
			m_dMeanAccumulator[pParm->nIndex] = 0;
			m_dSNRAccumulator[pParm->nIndex] = 0;
			m_dFocusAccumulator[pParm->nIndex] = 0;
		}
	}
	else {
		m_dOpticsMean[pParm->nIndex] = floor(pParm->dMean*10)/10;
		m_dOpticsSNR[pParm->nIndex] = floor(pParm->dSNR*10)/10;
		m_dOpticsFocus[pParm->nIndex] = floor(pParm->dFocus*100);
		
		if(pParm->nCalcMode == 2) {
			m_nAccCount[pParm->nIndex] = 0;
			m_dMeanAccumulator[pParm->nIndex] = 0;
			m_dSNRAccumulator[pParm->nIndex] = 0;
			m_dFocusAccumulator[pParm->nIndex] = 0;
		}
	}

	if(m_nCircular){
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(TRUE);
		/*GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(FALSE);*/
	}
	else{	
		GetDlgItem(IDC_FRAMECOUNT_COMBO)->EnableWindow(FALSE);
		/*GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);
		GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(m_bCheckROI && m_nCircular==0);*/
	}

	GetDlgItem(IDC_TOP_OFFSET_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BOTTOM_OFFSET_EDIT)->EnableWindow(TRUE);

	if(::IsWindow(m_hWnd) && IsVisible())
		UpdateData(FALSE);

	return 0;
}

LRESULT COpticsDlg::RefreshGraph(WPARAM wParam, LPARAM lParam)
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
		m_wndLineChart.SetGlobalBounds(nMaxUpper, nMinLower);

		for(nIndex=0; nIndex<nFrames; nIndex++)
			m_wndLineChart.SetPos(nIndex, Round(m_dOpticsFocus[nIndex]));
		m_wndLineChart.DrawSpikeGlobal();
	}
	else{
		if(nMaxUpper < m_dOpticsFocus[0])
			nMaxUpper = Round(m_dOpticsFocus[0]);
		if(nMinLower > m_dOpticsFocus[0])
			nMinLower = Round(m_dOpticsFocus[0]);
		m_wndLineChart.SetGlobalBounds(nMaxUpper, nMinLower);
		
		m_wndLineChart.SetPos(0, Round(m_dOpticsFocus[0]));
		m_wndLineChart.DrawSpikeGlobal();


	}

	return 0;
}

void COpticsDlg::SetStats(int nFrames, int nFocusImages, int nTopOffset,int nBottomOffset)
{
	m_nAvSize = nFocusImages;
	m_nTopOffset =nTopOffset;
	m_nBottomOffset =nBottomOffset;
	((CComboBox*)GetDlgItem(IDC_FRAMECOUNT_COMBO))->SetCurSel(nFrames-1);

	UpdateData(FALSE);
	RepositionWindow(nFrames);
}

void COpticsDlg::GetStats(int& nFrames, int& nFocusImages, int& nTopOffset,int& nBottomOffset)
{
	UpdateData(TRUE);
	
	nFrames = m_ctrlFrameCount.GetCurSel() + 1;
	nFocusImages = m_nAvSize;
	nTopOffset =m_nTopOffset;
	nBottomOffset =m_nBottomOffset;
}

void COpticsDlg::OnResizeFrames(int& nFrames, int& nFocusImages,int& nTopOffset,int& nBottomOffset)
{
	int nFramesBk = nFrames;
	GetStats(nFrames, nFocusImages,nTopOffset,nBottomOffset);

	if(nFramesBk != nFrames) {
		RepositionWindow(nFrames);
		if(nFrames < nFramesBk) {
			for(int nIndex=nFrames; nIndex<nFramesBk; nIndex++)
				m_wndLineChart.ShowLine(nIndex, FALSE);
		}
		else {
			for(int nIndex=nFramesBk; nIndex<nFrames; nIndex++)
				m_wndLineChart.ShowLine(nIndex, TRUE);
		}
		m_wndLineChart.DrawSpikeGlobal();
	}
}

void COpticsDlg::RepositionWindow(int nFrames)
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

	CRect rect1, rect2, rect3, rect4;

	GetDlgItem(IDC_DATAGRID_STATIC)->GetWindowRect(&rect1);
	GetDlgItem(IDC_FOCUS1_EDIT+nFrames-1)->GetWindowRect(&rect2);
	GetDlgItem(IDC_DATAGRID_STATIC)->SetWindowPos(NULL, 0, 0, rect1.Width(), rect2.bottom + 10 - rect1.top, SWP_NOMOVE);

	GetDlgItem(IDC_DATAGRID_STATIC)->GetWindowRect(&rect1);
	ScreenToClient(rect1);
	GetDlgItem(IDC_FOCUS_LINECHART)->GetWindowRect(&rect2);
	ScreenToClient(rect2);
	GetDlgItem(IDC_FOCUS_LINECHART)->SetWindowPos(NULL, rect2.left, rect1.bottom + 5, 0, 0, SWP_NOSIZE);

	GetWindowRect(&rect1);
	GetDlgItem(IDC_BOTTOM_STATIC)->GetWindowRect(&rect2);

	GetDlgItem(IDC_FOCUS1_EDIT)->GetWindowRect(&rect3);
	GetDlgItem(IDC_FOCUS2_EDIT)->GetWindowRect(&rect4);

	int nHeight = rect2.bottom - rect1.top;
	nHeight -= (8-nFrames)*(rect4.top - rect3.top);

	CalcDynamicLayout(nHeight, LM_LENGTHY|LM_COMMIT);
	///Udupa; TODO
	((CMainFrame*)AfxGetMainWnd())->DisplayOpticsPanel(TRUE, nHeight);
}

BOOL COpticsDlg::Create( CWnd* pParentWnd, UINT nIDTemplate,
							  UINT nStyle, UINT nID, BOOL bChange)
{
	if(!CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID))
		return FALSE;
	
	m_bChangeDockedSize = bChange;
	if(!OnInitDialogBar())
		return FALSE;
	return TRUE;
}

BOOL COpticsDlg::Create(CWnd* pParentWnd,
						  LPCTSTR lpszTemplateName,
						  UINT nStyle, UINT nID, BOOL bChange)
{
	if (!CDialogBar::Create( pParentWnd, lpszTemplateName,
		nStyle, nID))
		return FALSE;
	
	m_bChangeDockedSize = bChange;
	if(!OnInitDialogBar())
		return FALSE;
	return TRUE;
}

CSize COpticsDlg::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	// Return default if it is being docked or floated
	if ((dwMode & LM_VERTDOCK) || (dwMode & LM_HORZDOCK))
	{
		if (dwMode & LM_STRETCH) // if not docked stretch to fit
			return CSize((dwMode & LM_HORZ) ? 32767 : m_sizeDocked.cx,
			(dwMode & LM_HORZ) ? m_sizeDocked.cy : 32767);
		else {
			GetDlgItem(IDC_FRAMECOUNT_COMBO)->SetFocus();
			return m_sizeDocked;
		}
	}

	if (dwMode & LM_MRUWIDTH)
		return m_sizeFloating;
	
	// In all other cases, accept the dynamic length
	if (dwMode & LM_LENGTHY) {
		return CSize(m_sizeFloating.cx, (m_bChangeDockedSize) ?
					m_sizeFloating.cy = m_sizeDocked.cy = nLength :
					m_sizeFloating.cy = nLength);
	}
	else {
		return CSize((m_bChangeDockedSize) ?
					m_sizeFloating.cx = m_sizeDocked.cx = nLength :
					m_sizeFloating.cx = nLength, m_sizeFloating.cy);
	}
}

BOOL COpticsDlg::OnInitDialogBar() 
{
	m_sizeFloating = m_sizeDocked = m_sizeDefault;
	
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

	GetDlgItem(IDC_BUTTON_AUTO_FOCUSING)->EnableWindow(TRUE);
	
	m_wndLineChart.SubclassDlgItem(IDC_FOCUS_LINECHART, this);
	m_wndLineChart.m_bIsDialogBar = TRUE;
	for(int nIndex=0; nIndex<8; nIndex++)
		m_wndLineChart.Add(m_clrROI[nIndex], 0, 0);

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COpticsDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetDlgItem(IDC_FRAMECOUNT_COMBO)->SetFocus();
	
	CDialogBar::OnLButtonDown(nFlags, point);
}

HBRUSH COpticsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME1_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	
    if( pWnd->GetDlgCtrlID() == IDC_FRAME2_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME3_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME4_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME5_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME6_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME7_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FRAME8_STATIC )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 0,0,0 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	//HBRUSH hbr = CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN1_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	
    if( pWnd->GetDlgCtrlID() == IDC_MEAN2_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN3_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN4_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN5_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB(230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN6_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB(230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN7_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_MEAN8_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE1_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	
    if( pWnd->GetDlgCtrlID() == IDC_NOISE2_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE3_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE4_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE5_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB(230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE6_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB(230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE7_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_NOISE8_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS1_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	
    if( pWnd->GetDlgCtrlID() == IDC_FOCUS2_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS3_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS4_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS5_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB(230,230,230));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS6_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB(230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS7_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_FOCUS8_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 230,230,230 ));
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
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_FRAME )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_MEAN )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_SNR )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_FOCUS )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	if( pWnd->GetDlgCtrlID() == ID_VIEW_OPTICS_ROI )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }
	
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_TOP_OFFSET )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }

	if( pWnd->GetDlgCtrlID() == IDC_STATIC_BOTTOM_OFFSET )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 68,174,255 ));
        pDC->SetBkMode( TRANSPARENT );
    }

	if( pWnd->GetDlgCtrlID() == IDC_TOP_OFFSET_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB( 30,30,30 ));
    }
	if( pWnd->GetDlgCtrlID() == IDC_BOTTOM_OFFSET_EDIT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB( 30,30,30 ));
    }

	if( pWnd->GetDlgCtrlID() == IDC_STATIC_MESSAGE )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,100,100 ));
        pDC->SetBkMode( TRANSPARENT );
    }

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


void COpticsDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	m_nCircular=1;
	EnableEditControls();
}

void COpticsDlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	m_nCircular=0;	
	EnableEditControls();
}

void COpticsDlg::EnableEditControls()
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
void COpticsDlg::DisplayMessage(bool m_nFlag)
{
	if(m_nFlag){
		GetDlgItem(IDC_FRAME1_STATIC)->ShowWindow(FALSE);
		GetDlgItem(IDC_MEAN1_EDIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_NOISE1_EDIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FOCUS1_EDIT)->ShowWindow(FALSE);
		//GetDlgItem(IDC_STATIC_FRAME_TITLE)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_MESSAGE)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_MESSAGE2)->ShowWindow(TRUE);
		
	}
	else{
		GetDlgItem(IDC_FRAME1_STATIC)->ShowWindow(TRUE);
		GetDlgItem(IDC_MEAN1_EDIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_NOISE1_EDIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_FOCUS1_EDIT)->ShowWindow(TRUE);
		//GetDlgItem(IDC_STATIC_FRAME_TITLE)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_MESSAGE)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_MESSAGE2)->ShowWindow(FALSE);
		
	}

}

void COpticsDlg::OnUpdateButtonAutoFocusing(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bAutoFocusing);
}

void COpticsDlg::OnBnClickedButtonAutoFocusing()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_bAutoFocusing = FALSE;
	theApp.m_ThreadManager.CreateThread(TrackAutoFocusing, this, TRACK_THREAD, -1);
}
