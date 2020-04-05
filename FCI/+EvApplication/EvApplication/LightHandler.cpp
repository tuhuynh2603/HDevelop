/// Light Handler: Controls all light controllers within one station
/// Udupa; Sept'2012


#include "StdAfx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "LightHandler.h"
#include "GardaLightCtrl.h"


CLightHandler::CLightHandler()
{
	m_nControllerCount = 0;
	m_pController = NULL;
	m_nImageCount = 0;//2;

	m_nControllerSegCount = 0;
	m_pstrSerialNos = NULL;
	m_nSegmentsPerTrack = 0;

	m_pMaxCurrents = NULL;
	//m_nMaxPulseWidth = 1000;
	m_pMaxPulseWidth = NULL;
}

CLightHandler::~CLightHandler()
{
	if(m_pController != NULL)
		delete[] m_pController;

	if(m_pstrSerialNos != NULL)
		delete[] m_pstrSerialNos;

	if(m_pMaxCurrents != NULL)
		delete[] m_pMaxCurrents;
}

void CLightHandler::SetParameters(CTrackHandler* pTracks, int nTracks)
{
	m_pApp = (CApplication*) AfxGetApp();
	CString strSec, str;

	strSec.Format("a. Controller Segment Count");
	m_nControllerSegCount =  GetRegistryEntry(READ_REGPATH + "Light", strSec, -1);
	if(m_nControllerSegCount==-1) {
		m_nControllerSegCount = 8;//3;
		PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_nControllerSegCount);
	}

	strSec.Format("b. Segments Per Track");
	m_nSegmentsPerTrack =  GetRegistryEntry(READ_REGPATH + "Light", strSec, -1);
	if(m_nSegmentsPerTrack==-1) {
		m_nSegmentsPerTrack = 8;
		PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_nSegmentsPerTrack);
	}

	strSec.Format("Share All Channels Between Tracks: Enable");
	m_bShareAllChannel =  GetRegistryEntry(READ_REGPATH + "Light", strSec, -1);
	if(m_bShareAllChannel==-1) {
		m_bShareAllChannel = 1;
		PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_bShareAllChannel);
	}

	m_nControllerCount = m_bShareAllChannel ? (m_nSegmentsPerTrack + m_nControllerSegCount - 1) / m_nControllerSegCount : (nTracks * m_nSegmentsPerTrack + (m_nControllerSegCount-1)) / m_nControllerSegCount;
	m_pController = new CGardaLightCtrl[m_nControllerCount];

	m_pstrSerialNos = new CString[m_nControllerCount];
	for(int i=0; i<m_nControllerCount; i++) {
		strSec.Format("c. Serial No. Controller %d", i+1);
		str = GetRegistryEntry(READ_REGPATH + "Light", strSec, "");
		if (str=="") {
			m_pstrSerialNos[i].Format("%d", 272878+i);
			PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_pstrSerialNos[i]);
		}
		else
			m_pstrSerialNos[i] = str;
	}


 	//m_pMaxCurrents = new float[m_nControllerSegCount];
 	m_pMaxCurrents = new float[m_nSegmentsPerTrack];
	CString strMax;
	//int temp= 0;
	for(int j=0; j<m_nSegmentsPerTrack; j++) {	
		strSec.Format("d. Max Current Seg %d (A)", j+1);
		strMax =  GetRegistryEntry(READ_REGPATH + "Light", strSec, "");
		if(strMax.IsEmpty()) {
			strMax.Format("%f", 1.0);
			PutRegistryEntry(WRITE_REGPATH + "Light", strSec, strMax);
		}
		sscanf_s(strMax, "%f", &m_pMaxCurrents[/*temp*/j]);
		//m_pMaxCurrents[i] = atof(strMax);
		//OutputDebugLogTo(0,TRUE,"%.2f", m_pMaxCurrents[j]);
	}
	//for(int k=m_nSegmentsPerTrack, n=0; k<m_nControllerSegCount; k++, n++)
	//	m_pMaxCurrents[k] = m_pMaxCurrents[n%m_nSegmentsPerTrack];

	/*strSec.Format("e. Max Pulse Width (us)");
	m_nMaxPulseWidth =  GetRegistryEntry(READ_REGPATH + "Light", strSec, -1);
	if(m_nMaxPulseWidth==-1) {
		m_nMaxPulseWidth = 1000;
		PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_nMaxPulseWidth);
	}*/

	m_pMaxPulseWidth = new int[m_nSegmentsPerTrack];
	int temp= 0;
	for(int x=0; x<m_nSegmentsPerTrack; x++) {
		strSec.Format("e. Max Pulse Width %d (us)", temp+1);
		m_pMaxPulseWidth[x] =  GetRegistryEntry(READ_REGPATH + "Light", strSec, -1);
		if(m_pMaxPulseWidth[x] == -1) {
			m_pMaxPulseWidth[x] = 1000;
			PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_pMaxPulseWidth[temp]);
		}
	}

	//CString strTempName[] = {"(T1)Top coaxial",
	//						"(T2)Top ring (big and small rings)",
	//						"(T3)Bottom coaxial", "(T4)Bottom coaxial", "(T5) ",
	//						"(B1)Bottom ring (big and small rings)", "(B2) ",
	//						"(B3)Top coaxial & Bottom small ring",
	//						"(B4)Top coaxial & Bottom small ring", "(B5) ",
	//						"(S1)Side light",
	//						"(S2)Side light"};

	CString strTempName[8];
	int nTrackIndex[8];
	int nDocViewIndex[8];

	int nViews = 0;
	CString strTemp;

	for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_pApp->m_pTracks[nTrack].m_nTotalDocs; nDoc++, nViews++) {
			if (nTrack == STATION_TOP) {
		//		strTemp.Format("T%d", nDoc);
				strTempName[nViews] = m_pApp->m_strTopCameraName[nDoc];
			}
			else if (nTrack == STATION_BOTTOM) {
				strTempName[nViews] = m_pApp->m_strBottomCameraName[nDoc];
			}
			nTrackIndex[nViews] = nTrack;
			nDocViewIndex[nViews] = nDoc;
		}
	}

	CString strTemp1, strTemp2, strTemp3, strTemp4, strTemp5, strSec1;
	for(int y=0; y< m_pApp->m_nTotalViews; y++) {
		strSec.Format("g. %s: Bits", strTempName[y]);
		strTemp1 = GetRegistryEntry(READ_REGPATH + "Light", strSec, "");
		if(strTemp1.IsEmpty()) {
			strTemp1.Format("-1");
			PutRegistryEntry(WRITE_REGPATH + "Light", strSec, strTemp1);
		}
		int nCount = 1;
		for(int nChar=0; nChar<strTemp1.GetLength(); nChar++)
			if(strTemp1[nChar]==';')
				nCount++;
		
		SortingElements(strTemp1, nCount, strTemp3);

//		int nTrack = y<MAX_TRACKS ? 0 : (y >= (MAX_TRACKS * 2) ? 2 : 1);
//		int nView = y%MAX_TRACKS;
		int nTrack = nTrackIndex[y];
		int nView = nDocViewIndex[y];
		int nTokenPos = 0;
		CString strToken;
		for(int z=0; z<nCount; z++) {
			//strToken = (nTokenPos >= 0) ? strTemp1.Tokenize(";", nTokenPos) : "";
			strToken = (nTokenPos >= 0) ? strTemp3.Tokenize(";", nTokenPos) : "";
			if(strToken.IsEmpty())
				strToken.Format("0");
			m_pApp->m_pTracks[nTrack].m_nSegmentsOfImage[nView].Add(atoi(strToken));
		}

		strTemp2.Format("f. %s: No. of Segments", strTempName[y]);
		m_nLightNoOfSegments[y] = GetRegistryEntry(READ_REGPATH + "Light", strTemp2, -1);
		if(m_nLightNoOfSegments[y]==-1)
			m_nLightNoOfSegments[y] = 0;
		PutRegistryEntry(WRITE_REGPATH + "Light", strTemp2, m_nLightNoOfSegments[y]);

		// JY: for selected light channel
		strSec1.Format("h. %s: Active Bits", strTempName[y]);
		strTemp4 = GetRegistryEntry(READ_REGPATH + "Light", strSec1, "");
		if (strTemp4.IsEmpty()) {
			strTemp4.Format("-1");
			PutRegistryEntry(WRITE_REGPATH + "Light", strSec1, strTemp4);
		}
		int nCount1 = 1;
		for (int nChar = 0; nChar<strTemp4.GetLength(); nChar++)
			if (strTemp4[nChar] == ';')
				nCount1++;

		SortingElements(strTemp4, nCount1, strTemp5);

//		int nTrack1 = y<MAX_TRACKS /*? 0 : (y >= (MAX_TRACKS * 2) ? 2 : 1)*/;
//		int nView1 = y%MAX_TRACKS;
		int nTokenPos1 = 0;
		CString strToken1;
		for (int z = 0; z<nCount1; z++) {
			strToken1 = (nTokenPos1 >= 0) ? strTemp5.Tokenize(";", nTokenPos1) : "";
			if (strToken1.IsEmpty())
				strToken1.Format("0");
			m_pApp->m_pTracks[nTrack].m_nSelectedSegmentsOfImage[nView].Add(atoi(strToken1));
		}
	}

	//for(int y=0; y</*m_pApp->m_nTotalViews*/MAX_DOCS; y++)
	//{
	//	//strSec.Format("f. Start Bit of Image %d ", i+1);
	//	strSec.Format("f. Start Bit of %s%d ", (y<MAX_TRACKS?"T":(y>=(MAX_TRACKS*2)?"S":"B")), (y%MAX_TRACKS)+1);	
	//	m_pApp->m_nStartSegmentsOfImage.Add(GetRegistryEntry(READ_REGPATH + "Light", strSec, -1));
	//	if(m_pApp->m_nStartSegmentsOfImage[y] == -1)
	//	{ 
	//		/*if(y==0)
	//		{
	//			m_pApp->m_nStartSegmentsOfImage.RemoveAt(0);
	//			int x = m_pApp->m_nStartSegmentsOfImage.Add(0);
	//		}
	//		else if(y==1)
	//		{
	//			m_pApp->m_nStartSegmentsOfImage.RemoveAt(1);
	//			int x = m_pApp->m_nStartSegmentsOfImage.Add(2);
	//		}
	//		else if(y==2)
	//		{
	//			m_pApp->m_nStartSegmentsOfImage.RemoveAt(2);
	//			m_pApp->m_nStartSegmentsOfImage.Add(3);
	//		}*/
	//		m_pApp->m_nStartSegmentsOfImage[y]=0;
	//		PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_pApp->m_nStartSegmentsOfImage[y]);
	//	}
	//	CString strTemp;
	//	//strTemp.Format("g.Image %d :No. Of Segments", i+1);
	//	strTemp.Format("g. %s%d :No. Of Segments", (y<MAX_TRACKS?"T":(y>=(MAX_TRACKS*2)?"S":"B")), (y%MAX_TRACKS)+1);
	//	m_nLightNoOfSegments[y] = GetRegistryEntry(READ_REGPATH + "Light", strTemp, -1);
	//	if(m_nLightNoOfSegments[y]==-1)
	//		m_nLightNoOfSegments[y] = 0;
	//	PutRegistryEntry(WRITE_REGPATH + "Light", strTemp, m_nLightNoOfSegments[y]);
	//	//OutputDebugLogTo(0,TRUE,"View%d  StartSeg%d  LightSeg%d", y, m_pApp->m_nStartSegmentsOfImage[y], m_nLightNoOfSegments[y]); //// for testing
	//}

	for(int nTrack=0, nIndex=m_nControllerSegCount; nTrack<nTracks; nTrack++, nIndex+=m_nSegmentsPerTrack) 
	{
		if(nTrack != 0 && m_bShareAllChannel)
			nIndex -= m_nSegmentsPerTrack;

		//pTracks[nTrack].m_nLightControllerId.Add(nTrack);
		for(int nController=0; nController<m_nControllerCount; nController++)
			pTracks[nTrack].m_nLightControllerId.Add(nController);
		//for(int nImage=0; nImage</*m_pApp->m_nTotalViews*/MAX_DOCS; nImage++) {
		//int nCount = m_pApp->m_pTracks[nTrack].m_nStartSegmentsOfImage.GetCount();
		//for(int nImage=0; nImage<nCount; nImage++) {	
		//	pTracks[nTrack].m_nLightStartSegmentNo.Add(m_pApp->m_pTracks[nTrack].m_nStartSegmentsOfImage[nImage]);
		//	//OutputDebugLogTo(0,TRUE,"T%d LightStartSeg%d", nTrack, pTracks[nTrack].m_nLightStartSegmentNo[nImage]); //// for testing
		//}
	}

	m_nImageCount = m_pApp->m_nTotalViews;//m_pApp->m_nTotalViews;//m_pApp->m_nTotalTracks;
}

void CLightHandler::SortingElements(CString strSrc, int nSrcCount, CString &strDst)
{
	int arr[8] = { -1 };
	int nTemp, nPos=0;
	CString strTemp, strTempAppend;

	for(int x=0; x<nSrcCount; x++) {
		strTemp = (nPos >= 0) ? strSrc.Tokenize(";", nPos) : "";
		if(strTemp.IsEmpty())
			strTemp.Format("0");
		arr[x] = (atoi(strTemp));
	}

	for (int i=0; i<nSrcCount; i++) {
        for (int j=0; j<(nSrcCount - i - 1); j++) {
            if (arr[j] > arr[j + 1]) {
                nTemp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = nTemp;
            }
        }
    }

	for(int y=0; y<nSrcCount; y++) {
		if(!y)
			strDst.Format("%d", arr[y]);
		else {
			strTempAppend.Format(";%d",arr[y]);;
			strDst.Append(strTempAppend);
		}
	}
}

int CLightHandler::ConfigureControllers()
{
	if(m_pApp->m_LightModule.hDLL == NULL)
		return -1;

	for(int nController=0; nController<m_nControllerCount; nController++) {
		if(m_pstrSerialNos[nController].CompareNoCase("none") != 0) {
            CString strPort = m_pstrSerialNos[nController];
			int nError = m_pApp->m_LightModule.ConfigIOPortLight(nController,strPort, 0, 0);
			if(nError==0)
				OutputDebugLogTo(0, TRUE, "Configured Light Controller %s successfully", m_pstrSerialNos[nController]);
			else {
				CString str;
				str.Format("Failed to Configure Light Controller %s", m_pstrSerialNos[nController]);
				OutputDebugLogTo(0, TRUE, str.GetBuffer(0));
				str.ReleaseBuffer();
				AfxMessageBox(str);
			}
		}
	}
	for(int nController=0; nController<m_nControllerCount; nController++) {
		m_pController[nController].Configure(nController, m_nControllerSegCount, m_pMaxCurrents, /*m_nMaxPulseWidth*/m_pMaxPulseWidth, m_nImageCount);
	}
	///LoadLightSettings();

	return 0;
}

int CLightHandler::LoadLightSettings(int nDocId, int nImage, BOOL bLightDlgOpen)
{
	int nTempDoc = 0 /*(MAX_TRACKS*nImage)+nDocId*/;
	for (int i = 0; i < nImage + 1; i++) {
		for (int nDoc = 0; nDoc < m_pApp->m_pTracks[i].m_nTotalDocs; nDoc++) {
			if (nDocId == nDoc && nImage == i)
				break;
			nTempDoc++;
		}
	}

	if(nImage == -1) {
		for(nImage=0; nImage<m_nImageCount; nImage++) {
			for(int nController=0; nController<m_nControllerCount; nController++)
				//m_pControllers[nController].LoadLightSetting(nController, m_pApp->m_strConfigDir, m_pApp->m_strConfigFile, nImage);
				m_pController[nController].LoadLightSetting(nController, m_pApp->m_pTracks[nImage].m_pDoc[nDocId]->m_strConfigDir, m_pApp->m_strConfigFile, nTempDoc, bLightDlgOpen, 0);
		}
		nImage = -1;
	}
	else {
		for (int nController = 0; nController < m_nControllerCount; nController++) {
			int m_nStartSegment = m_pApp->m_pTracks[nImage].m_nSegmentsOfImage[nDocId][0];
			int nLast = m_pApp->m_pTracks[nImage].m_nSegmentsOfImage[nDocId].GetCount();
			int m_nEndSegment = m_pApp->m_pTracks[nImage].m_nSegmentsOfImage[nDocId][nLast - 1];

			for (int i = 0; i < m_nSegmentsPerTrack; i++)
			{
				if (i >= m_nStartSegment&i <= m_nEndSegment)
					bTrackSegments[i] = TRUE;
				else
					bTrackSegments[i] = FALSE;
			}

			//m_pControllers[nController].LoadLightSetting(nController, m_pApp->m_strConfigDir, m_pApp->m_strConfigFile, nImage);
			m_pController[nController].LoadLightSetting(nController, m_pApp->m_pTracks[nImage].m_pDoc[nDocId]->m_strConfigDir, m_pApp->m_strConfigFile, nTempDoc, bLightDlgOpen, bTrackSegments);
		}
	}
////For RT command no need to set pulse width & refresh light OnInitial; since light intensity and pulse width is set/get together
	//if(nImage == (m_nImageCount-1)) {
	//	for(int nController=0; nController<m_nControllerCount; nController++)
	//	{
	//		m_pControllers[nController].SetAllPulseWidth();

	//		//RefreshLight(nController, -1);////for last initial grab no need of refresh
	//		RefreshLight(nController, 0);
	//	}
	//}

	return 0;
}

int CLightHandler::SaveLightSettings(int nDocId, int nImage)
{
	int nTempDoc = 0 /*(MAX_TRACKS*nImage)+nDocId*/;
	for (int i = 0; i <= nImage ; i++) {
		for (int nDoc = 0; nDoc < m_pApp->m_pTracks[i].m_nTotalDocs; nDoc++) {
			if (nDocId == nDoc && nImage == i)
				break;
			nTempDoc++;
		}
	}

	if(nImage == -1) {
		for(nImage=0; nImage<m_nImageCount; nImage++) {
			for(int nController=0; nController<m_nControllerCount; nController++)
				//m_pControllers[nController].SaveLightSetting(nController, m_pApp->m_strConfigDir, m_pApp->m_strConfigFile, nImage);
				m_pController[nController].SaveLightSetting(nController, m_pApp->m_pTracks[nImage].m_pDoc[nDocId]->m_strConfigDir, m_pApp->m_strConfigFile, nTempDoc);
		}
	}
	else {
		for(int nController=0; nController<m_nControllerCount; nController++)
			//m_pControllers[nController].SaveLightSetting(nController, m_pApp->m_strConfigDir, m_pApp->m_strConfigFile, nImage);
			m_pController[nController].SaveLightSetting(nController, m_pApp->m_pTracks[nImage].m_pDoc[nDocId]->m_strConfigDir, m_pApp->m_strConfigFile, nTempDoc);
	}

	return 0;
}

void CLightHandler::RefreshLight(int nController, int nDocId, int nImage)
{
	int nTempDoc =  0/*(MAX_TRACKS*nImage)+nDocId*/;
	for (int i = 0; i < nImage + 1; i++) {
		for (int nDoc = 0; nDoc < m_pApp->m_pTracks[i].m_nTotalDocs; nDoc++) {
			if (nDocId == nDoc && nImage == i)
				break;
			nTempDoc++;
		}
	}

	if(nController < m_nControllerCount)
		m_pController[nController].RefreshLight(/*nImage*/nTempDoc, bTrackSegments);
	else
		OutputDebugLogTo(0, TRUE, "RefreshLight: [FOV%d] Controller count (%d) exceelds limit (%d)", nController, m_nControllerCount);
}

int CLightHandler::RefreshLight()
{
	for(int nTrack=0; nTrack<m_pApp->m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			for (int nController = 0; nController < m_nControllerCount; nController++)
				RefreshLight(nController, nDoc, nTrack);
		}
	}
	return 0;
}

void CLightHandler::GetController(int& nController, int& nChannel)
{
	if(nChannel >= m_nControllerSegCount) {
		nController = nChannel / m_nControllerSegCount;
		nChannel = nChannel % m_nControllerSegCount;
	}
}

int CLightHandler::SetLight(int nController, int nChannel, int nCurrent, BOOL bSend)
{
	GetController(nController, nChannel);
	m_pController[nController].SetLight(nChannel, nCurrent, bSend);

	return nController;
}

int CLightHandler::SetAllLights(int nController, int nFOV, int nCurrent)
{
	return m_pController[nController].SetAllLights(nCurrent);
}

int CLightHandler::GetLight(int nController, int nChannel)
{
	GetController(nController, nChannel);
	return m_pController[nController].GetLight(nChannel);
}

int CLightHandler::SetPulseWidth(int nController, int nChannel, int nPulseWidth, BOOL bSend)
{
	GetController(nController, nChannel);
	m_pController[nController].SetPulseWidth(nChannel, nPulseWidth, bSend);

	return nController;
}

int CLightHandler::SetAllPulseWidth(int nController)
{
	//GetController(nController, nChannel);
	m_pController[nController].SetAllPulseWidth();

	return nController;
}

int CLightHandler::GetPulseWidth(int nController, int nChannel)
{
	GetController(nController, nChannel);
	return m_pController[nController].GetPulseWidth(nChannel);
}

int CLightHandler::SendLightCommandString(int nController, CString* pstrLightCommand)
{
	int nError;
	if(m_pApp->m_pMainWnd && IsWindow(m_pApp->m_pMainWnd->m_hWnd))
		nError = (int) m_pApp->m_pMainWnd->SendMessage(WM_SEND_LIGHTVALUE, (WPARAM) nController, (LPARAM) pstrLightCommand);
	else
		nError = m_pApp->m_LightModule.OutPortWriteString(nController, 0, pstrLightCommand, 0);

	return nError;
}

int CLightHandler::ResetLightSteps()
{
	//return m_pControllers[0].ResetLightSteps();

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;


	int nErr;
	for(int nFOV=0; nFOV< m_nNoOfFov; nFOV++) {
		for(int nController=0; nController<m_nControllerCount; nController++) {
			nErr = m_pController[nController].ResetLightSteps();
		}
	}

	return nErr;
}

//void CLightHandler::SendRTValues()
//{
//	return m_pControllers[0].SendRTValues(nChannel, nPulseWidth);
//}

void CLightHandler::UpdateLightStep(int nController, int nStep)
{
	//GetController(nController, nChannel);
	m_pController[nController].UpdateLightStep(nStep);
}