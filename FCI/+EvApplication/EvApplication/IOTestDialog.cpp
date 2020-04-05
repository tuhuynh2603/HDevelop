// IOTestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "IOTestDialog.h"
#include "afxdialogex.h"
#include "ApplicationDef.h"
#include "MotionControlDialog.h"

static const UINT_PTR IO_CHECK_TIMER = 110;
// CIOTestDialog dialog

IMPLEMENT_DYNAMIC(CIOTestDialog, CDialogEx)

CIOTestDialog::CIOTestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIOTestDialog::IDD, pParent)
{
	m_nCounter = 0;
	m_nNoOfSegments = 2;

	m_pApp = (CApplication*) AfxGetApp();

	m_brushGreen.CreateSolidBrush(RGB(50,250,50));
	m_brushRed.CreateSolidBrush(RGB(220,50,50));
	m_brushBlack.CreateSolidBrush(RGB(0,0,0));
	m_brushYellow.CreateSolidBrush(RGB(255, 255, 255));
}

CIOTestDialog::~CIOTestDialog()
{
	m_brushGreen.DeleteObject();
	m_brushRed.DeleteObject();
	m_brushBlack.DeleteObject();
	m_brushYellow.DeleteObject();

	for(int i=0; i<m_nNoOfSegments; i++)
		m_prgnLightSegment[i].DeleteObject();
}

void CIOTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIOTestDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CIOTestDialog::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CIOTestDialog message handlers

BOOL CIOTestDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int nCardNo = 0;
	int nPortNo = 1;
	UINT pDataIn = 0;
	CRect rect;
	for(int i=0; i<m_nNoOfSegments; i++) {
		m_nIconStatus[i] = 0;
		GetDlgItem(IDC_SEGMENT0+i)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_prgnLightSegment[i].CreateEllipticRgn(rect.left-2, rect.top-2, rect.right+2, rect.bottom+2);
	}

	//Previously we use "I_7230.dll", now we change to "M_7856.dll" that already loaded to m_MotorResource interface
	CString str;
	/*if (m_pApp->m_CommModule.hDLL) {
		str.Format("I/O Module: %s", m_pApp->m_strIOInterfaceCard);

		if (m_nCounter % 2 == 0)
			str = "/" + str;
		else
			str = "\\" + str;

		m_pApp->m_MotorResource.InPortReadBit(nCardNo, nPortNo, IN_MOTOR_CURTAIN_SENSOR, &pDataIn);
		if (pDataIn)
			m_nIconStatus[0] = 2;
		else
			m_nIconStatus[0] = 3;

	}
	else {
		str = "No I/O module loaded";
	}*/


	//m_pApp->m_MotorResource.InPortReadBit(nCardNo, nPortNo, OUT_MOTOR_CONTACTOR_CONTROL, &pDataIn);
	//if (pDataIn && OUT_MOTOR_CONTACTOR_CONTROL)
	//	m_bChecked[1] = TRUE;

	
	
	//Check the IO sensor
	if (m_pApp->m_MotorResource.isDllOpened()) {
		//1. Curtain Sensor
		m_pApp->m_MotorResource.InPortReadBit(nCardNo, nPortNo, IN_MOTOR_CURTAIN_SENSOR, &pDataIn);
		if (pDataIn)
			m_nIconStatus[0] = 2;
		else
			m_nIconStatus[0] = 3;

		//2. Emergency Stop Output Signal of Motion Axis
		m_nIconStatus[1] = 2;

		pDataIn = 0;
		long DataIn;
		for (int i = 0; i < NO_OF_AXIS; i++) {
			m_pApp->motionParameters.GetMotorIOStatus(i, DataIn);
//			m_pApp->m_MotorResource.GetIoStatus(m_pApp->m_nAxes[i], &pDataIn);
			if ((DataIn >> 4) & 1) {
				m_nIconStatus[1] = 3;
				break;
			}
		}
	}

//	SetTimer(IO_CHECK_TIMER, 50,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIOTestDialog::OnPaint() 
{
	CPaintDC dc(this);

	CDC* pDC = &dc;
	CRect rect;

	for(int i=0; i<m_nNoOfSegments; i++) {
		CBrush* pBrushTemp;
		if (m_nIconStatus[i] == 0)
			pBrushTemp = &m_brushBlack;
		else if(m_nIconStatus[i] == 1)
			pBrushTemp = &m_brushYellow;
		else if (m_nIconStatus[i] == 2)
			pBrushTemp = &m_brushGreen;
		else
			pBrushTemp = &m_brushRed;

		pDC->FillRgn(&m_prgnLightSegment[i], pBrushTemp);
		pDC->FrameRgn(&m_prgnLightSegment[i], &m_brushBlack, 1, 1);
	}
}

void CIOTestDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	/*int nIndex = -1;
	CRect rect;
	for(int i=1; i<8; i++) {
		if(m_prgnLightSegment[i].PtInRegion(point)) {
			nIndex = i;
			break;
		}
	}

	if (nIndex != -1) {
		m_bChecked[nIndex] = !m_bChecked[nIndex];
		int nError = m_pApp->m_MotorResource.OutPortWriteBit(0,1, OUT_MOTOR_CONTACTOR_CONTROL,m_bChecked[nIndex]);
		if (!nError) {
			OutputDebugLogTo(9, TRUE, "Unable To Write %d Bit", OUT_MOTOR_CONTACTOR_CONTROL);
		}
		CRect rect;
		GetDlgItem(IDC_SEGMENT0 + nIndex)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		InvalidateRect(&rect);
	}*/
}

void CIOTestDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_nCounter++;

	int nCardNo = 0;
	int nPortNo = 1;
	UINT pDataIn = 0;
	bool bDataChange = false;

	//Previously we use "I_7230.dll", now we change to "M_7856.dll" that already loaded to m_MotorResource interface
	/*if (m_pApp->m_CommModule.hDLL) {
		m_pApp->m_MotorResource.InPortReadBit(nCardNo, nPortNo, IN_MOTOR_CURTAIN_SENSOR, &pDataIn);
		if (pDataIn)
			m_nIconStatus[0] = 2;
		else
			m_nIconStatus[0] = 3;

		str.Format("I/O Module: %s", m_pApp->m_strIOInterfaceCard);

		if (m_nCounter % 2 == 0)
			str = "/" + str;
		else
			str = "\\" + str;
	}
	else {
		if (m_nCounter % 2 == 0)
			m_nIconStatus[0] = 0;
		else
			m_nIconStatus[0] = 1;

		str = "No I/O module loaded";

		if (m_nCounter % 2 == 0)
			str = "/" + str;
		else
			str = "\\" + str;
	}*/

	
	//Check Emergency Stop Output Signal of	Motion Axis
	if (nIDEvent == IO_CHECK_TIMER) {
		if (m_pApp->m_MotorResource.isDllOpened()) {
			//1. Curtain Sensor
			m_pApp->m_MotorResource.InPortReadBit(nCardNo, nPortNo, IN_MOTOR_CURTAIN_SENSOR, &pDataIn);
			//OutputDebugLogTo(6, FALSE, "IN_MOTOR_CURTAIN_SENSOR: %d", pDataIn);

			if (pDataIn) {
				if (m_nIconStatus[0] != 2) {
					m_nIconStatus[0] = 2;
					bDataChange = true;
				}
			}
			else {
				if (m_nIconStatus[0] != 3) {
					m_nIconStatus[0] = 3;
					bDataChange = true;
				}
			}

			//2. Emergency Stop Output Signal of Motion Axis
			pDataIn = 0;
			long DataIn;
			m_nIconStatus[1] = 2;
			for (int i = 0; i < NO_OF_AXIS; i++) {
//				m_pApp->m_MotorResource.GetIoStatus(m_pApp->m_nAxes[i], &pDataIn);
				m_pApp->motionParameters.GetMotorIOStatus(i, DataIn);

				if ((pDataIn >> 4) & 1) {
					if (m_nIconStatus[1] != 3) {
						m_nIconStatus[1] = 3;
						bDataChange = true;
						break;
					}
				}
			}
		}
		else {
			//if (m_nCounter % 10 == 0) {
			if (m_nIconStatus[0] != 0) {
				m_nIconStatus[0] = 0;
				bDataChange = true;
			}

			if (m_nIconStatus[1] != 0) {
				m_nIconStatus[1] = 0;
				bDataChange = true;
			}
			//}
			//else {
			//	m_nIconStatus[0] = 1;
			//	m_nIconStatus[1] = 1;
			//}
		}

	//	if (m_nCounter == 99)
	//		m_nCounter = 0;
		if(bDataChange)
			Invalidate();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CIOTestDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	KillTimer(IO_CHECK_TIMER);
	CDialogEx::OnOK();
}

void CIOTestDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(IO_CHECK_TIMER);
	CDialogEx::OnClose();
}

void CIOTestDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	SetTimer(IO_CHECK_TIMER, 50, NULL);
}
