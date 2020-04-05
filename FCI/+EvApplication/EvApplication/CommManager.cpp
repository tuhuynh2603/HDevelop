/// Communication Manager
/// Udupa; Jan'11
/// Modified for 2D-Wire.. E Santosh Jul'12


#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "CommManager.h"
#include "MainFrame.h"
#include "TrackHandler.h"
#include "StatisticsView.h"
#include "ImageSelectionDlg.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"

CCommManager::CCommManager(int nStation)
{
	m_nStation = nStation;
	//m_pDoc = NULL;
	m_pApp = (CApplication*) AfxGetApp(); 
	m_pDoc = m_pApp->m_pTracks[m_nStation].m_pDoc[0];
	
	isValid			= FALSE;
	Initialize();
}

CCommManager::~CCommManager()
{
}

void CCommManager::Initialize()
{
	isValid	= m_Monitor.Create(&m_pApp->m_CommInterface, m_nStation);

	

	if(!isValid)
		OutputDebugLog(" Controller:: Failed to open port");

	AfxBeginThread(CommSendThread, this);
	/*CWinThread* pThread = AfxBeginThread(MessageParser, this);
	m_hMessageParser	= pThread->m_hThread;

	pThread				= AfxBeginThread(MessageServer, this);
	m_hMessageServer	= pThread->m_hThread;*/
}

void CCommManager::Close()
{
	m_Monitor.Close();
	isValid			= FALSE;
	/*SetEvent(m_Message.hMessageEvent);
	SetEvent(m_Message.hHandshakeEvent);*/
}


BOOL CCommManager::SendDataMessage(char nCommand,CString strMessage)
{
	char* pData;
	int i,j=1;
	char sendData[255];
	int length =  3;

	//sendData[0] = nCommand;
	/*sendData[1] = ASCII_CR;
	sendData[2] = ASCII_LF;*/
	int nLength = strMessage.GetLength();
	
	//sendData[j++] = ',';
	for(i=0;i<strMessage.GetLength()+1;i++)
	{
		sendData[i] = strMessage[i];
	}
	sendData[i-1] = ASCII_CR;

	pData = sendData;//  = sendData;

	BOOL bError = m_Monitor.SendDataMessage(pData, i);

	LogVisionMessage(strMessage, nCommand);
	
	return bError;
}


//UINT MessageParser(LPVOID pParam)
//{
//	CCommManager* pManager = (CCommManager*) pParam;
//	CCommMonitor* pMonitor  = (CCommMonitor*) &pManager->m_Monitor;
//
//	while(1) {
//		/*if(WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
//			OutputDebugLogTo(1, TRUE, "MessageParser thread closed");
//			return 0;
//		}*/
//
//		WaitForSingleObject(pMonitor->m_hMessage, INFINITE);
//
//		if(pManager->m_Monitor.m_bClosing)
//			break;
//
//		pMonitor->RemoveFromMessage();
//
//		if(pManager->m_Message.nDataLength < 1)
//			continue;
//
//		ResetEvent(pManager->m_Message.hHandshakeEvent);
//		SetEvent(pManager->m_Message.hMessageEvent);
//		WaitForSingleObject(pManager->m_Message.hHandshakeEvent, INFINITE);
//	}
//
//	return 0;
//}
//
//UINT MessageServer(LPVOID pParam)
//{
//	CCommManager* pManager = (CCommManager*) pParam;
//	while(1) {
//		WaitForSingleObject(pManager->m_Message.hMessageEvent, INFINITE);
//		if(pManager->m_Monitor.m_bClosing)
//			break;
//		 
//		pManager->ServiceMessage();
//		if(pManager->m_Monitor.m_bClosing)
//			break;
//
//		SetEvent(pManager->m_Message.hHandshakeEvent);
//	}
//	return -1;
//}


UINT CommSendThread(LPVOID pParam)
{
	CCommManager* pManager = (CCommManager*) pParam;
	while(1) {
		WaitForSingleObject(pManager->m_pApp->m_EventCommSendMsg, INFINITE);
		if(pManager->m_pApp->m_bExitApp)
			break;
		 
		pManager->SendOkToIndexMsg();
		ResetEvent(pManager->m_pApp->m_EventCommSendMsg);
	}
	return -1;
}


void CCommManager::LogMachineMessage(char Command, CString strShortMessage, CString strMessage, CString strData)
{
	CString strOutputMessage;
	strOutputMessage.Format("MACHINE::[%c]  %s %s", Command , strShortMessage, strData);
	//m_pApp->SendLogDisplay(strOutputMessage);

	OutputDebugLogTo(1, TRUE, "MACHINE->   %s    %s [%c] [%XH]", 
		strShortMessage,  
		strMessage, 
		Command,
		Command);
}


void CCommManager::LogVisionMessage(CString strShortMessage, char nCommand)
{
	CString strOutputMessage;
	strOutputMessage.Format("VISION::[%c]  %s", nCommand , strShortMessage);
	//m_pApp->SendLogDisplay(strOutputMessage);

	OutputDebugLogTo(1, TRUE, "Vision------->:   %s   [%C] [%XH]", 
					strShortMessage, 
					nCommand,
					nCommand);
}


int CCommManager::ServiceMessage()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	CString strCommEventMsg;
	CString strCommand;
	CString strMessage;
	CString strAck;
	SYSTEMTIME sysTime;
	BOOL bUpdateStats	= FALSE;
	int nError			= 0;

	CString strRecipeType	= "";
	CString strCartonType	= "";
	CString strLotNo		= "";

	int nIndex			= 1;

	INT32 nCommand = m_Message.nCommand;

	strMessage		 = "";
	if(m_Message.nDataLength > 0){
		strMessage = CString(m_Message.pData);
		strMessage = strMessage.Left(m_Message.nDataLength - 1);
		OutputDebugLogTo(1, TRUE, "Data Being Send %s", strMessage);
	}
	CAtlString resToken;
	CString respone[50];
	int curPos = 0;
	int i=0;
	do
	{
		resToken = strMessage.Tokenize(_T(","), curPos);
		resToken = resToken.Trim('\'');
		respone[i]= resToken;
		i++;
	}while (resToken != "");

	int nCommandTemp = atoi(respone[0]);
	OutputDebugLogTo(3, TRUE, "strLotNumBer = %d,respone[0] = %s",nCommandTemp,respone[0]);
	i=0;
	switch(nCommandTemp){
		case INFO:{
					/*  if(!m_pApp->m_bOnline && m_pApp->m_bLotOpened)
						  m_pApp->OnVisionOnline();*/
					  strCommEventMsg.Format("Comm: %s", strMessage);
					  m_pApp->AddLogEvent(strCommEventMsg,COMM_LOG);
					  if(/*m_pApp->m_bOnline &&*/ !m_pApp->m_bCurtainSensor && !m_pApp->m_bEmergencyStop && !m_pApp->m_bReceivedStopCmd){
							CString strStatus = "READY";
							CString strVersion = GetCurrentVersion(m_pApp->m_strAppId);
							int nCmd = INFOACK;
							CString strTemp;
							if(!m_pApp->m_strLotInfo.IsEmpty())
								strTemp.Format("%d,'%s','%s','%s','%s',%d",INFOACK,strStatus,strVersion,m_pApp->m_strLotInfo,m_pApp->m_strConfigFile,m_pApp->m_strMainCfgModeDir.Compare("Production Config")!=0?1:0);
							else
								strTemp.Format("%d,'%s','%s','NULL','%s',%d",INFOACK,strStatus,strVersion, m_pApp->m_strConfigFile, m_pApp->m_strMainCfgModeDir.Compare("Production Config") != 0 ? 1 : 0);
							SendDataMessage(nCmd,strTemp);
							strCommEventMsg.Format("Vision : %s", strTemp);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
					  }
					  else
					  {
						  CString strTemp,strTemp1;
						 /* if(!m_pApp->m_bOnline)
								strTemp = "'NOT ONLINE'";*/
						  if(m_pApp->m_bCurtainSensor)
								strTemp = "CURTAIN SENSOR TRIGGERED";
						  if(m_pApp->m_bEmergencyStop)
								strTemp = "EMERGENCY STOP TRIGGERED";
						  if (m_pApp->m_bReceivedStopCmd)
							  strTemp = "RECEIVED STOP COMMAND";

						  int nCmd = INFOACK;
						  CString strVersion = GetCurrentVersion(m_pApp->m_strAppId);
						  if (!m_pApp->m_strLotInfo.IsEmpty())
							  strTemp1.Format("%d,'%s','%s','%s','%s',%d", INFOACK, strTemp, strVersion, m_pApp->m_strLotInfo, m_pApp->m_strConfigFile, m_pApp->m_strMainCfgModeDir.Compare("Production Config") != 0 ? 1 : 0);
						  else
							  strTemp1.Format("%d,'%s','%s','NULL','%s',%d", INFOACK, strTemp, strVersion, m_pApp->m_strConfigFile, m_pApp->m_strMainCfgModeDir.Compare("Production Config") != 0 ? 1 : 0);

						  SendDataMessage(nCmd,strTemp1);
						  strCommEventMsg.Format("Vision : %s", strTemp1);
						  m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
						  OutputDebugLogTo(9, TRUE, strCommEventMsg.GetBuffer(0));
					  }
				  }
				  break;
		case START:{ // strMessage =  ",'3456789',3456789','3456789',1,1,1,'345uknn','receipe'";
						if(!m_pApp->m_bOnline)
								m_pApp->OnVisionOnline();
						strCommEventMsg.Format("Comm: %s", strMessage);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
						i=1;
						for(int j=0;j<m_nNoOfDies;j++,i++)
						{
							m_pApp->m_strPartFlex[j] = respone[i];
						}
						for(int j=0;j<m_nNoOfDies;j++,i++)
						{
							m_pApp->m_bPartInspect[j] = atoi(respone[i]);
							OutputDebugLogTo(3, TRUE, "m_pApp->m_bPartInspect[%d] = %d,respone[%d]= %s",j,m_pApp->m_bPartInspect[j],i,respone[i]);//,strLotReceipe = %s,m_pApp->m_strLotInfo = %s,m_pApp->m_strConfigFile = %s",strLotNumBer,strLotReceipe,m_pApp->m_strLotInfo,m_pApp->m_strConfigFile);
						}
						CString strLotNumBer = respone[i++];
						CString strLotReceipe = respone[i++];
						int nVerificationMode = atoi(respone[i++]);
						int nMode = atoi(respone[i++]);
						OutputDebugLogTo(6, TRUE, "strLotNumBer = %s,strLotReceipe = %s,m_pApp->m_strLotInfo = %s,m_pApp->m_strConfigFile = %s",strLotNumBer,strLotReceipe,m_pApp->m_strLotInfo,m_pApp->m_strConfigFile);
						
						if(m_pApp->m_bCurtainSensor || m_pApp->m_bEmergencyStop || m_pApp->m_bReceivedStopCmd)
						{
							CString strStatus;
							if(m_pApp->m_bCurtainSensor)
								strStatus = "CURTAIN SENSOR TRIGGERED";
							if(m_pApp->m_bEmergencyStop)
								strStatus = "EMERGENCY STOP TRIGGERED";
							if(m_pApp->m_bReceivedStopCmd)
								strStatus = "RECEIVED STOP COMMAND";
							int nCmd = STARTACK;
							CString strTemp,strTemp1;
							strTemp = m_pApp->m_strPartFlex[0];
							for(int k=1;k<m_nNoOfDies;k++)
							{
								strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
								strTemp = strTemp1;
							}
							strTemp1.Format("%d,'%s','%s','%s'",STARTACK,strTemp,strStatus,m_pApp->m_strConfigFile);
							SendDataMessage(nCmd,strTemp1);
							strCommEventMsg.Format("Vision : %s", strTemp1);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
							break;

						}

						if(!m_pApp->m_bLotOpened)
						{
							CString strStatus = strLotNumBer + "LOT NOT OPENED";
							int nCmd = STARTACK;
							CString strTemp,strTemp1;
							strTemp = m_pApp->m_strPartFlex[0];
							for(int k=1;k<m_nNoOfDies;k++)
							{
								strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
								strTemp = strTemp1;
							}
							strTemp1.Format("%d,'%s','%s','%s'",STARTACK,strTemp,strStatus,m_pApp->m_strConfigFile);
							SendDataMessage(nCmd,strTemp1);
							strCommEventMsg.Format("Vision : %s", strTemp1);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
							break;
						}
						if (m_pApp->m_strMainCfgModeDir.Compare(nMode ? "Engineering Config" : "Production Config") != 0) {
							CString strStatus = "CONFIG MODE IS INCORRECT";
							int nCmd = STARTACK;
							CString strTemp, strTemp1;
							strTemp = m_pApp->m_strPartFlex[0];
							for (int k = 1; k<m_nNoOfDies; k++)
							{
								strTemp1.Format("%s','%s", strTemp, m_pApp->m_strPartFlex[k]);
								strTemp = strTemp1;
							}
							strTemp1.Format("%d,'%s','%s','%s'", STARTACK, strTemp, strStatus, m_pApp->m_strConfigFile);
							SendDataMessage(nCmd, strTemp1);
							strCommEventMsg.Format("Vision : %s", strTemp1);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
							break;
						}
						if(strLotNumBer.Compare(m_pApp->m_strLotInfo)== 0 && strLotReceipe.Compare(m_pApp->m_strConfigFile)==0){
							CString strStatus = "TEST STARTED";
							int nCmd = STARTACK;
							CString strTemp,strTemp1;
							strTemp = m_pApp->m_strPartFlex[0];
							for(int k=1;k<m_nNoOfDies;k++)
							{
								strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
								strTemp = strTemp1;
							}
							strTemp1.Format("%d,'%s','%s','%s'",STARTACK,strTemp,strStatus,m_pApp->m_strConfigFile);

							if (m_pApp->m_bShowDieVerification) {
								m_pApp->m_bShowDieVerification = FALSE;
								((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.EnableVerification(m_pApp->m_bShowDieVerification);
							}
							m_pApp->m_nVerificationMode = nVerificationMode;
							m_pApp->m_bReceivedStartCmd = TRUE;


							SendDataMessage(nCmd,strTemp1);
							strCommEventMsg.Format("Vision : %s", strTemp1);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
						}
						else{
							CString strStatus = "RECEIPE OR LOT INFO DOES NOT MATCH";
							int nCmd = STARTACK;
							CString strTemp,strTemp1;
							strTemp = m_pApp->m_strPartFlex[0];
							for(int k=1;k<m_nNoOfDies;k++)
							{
								strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
								strTemp = strTemp1;
							}
							strTemp1.Format("%d,'%s','%s','%s'",STARTACK,strTemp,strStatus,m_pApp->m_strConfigFile);
							SendDataMessage(nCmd,strTemp1);
							strCommEventMsg.Format("Vision : %s", strTemp1);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
						}
					}
					break;
		case RESULT:{

						if(!m_pApp->m_bOnline)
							m_pApp->OnVisionOnline();

						strCommEventMsg.Format("Comm: %s", strMessage);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

						int nCmd = RESULTACK;
						CString strTemp,strTemp1,strTemp2,strTemp3;
						strTemp = m_pApp->m_strPartFlex[0];
						for(int k=1;k<m_nNoOfDies;k++)
						{
							strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
							strTemp = strTemp1;
						}

						int nErrorDefectCode = m_pApp->m_pImageSelectionDlg->m_wndDlg.getErrorDefectCode(m_pApp->m_nPartErrorCode[0]);
						strTemp3.Format("%d", nErrorDefectCode);
						for(int k=1;k<m_nNoOfDies;k++)
						{
							int nDieErrorDefectCode = m_pApp->m_pImageSelectionDlg->m_wndDlg.getErrorDefectCode(m_pApp->m_nPartErrorCode[k]);
							strTemp2.Format("%s,%d",strTemp3, nDieErrorDefectCode);
							strTemp3 = strTemp2;
						}
						strTemp.Format("%d,'%s',%s,'%s','%s'",RESULTACK,strTemp,strTemp3,m_pApp->m_strLotInfo,m_pApp->m_strConfigFile);
						SendDataMessage(nCmd,strTemp);
						strCommEventMsg.Format("Vision : %s", strTemp);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
					}
					break;
		case STARTLOT:{
						strCommEventMsg.Format("Comm: %s", strMessage);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

						i=1;
						CString strLotReceipe = respone[i++];
						CString strLotNumBer = respone[i++];
						int nMode = atoi(respone[i++]);

						if(m_pApp->m_bCurtainSensor || m_pApp->m_bEmergencyStop || m_pApp->m_bReceivedStopCmd)
						{
							CString strStatus,strTemp;
							if(m_pApp->m_bCurtainSensor)
								strStatus = "'CURTAIN SENSOR TRIGGERED'";
							if(m_pApp->m_bEmergencyStop)
								strStatus = "'EMERGENCY STOP TRIGGERED'";
							if(m_pApp->m_bReceivedStopCmd)
								strStatus = "'RECEIVED STOP COMMAND'";
							int nCmd = STARTLOTACK;
							strTemp.Format("%d,'%s','%s','%s'",STARTLOTACK,strLotReceipe,strLotNumBer,strStatus);
							SendDataMessage(nCmd,strTemp);
							strCommEventMsg.Format("Vision : %s", strTemp);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
							break;

						}
						HANDLE hFile;
						WIN32_FIND_DATA FindFileData;
						BOOL bFound = FALSE;
						CString strTempConfig;
						if (!nMode)
							m_pApp->m_strMainCfgModeDir = "Production Config";
						else
							m_pApp->m_strMainCfgModeDir = "Engineering Config";

						strTempConfig.Format("%s\\%s\\*.",m_pApp->m_strMainCfgDir,m_pApp->m_strMainCfgModeDir);
						if((hFile=FindFirstFile(strTempConfig, &FindFileData))!=INVALID_HANDLE_VALUE) {
							do {
								if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
									CString str = FindFileData.cFileName;
									if(str.Find(".")==-1 && str.CompareNoCase("_Common")!=0) {
										if(strLotReceipe == CString(FindFileData.cFileName)){
											m_pApp->m_strConfigFile = strLotReceipe;
											m_pApp->m_strLotConfigFile = strLotReceipe;
											bFound = TRUE;
											break;
										}
									}
								}
							} while (FindNextFile(hFile, &FindFileData));
						}
						FindClose(hFile);

						OutputDebugLogTo(3, TRUE, "strLotNumBer = %s,strLotReceipe = %s,m_pApp->m_strLotInfo = %s,m_pApp->m_strConfigFile = %s",strLotNumBer,strLotReceipe,m_pApp->m_strLotInfo,m_pApp->m_strConfigFile);

						if(bFound && !m_pApp->m_bLotOpened){
							
							GetLocalTime(&sysTime);
							//m_pApp->m_pStatsView->SaveLotSummary();
							LoadRecipe();
							
							m_pApp->m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_RECEIPE_STATUS_STATIC)->SetWindowTextA(m_pApp->m_strLotConfigFile);
							PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info Receipe", m_pApp->m_strLotConfigFile);
							PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Recipe", m_pApp->m_strConfigFile);

							///m_pApp->m_pStatsView->RecordLotStats(TRUE);
							m_pApp->m_LotSummary.strStartLotTime.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
							((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.m_bWriteFirstLot = TRUE;
							m_pApp->m_strLotInfo = strLotNumBer;
							m_pApp->m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_LOT_ID_STATIC)->SetWindowTextA(m_pApp->m_strLotInfo);
							m_pApp->m_bLotOpened = TRUE;
							CString strTemp;
							CString strStatus = "SUCCESS";
							int nCmd = STARTLOTACK;
							strTemp.Format("%d,'%s','%s','%s'",STARTLOTACK,m_pApp->m_strConfigFile,m_pApp->m_strLotInfo,strStatus);
							SendDataMessage(nCmd,strTemp);
							strCommEventMsg.Format("Vision : %s", strTemp);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
							PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Opened", m_pApp->m_bLotOpened);
							PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info", m_pApp->m_strLotInfo);

							//Check Free Disk Space For Saving Image Every Time Start New Lot
							m_pApp->CheckDiskFreeSpace();
							m_pApp->CheckDLDiskFreeSpace();
						}
						else
						{
							CString strTemp;
							CString strStatus;
							if(!bFound)
								strStatus = "RECEIPE NOT FOUND";
							if(m_pApp->m_bLotOpened)
								strStatus = "CURRENT LOT IS NOT STOPPED";
							int nCmd = STARTLOTACK;
							strTemp.Format("%d,'%s','%s','%s'",STARTLOTACK,m_pApp->m_strConfigFile,m_pApp->m_strLotInfo,strStatus);
							SendDataMessage(nCmd,strTemp);
							strCommEventMsg.Format("Vision : %s", strTemp);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
						}
						
						//// Place at the last in order to load lot info if lot num found
						if (!m_pApp->m_bOnline)
							m_pApp->OnVisionOnline();

					  }
					  break;
		case STOPLOT:{
						if(!m_pApp->m_bOnline)
							  m_pApp->OnVisionOnline();

						strCommEventMsg.Format("Comm: %s", strMessage);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

						i=1;
						CString strLotReceipe = respone[i++];
						CString strLotNumBer = respone[i++];
						if(m_pApp->m_bLotOpened){
							if(strLotNumBer.Compare(m_pApp->m_strLotInfo)== 0 && strLotReceipe.Compare(m_pApp->m_strLotConfigFile)==0){
								GetLocalTime(&sysTime);
								m_pApp->m_LotSummary.strEndTimeDay.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
								((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateLotInfo();
								((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateTotalLotInfo();
								((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.m_bWriteFirstLot = FALSE;
								//m_pApp->RecordLotStats();
								m_pApp->m_bLotOpened = FALSE;
								m_pApp->m_strLotInfo = "";
								m_pApp->m_strLotConfigFile = "";
								m_pApp->m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_LOT_ID_STATIC)->SetWindowTextA(m_pApp->m_strLotInfo);
								m_pApp->m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_RECEIPE_STATUS_STATIC)->SetWindowTextA(m_pApp->m_strLotConfigFile);
								PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info Receipe", m_pApp->m_strLotConfigFile);
								CString strTemp;
								CString strStatus = "SUCCESS";
								int nCmd = STOPLOTACK;
								strTemp.Format("%d,'%s','%s','%s'",STOPLOTACK,m_pApp->m_strConfigFile,strLotNumBer,strStatus);
								SendDataMessage(nCmd,strTemp);
								strCommEventMsg.Format("Vision : %s", strTemp);
								m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
								PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Opened", m_pApp->m_bLotOpened);
								PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info", m_pApp->m_strLotInfo);
								m_pApp->m_LotSummary.InitDisplaySummary();
							}
							else
							{
								CString strTemp;
								CString strStatus = "RECEIPE OR LOT INFO NOT MATCH";
								int nCmd = STOPLOTACK;
								strTemp.Format("%d,'%s','%s','%s'",STOPLOTACK,m_pApp->m_strConfigFile,strLotNumBer,strStatus);
								SendDataMessage(nCmd,strTemp);
								strCommEventMsg.Format("Vision : %s", strTemp);
								m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
							}
						}
						else
						{
							CString strTemp;
							CString strStatus = "LOT NOT OPENED";
							int nCmd = STOPLOTACK;
							strTemp.Format("%d,'%s','%s','%s'",STOPLOTACK,m_pApp->m_strConfigFile,strLotNumBer,strStatus);
							SendDataMessage(nCmd,strTemp);
							strCommEventMsg.Format("Vision : %s", strTemp);
							m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
						}

					}
					break;
		case RESET:{
						if(!m_pApp->m_bOnline)
							 m_pApp->OnVisionOnline();
						strCommEventMsg.Format("Comm: %s", strMessage);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

						if(m_pApp->m_bOnline)
							 m_pApp->m_bReceivedResetCmd = TRUE;

						CString strTemp;
						CString strStatus = "";
						int nError = m_pApp->OnReceiveResetCmd();
						if (nError == -1)
							strStatus = "EMERGENCY STOP TRIGGERED";
						else if (nError == -2)
							strStatus = "CURTAIN SENSOR TRIGGERED";
						else
							strStatus = "READY";

						int nCmd = RESETACK;
						strTemp.Format("%d,'%s'",RESETACK,strStatus);
						SendDataMessage(nCmd,strTemp);
						strCommEventMsg.Format("Vision : %s", strTemp);
						m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
				   }
				   break;
		case STOP:{
					strCommEventMsg.Format("Comm: %s", strMessage);
					m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
					if (m_pApp->m_bOnline) {
						m_pApp->m_bReceivedStopCmd = TRUE;
						theApp.motionParameters.StopEveryAxis();
					}

				  }
				  break;
		case WARNING: {
			strCommEventMsg.Format("Comm: %s", strMessage);
			m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
			i = 1;

			CString strStatus;
			if (respone[i].GetLength() > 0) {
				//If we're using Centralized Reject PC
				if (m_pApp->m_bCentralizedVerificationMode && !m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
					if (m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.m_bConnected) {
						int nError = m_pApp->m_CentralizedCommInterface.m_pCommManager[0]->SendWarningMessage(respone[i]);
						if (nError)
							strStatus = "READY";
						else
							strStatus = "FAILED TO SEND WARNING MESSAGE TO REJECT PC";
					}
					else {
						strStatus = "DISCONNECTED WITH REJECT PC";
					}
				}
				else {
					if (m_pApp->pErrorMsgDlg[0] != NULL) {
						m_pApp->pErrorMsgDlg[0]->PostModuleStatus(true, respone[i]);
						strStatus = "READY";
					}
					else {
						m_pApp->pErrorMsgDlg[0] = new CErrorDlg();
						if (!m_pApp->pErrorMsgDlg[0]->Create(IDD_ERRORDLG, AfxGetMainWnd())) {
							delete m_pApp->pErrorMsgDlg[0];
							m_pApp->pErrorMsgDlg[0] = NULL;
							strStatus = "FAILED";
						}
						else {
							m_pApp->pErrorMsgDlg[0]->PostModuleStatus(true, respone[i]);
							strStatus = "READY";
						}
					}
				}
				
			}
			else {
				strStatus = "EMPTY MESSAGE";
			}

			int nCmd = WARNINGACK;
			CString strMsg;
			strMsg.Format("%d,'%s'", nCmd, strStatus);

			SendDataMessage(nCmd, strMsg);
			strCommEventMsg.Format("Vision : %s", strMsg);
			m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
		}
		break;
	}
	return 0;
}

int CCommManager::SetStation(int nTrack)
{
	m_nStation	= nTrack - 1;
	if(m_nStation < 0)
		m_nStation = 0;

	//m_nStation = 0; ///Always set to zero for 1 Station....
	m_pDoc = m_pApp->m_pTracks[m_nStation].m_pDoc[0];
	return 0;
}


void CCommManager::LoadRecipe()
{
	m_pApp->m_pMainWnd->SendMessage(WM_LOAD_RECIPE);
	OutputDebugLogTo(0, TRUE, "Loading Recipe from Controller");
}


void CCommManager::SendOkToIndexMsg()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;


	CString strCommEventMsg;
	CString strStatus = "READY";
	int nCmd = OKTOINDEX;
	CString strTemp,strTemp1;
	strTemp = m_pApp->m_strPartFlex[0];
	for(int k=1;k<m_nNoOfDies;k++)
	{
		strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
		strTemp = strTemp1;
	}
	strTemp.Format("%d,'%s','%s'",OKTOINDEX,strTemp,strStatus);
	SendDataMessage(nCmd,strTemp);
	strCommEventMsg.Format("Vision : %s", strTemp);
	m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
}
