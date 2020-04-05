// CentralizedMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CentralizedMonitorDlg.h"
#include "afxdialogex.h"
#include "Application.h"
#include "CentralizedCommMonitor.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommInterface.h"

static const UINT_PTR CENTRALIZED_CHECK_TIMER = 120;

// CCentralizedMonitorDlg dialog

IMPLEMENT_DYNAMIC(CCentralizedMonitorDlg, CDialogEx)

CCentralizedMonitorDlg::CCentralizedMonitorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CENTRALIZED_MONITOR_DIALOG, pParent)
{
	m_pApp = (CApplication*)AfxGetApp();

	m_brushGreen.CreateSolidBrush(RGB(50, 250, 50));
	m_brushRed.CreateSolidBrush(RGB(220, 50, 50));
	m_brushBlack.CreateSolidBrush(RGB(0, 0, 0));
	m_brushYellow.CreateSolidBrush(RGB(255, 255, 255));
}

CCentralizedMonitorDlg::~CCentralizedMonitorDlg()
{
	m_brushGreen.DeleteObject();
	m_brushRed.DeleteObject();
	m_brushBlack.DeleteObject();
	m_brushYellow.DeleteObject();
}

void CCentralizedMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCentralizedMonitorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_CLIENT0, &CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient0)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_CLIENT1, &CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient1)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_CLIENT2, &CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient2)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_CLIENT3, &CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient3)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_CLIENT4, &CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient4)
END_MESSAGE_MAP()


// CCentralizedMonitorDlg message handlers
BOOL CCentralizedMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	for (int i = 0; i < MAX_CLIENT_MONITORING; i++) {
		m_nIconStatus[i] = 0;
		GetDlgItem(IDC_CLIENT0 + i)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_prgnClientSegment[i].CreateEllipticRgn(rect.left - 2, rect.top - 2, rect.right + 2, rect.bottom + 2);

		GetDlgItem(IDC_STATIC_CLIENT0 + i)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DISCONNECT_CLIENT0 + i)->ShowWindow(FALSE);
	}
	
	int clientIndex = 0;

	//Centralized Reject PC UI
	if (m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {

		for (int iPort = 0; iPort < NO_OF_PORT_CENTRALIZED; iPort++, clientIndex++) {
			if (m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort] != NULL) {

				BOOL bClientAcceptedStatus;
				CString strClientMachineID, strClientMachineIP;
				m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.GetClientAcceptedInfo(strClientMachineID, strClientMachineIP, bClientAcceptedStatus);

				if (strClientMachineIP != "") {
					CString strClientInfo;
					strClientInfo.Format("IPv4: %s - Machine ID: %s", strClientMachineIP, strClientMachineID);
					GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->SetWindowTextA(strClientInfo);
					GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->ShowWindow(TRUE);
					if (bClientAcceptedStatus) {
						m_nIconStatus[clientIndex] = 2;
						GetDlgItem(IDC_BUTTON_DISCONNECT_CLIENT0 + clientIndex)->ShowWindow(TRUE);
					}

					m_ClientAcceptedMap.insert(std::pair<CString, BOOL>(strClientMachineIP, bClientAcceptedStatus));
					m_ClientAcceptedMachineId.insert(std::pair<CString, CString>(strClientMachineIP, strClientMachineID));
				}

			}
		}
	}
	//Client Machine PC UI
	else {
		int iPort = 0;
		if (m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort] != NULL) {
			CString strClientInfo;
			strClientInfo.Format("IPv4: %s - Centralized Reject PC", m_pApp->m_CentralizedCommInterface.m_strIpAddress);
			GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->SetWindowTextA(strClientInfo);
			GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->ShowWindow(TRUE);
			if (m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.m_bConnected) {
				m_nIconStatus[clientIndex] = 2;
			}

			m_ClientAcceptedMap.insert(std::pair<CString, BOOL>(m_pApp->m_CentralizedCommInterface.m_strIpAddress, m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.m_bConnected));
			m_ClientAcceptedMachineId.insert(std::pair<CString, CString>(m_pApp->m_CentralizedCommInterface.m_strIpAddress, "Centralized Reject PC"));
		}
	}
	

	//	SetTimer(IO_CHECK_TIMER, 50,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CCentralizedMonitorDlg::OnPaint()
{
	CPaintDC dc(this);

	CDC* pDC = &dc;
	CRect rect;

	int clientIndex = 0;
	for (int iPort = 0; iPort < NO_OF_PORT_CENTRALIZED; iPort++, clientIndex++) {
		if (m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort] != NULL) {

			BOOL bClientAcceptedStatus;
			CString strClientMachineID, strClientMachineIP;
			m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.GetClientAcceptedInfo(strClientMachineID, strClientMachineIP, bClientAcceptedStatus);

			if (strClientMachineIP != "") {
				CBrush* pBrushTemp;
				if (m_nIconStatus[clientIndex] == 0)
					pBrushTemp = &m_brushBlack;
				else if (m_nIconStatus[clientIndex] == 1)
					pBrushTemp = &m_brushYellow;
				else if (m_nIconStatus[clientIndex] == 2)
					pBrushTemp = &m_brushGreen;
				else
					pBrushTemp = &m_brushRed;

				pDC->FillRgn(&m_prgnClientSegment[clientIndex], pBrushTemp);
				pDC->FrameRgn(&m_prgnClientSegment[clientIndex], &m_brushBlack, 1, 1);
			}

		}
	}

	/*int nMinClient = m_ClientAcceptedMap.size() <= MAX_CLIENT_MONITORING ? m_ClientAcceptedMap.size() : MAX_CLIENT_MONITORING;
	for (int i = 0; i < nMinClient; i++) {
		CBrush* pBrushTemp;
		if (m_nIconStatus[i] == 0)
			pBrushTemp = &m_brushBlack;
		else if (m_nIconStatus[i] == 1)
			pBrushTemp = &m_brushYellow;
		else if (m_nIconStatus[i] == 2)
			pBrushTemp = &m_brushGreen;
		else
			pBrushTemp = &m_brushRed;

		pDC->FillRgn(&m_prgnClientSegment[i], pBrushTemp);
		pDC->FrameRgn(&m_prgnClientSegment[i], &m_brushBlack, 1, 1);
	}*/
}

void CCentralizedMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	bool bDataChange = false;

	//Check Emergency Stop Output Signal of	Motion Axis
	if (nIDEvent == CENTRALIZED_CHECK_TIMER) {
		int clientIndex = 0;
		//Centralized Reject PC UI
		if (m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
			int nClientCountPrev = m_ClientAcceptedMap.size();
			int nClientCountCurrent = 0;
			
			for (int iPort = 0; iPort < NO_OF_PORT_CENTRALIZED; iPort++, clientIndex++) {
				if (m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort] == NULL)
					continue;

				BOOL bClientAcceptedStatus;
				CString strClientMachineID, strClientMachineIP;

				m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.GetClientAcceptedInfo(strClientMachineID, strClientMachineIP, bClientAcceptedStatus);

				if (strClientMachineIP != "") {
					nClientCountCurrent++;

					CString strClientInfo;
					strClientInfo.Format("IP: %s - Machine ID: %s", strClientMachineIP, strClientMachineID);
					GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->SetWindowTextA(strClientInfo);
					GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->ShowWindow(TRUE);
					if (bClientAcceptedStatus) {
						if (m_nIconStatus[clientIndex] != 2)
							bDataChange = true;
						m_nIconStatus[clientIndex] = 2;
						GetDlgItem(IDC_BUTTON_DISCONNECT_CLIENT0 + clientIndex)->ShowWindow(TRUE);
					}
					else {
						if (m_nIconStatus[clientIndex] == 2)
							bDataChange = true;
						m_nIconStatus[clientIndex] = 0;
						GetDlgItem(IDC_BUTTON_DISCONNECT_CLIENT0 + clientIndex)->ShowWindow(FALSE);
					}

					m_ClientAcceptedMap.insert(std::pair<CString, BOOL>(strClientMachineIP, bClientAcceptedStatus));
					m_ClientAcceptedMachineId.insert(std::pair<CString, CString>(strClientMachineIP, strClientMachineID));
				}

			}
			if (nClientCountPrev != nClientCountCurrent)
				bDataChange = true;
		}
		//Client Machine PC UI
		else {
			int iPort = 0;
			
			CString strClientInfo;
			strClientInfo.Format("IP: %s - Centralized Reject PC", m_pApp->m_CentralizedCommInterface.m_strIpAddress);
			GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->SetWindowTextA(strClientInfo);
			GetDlgItem(IDC_STATIC_CLIENT0 + clientIndex)->ShowWindow(TRUE);
			if (m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.m_bConnected) {
				if (m_nIconStatus[clientIndex] != 2)
					bDataChange = true;
				m_nIconStatus[clientIndex] = 2;
			}
			else {
				if (m_nIconStatus[clientIndex] == 2)
					bDataChange = true;
				m_nIconStatus[clientIndex] = 0;
			}

			m_ClientAcceptedMap.insert(std::pair<CString, BOOL>(m_pApp->m_CentralizedCommInterface.m_strIpAddress, m_pApp->m_CentralizedCommInterface.m_pCommManager[iPort]->m_Monitor.m_bConnected));
			m_ClientAcceptedMachineId.insert(std::pair<CString, CString>(m_pApp->m_CentralizedCommInterface.m_strIpAddress, "Centralized Reject PC"));
		}
		

		if (bDataChange)
			Invalidate();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CCentralizedMonitorDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	KillTimer(CENTRALIZED_CHECK_TIMER);
	CDialogEx::OnOK();
}

void CCentralizedMonitorDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(CENTRALIZED_CHECK_TIMER);
	CDialogEx::OnClose();
}

void CCentralizedMonitorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	SetTimer(CENTRALIZED_CHECK_TIMER, 50, NULL);
}

void CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient0()
{
	// TODO: Add your control notification handler code here
	//m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.DisconnectSocket();
	m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(DISCONNECT, "DISCONNECT");
}


void CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient1()
{
	// TODO: Add your control notification handler code here
	//m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.DisconnectSocket();
	m_pApp->m_CentralizedCommInterface.m_pCommManager[1]->SendDataMessage(DISCONNECT, "DISCONNECT");
}


void CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient2()
{
	// TODO: Add your control notification handler code here
	//m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.DisconnectSocket();
	m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(DISCONNECT, "DISCONNECT");
}


void CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient3()
{
	// TODO: Add your control notification handler code here
	//m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.DisconnectSocket();
	m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(DISCONNECT, "DISCONNECT");
}


void CCentralizedMonitorDlg::OnBnClickedButtonDisconnectClient4()
{
	// TODO: Add your control notification handler code here
	//m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.DisconnectSocket();
	m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(DISCONNECT, "DISCONNECT");
}
