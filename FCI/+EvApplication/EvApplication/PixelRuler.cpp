// PixelRuler.cpp : implementation file
//

#include "stdafx.h"
#include "PixelRuler.h"
#include "afxdialogex.h"
#include "MathAlgo.h"
#include "Application.h"
// CPixelRuler dialog

IMPLEMENT_DYNAMIC(CPixelRuler, CDialogEx)

CPixelRuler::CPixelRuler(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPixelRuler::IDD, pParent)
{
	m_strDirectionX = "X: -- ";
	m_strDirectionY = "Y: -- ";
	m_strDistanceInPixel = "-- pix";
	m_strDistanceInMicron = "-- um";
	m_strAngleInDegrees = "-- deg";
	m_strAngleInRadians = "-- rad";
}

CPixelRuler::~CPixelRuler()
{
	m_brush.DeleteObject();
}

void CPixelRuler::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DIRECTION_X, m_strDirectionX);
	DDX_Text(pDX, IDC_STATIC_DIRECTION_Y, m_strDirectionY);
	DDX_Text(pDX, IDC_STATIC_DISTANCE_IN_PIXEL, m_strDistanceInPixel);
	DDX_Text(pDX, IDC_STATIC_DISTANCE_IN_MICRON, m_strDistanceInMicron);
	DDX_Text(pDX, IDC_STATIC_ANGLE_IN_DEGREES, m_strAngleInDegrees);
	DDX_Text(pDX, IDC_STATIC_ANGLE_IN_RADIANS, m_strAngleInRadians);
}


BEGIN_MESSAGE_MAP(CPixelRuler, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CPixelRuler::UpdateDatas(CPoint point1, CPoint point2, double dResolution, double dResolutionX, double dResolutionY, BOOL bImperialUnit)
{
	UpdateData();

	m_strDirectionX.Format("X: %d pix (%.3f um)", point2.x - point1.x, (point2.x - point1.x)*dResolutionX);
	m_strDirectionY.Format("Y: %d pix (%.3f um)", point2.y - point1.y, (point2.y - point1.y)*dResolutionY);

	double dPixel = sqrt((double)((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.y - point1.y)));
	m_strDistanceInPixel.Format("%.3f pix", dPixel);

	if (bImperialUnit)
		m_strDistanceInMicron.Format("%.3f mil", (dPixel * dResolution) / 25.4);
	else
		m_strDistanceInMicron.Format("%.3f um", dPixel * dResolution);

	double xDist = point2.x - point1.x;
	double yDist = point2.y - point1.y;
	double dAngle = atan2(yDist, xDist);

	m_strAngleInDegrees.Format("%.4f deg", dAngle * 180 / PI);
	m_strAngleInRadians.Format("%.4f rad", dAngle);

	UpdateData(FALSE);
}

HBRUSH CPixelRuler::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	int nId = pWnd->GetDlgCtrlID();
	if(nId == IDC_STATIC_DISTANCE || 
		nId == IDC_STATIC_ANGLE ||
		nId == IDC_STATIC_DIRECTION)
    {
		pDC->SetTextColor( RGB(255,147,25));
        pDC->SetBkMode( TRANSPARENT );
    }
	else if(nId == IDC_STATIC_DISTANCE_IN_PIXEL ||
		nId == IDC_STATIC_DISTANCE_IN_MICRON ||
		nId == IDC_STATIC_ANGLE_IN_DEGREES ||
		nId == IDC_STATIC_ANGLE_IN_RADIANS)
    {
        pDC->SetTextColor( RGB(200,210,250));
        pDC->SetBkMode( TRANSPARENT );
    }
	else if(nId == IDC_STATIC_DIRECTION_X || nId == IDC_STATIC_DIRECTION_Y) {
        pDC->SetTextColor( RGB(200,200,200));
        pDC->SetBkMode( TRANSPARENT );
    }

	return m_brush;
}


BOOL CPixelRuler::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	m_brush.CreateSolidBrush(RGB(50, 50, 50));

	return TRUE;
}


void CPixelRuler::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CApplication *pApp= (CApplication*) AfxGetApp();
	if(pApp->m_bPixelRuler)
		pApp->OnPixelRuler();
	//CDialogEx::OnClose();
}
