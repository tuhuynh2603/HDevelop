// YieldTrendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "YieldTrendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CYieldTrendDlg dialog


CYieldTrendDlg::CYieldTrendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYieldTrendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CYieldTrendDlg)
	m_dYieldTrk1 = 0.0;
	m_dYieldTrk2 = 0.0;
	m_dYieldTrk3 = 0.0;
	m_dYieldTrk4 = 0.0;
	m_dYieldTrk5 = 0.0;
	//}}AFX_DATA_INIT
}

CYieldTrendDlg::CYieldTrendDlg(CAppDoc* pDoc /*=NULL*/)
{
	m_brush.DeleteObject();
}

CYieldTrendDlg::~CYieldTrendDlg()
{
	//m_pDoc2->m_pTrackHandler->m_bYieldTrendDlgOpen = FALSE;
}

void CYieldTrendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYieldTrendDlg)
	DDX_Text(pDX, IDC_TRK1_YIELD, m_dYieldTrk1);
	DDX_Text(pDX, IDC_TRK2_YIELD, m_dYieldTrk2);
	DDX_Text(pDX, IDC_TRK3_YIELD, m_dYieldTrk3);
	DDX_Text(pDX, IDC_TRK4_YIELD, m_dYieldTrk4);
	DDX_Text(pDX, IDC_TRK5_YIELD, m_dYieldTrk5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CYieldTrendDlg, CDialog)
	//{{AFX_MSG_MAP(CYieldTrendDlg)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYieldTrendDlg message handlers

BOOL CYieldTrendDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_brush.CreateSolidBrush(RGB(30, 30, 30));
	
	
	m_pApp = (CApplication*) AfxGetApp();
	

	m_dYieldTrk1 = m_pApp->m_LotSummary.m_DisplaySummary[0].dTotalYield;
	m_dYieldTrk2 = m_pApp->m_LotSummary.m_DisplaySummary[1].dTotalYield;
	m_dYieldTrk3 = m_pApp->m_LotSummary.m_DisplaySummary[2].dTotalYield;
	m_dYieldTrk4 = m_pApp->m_LotSummary.m_DisplaySummary[3].dTotalYield;
	m_dYieldTrk5 = m_pApp->m_LotSummary.m_DisplaySummary[4].dTotalYield;


	m_wndLineChart.SubclassDlgItem(IDC_LINE_CHART_TRK1_CTRL, this);
	m_wndLineChart.Add(RGB(255,255,0),100, 0);
	m_wndLineChart.Add(RGB(0,255,0),100, 0);
	m_wndLineChart.Add(RGB(200,0,200),100, 0);
	m_wndLineChart.Add(RGB(255,0,0),100, 0);
	m_wndLineChart.Add(RGB(0,255,255),100, 0);
	
	SetTimer(1, 500, NULL);
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control

}

void CYieldTrendDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CYieldTrendDlg::OnTimer(UINT_PTR nIDEvent)
{
	CTime t = CTime::GetCurrentTime();
	int   nRandom;
	m_pApp = (CApplication*) AfxGetApp();
	
	srand(t.GetSecond());

	nRandom = int(m_pApp->m_LotSummary.m_DisplaySummary[0].dTotalYield);
	if(nRandom == 0)
		nRandom+=1;
	if(nRandom == 100)
		nRandom-=1;
	
	m_wndLineChart.SetPos(0,nRandom);

	nRandom = int(m_pApp->m_LotSummary.m_DisplaySummary[1].dTotalYield);
	if(nRandom == 0)
		nRandom+=1;
	if(nRandom == 100)
		nRandom-=1;
	m_wndLineChart.SetPos(1,nRandom);

	nRandom = int(m_pApp->m_LotSummary.m_DisplaySummary[2].dTotalYield);
	if(nRandom == 0)
		nRandom+=1;
	if(nRandom == 100)
		nRandom-=1;
	m_wndLineChart.SetPos(2,nRandom);

	nRandom = int(m_pApp->m_LotSummary.m_DisplaySummary[3].dTotalYield);
	if(nRandom == 0)
		nRandom+=1;
	if(nRandom == 100)
		nRandom-=1;
	m_wndLineChart.SetPos(3,nRandom);

	nRandom = int(m_pApp->m_LotSummary.m_DisplaySummary[4].dTotalYield);
	if(nRandom == 0)
		nRandom+=1;
	if(nRandom == 100)
		nRandom-=1;
	m_wndLineChart.SetPos(4,nRandom);

	m_wndLineChart.Go();

	m_dYieldTrk1 = m_pApp->m_LotSummary.m_DisplaySummary[0].dTotalYield;
	m_dYieldTrk2 = m_pApp->m_LotSummary.m_DisplaySummary[1].dTotalYield;
	m_dYieldTrk3 = m_pApp->m_LotSummary.m_DisplaySummary[2].dTotalYield;
	m_dYieldTrk4 = m_pApp->m_LotSummary.m_DisplaySummary[3].dTotalYield;
	m_dYieldTrk5 = m_pApp->m_LotSummary.m_DisplaySummary[4].dTotalYield;

	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}

HBRUSH CYieldTrendDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired

	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_1 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_2 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_3 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_4 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

		if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_5 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_6 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_7 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_8 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

		if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_9 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_LEVEL_10 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 220,220,220 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_1 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_50 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

		if( pWnd->GetDlgCtrlID() == IDC_STATIC_100 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_TRACK1 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_TRACK2 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_TRACK3 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

		if( pWnd->GetDlgCtrlID() == IDC_STATIC_TRACK4 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_TRACK5 )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 80,180,255 ));
        pDC->SetBkColor( RGB(30,30,30) );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_TRACK1_PERCENT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_TRACK2_PERCENT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_TRACK3_PERCENT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }
	
	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_TRACK4_PERCENT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

	if( pWnd->GetDlgCtrlID() == IDC_STATICIDC_STATIC_TRACK5_PERCENT )
    {
        // Set color as red.
        pDC->SetTextColor( RGB( 255,255,255 ));
        pDC->SetBkColor( RGB(30,30,30)  );
    }

	return m_brush;
}
