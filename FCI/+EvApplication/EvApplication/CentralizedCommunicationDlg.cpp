// CentralizedCommunicationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CentralizedCommunicationDlg.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"
#include "afxdialogex.h"

#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"

// CentralizedCommunicationDlg dialog

IMPLEMENT_DYNAMIC(CentralizedCommunicationDlg, CDialogEx)

CentralizedCommunicationDlg::CentralizedCommunicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CENTRALIZED_COMM_DLG, pParent)
	, m_strMessage(_T(""))
	, m_strImagePath(_T(""))
{

}

CentralizedCommunicationDlg::~CentralizedCommunicationDlg()
{
}

void CentralizedCommunicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MESSAGE_TO_SEND, m_strMessage);
	DDX_Text(pDX, IDC_IMAGE_PATH_TO_SEND, m_strImagePath);
}


BEGIN_MESSAGE_MAP(CentralizedCommunicationDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CentralizedCommunicationDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SEND_IMAGE, &CentralizedCommunicationDlg::OnBnClickedButtonSendImage)
	ON_BN_CLICKED(IDC_BUTTON_SEND_ALL_IMAGE, &CentralizedCommunicationDlg::OnBnClickedButtonSendAllImage)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MAPPING_RESULT, &CentralizedCommunicationDlg::OnBnClickedButtonSendMappingResult)
	ON_BN_CLICKED(IDC_BUTTON_SEND2, &CentralizedCommunicationDlg::OnBnClickedButtonSend2)
END_MESSAGE_MAP()


// CentralizedCommunicationDlg message handlers


void CentralizedCommunicationDlg::OnBnClickedButtonSend()
{
	UpdateData(TRUE);
	theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(SEND_MESSAGE, m_strMessage.GetString());
}


void CentralizedCommunicationDlg::OnBnClickedButtonSendImage()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	int nTrack, nDoc, nDevice, nFov;
	nTrack = nDoc = nDevice = nFov = 0;

	CImageBuffer imageBufferSend;
	LoadGrayScaleBMPFile(m_strImagePath, imageBufferSend);
	theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendImageBufferMessage(0, 0, 0, 0, imageBufferSend);
}


void CentralizedCommunicationDlg::OnBnClickedButtonSendAllImage()
{
	// TODO: Add your control notification handler code here
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int m_nNoOfFov = nFovPerX * nFovPerY;
	CImageBuffer imageBufferSend;
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < theApp.m_nTotalViewDocs[nTrack]; nDoc++)
				{
					theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendImageBufferMessage(nTrack, nDoc, nDie, nFov,
																			theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov]);
				}
			}
		}
	}
}


void CentralizedCommunicationDlg::OnBnClickedButtonSendMappingResult()
{
	// TODO: Add your control notification handler code here
	theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendMappingResult(MAPPING_RESULT);
}


void CentralizedCommunicationDlg::OnBnClickedButtonSend2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendWarningMessage(m_strMessage.GetString());
}
