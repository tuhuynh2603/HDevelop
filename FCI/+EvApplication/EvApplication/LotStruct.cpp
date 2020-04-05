#include "stdafx.h"
#include "Application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LOTSUMMARY::LOTSUMMARY()
{
	Init();
}

void LOTSUMMARY::Init()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int nNoOfFov = nFovPerX * nFovPerY;
	int nNoOfDies = nDevicePerX * nDevicePerY;

	strOperatorID.Empty();
	strLotID.Empty();
	strPkgName.Empty();
	strComments.Empty();

	strStartLotTime.Empty();
	strEndLotTime.Empty();
	strStartTimeDay.Empty();
	strEndTimeDay.Empty();
	strStartTimeHr.Empty();
	strEndTimeHr.Empty();
	strDate.Empty();

	for(int nDie=0; nDie< nNoOfDies; nDie++){
		for(int j=0;j<nNoOfFov;j++){
			for(int i=0; i<theApp.m_nTotalViews; i++) {
				m_DisplaySummary[nDie][j][i].Init();
			}
		}
	}
	memset(&timePrevRecord, 0, sizeof(SYSTEMTIME));
}


void LOTSUMMARY::InitDisplaySummary(int nDoc,int nFov)
{
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int nNoOfFov = nFovPerX * nFovPerY;
	int nNoOfDies = nDevicePerX * nDevicePerY;

	if(nFov==-1 ||nDoc==-1)
		for (int nDie = 0; nDie < nNoOfDies; nDie++) {
			for (int j = 0; j < nNoOfFov; j++) {
				for (int i = 0; i<theApp.m_nTotalViews; i++)
					m_DisplaySummary[nDie][j][i].Init();
			}
			m_TotalDisplaySummary[nDie].Init();
			for (std::map<int, long>::iterator it = m_mapIndividualPviDefectCounter.begin(); it != m_mapIndividualPviDefectCounter.end(); it++) {
				it->second = 0;
			}
		}		
	else
		m_DisplaySummary[0][nFov][nDoc].Init(); 
}


TRACKSUMMARY::TRACKSUMMARY()
{
	Init();
}

void TRACKSUMMARY::Init()
{
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int nNoOfFov = nFovPerX * nFovPerY;
	int nNoOfDies = nDevicePerX * nDevicePerY;

	lTotalUnitsInspected = 0;
	lTotalUnitsPass	= 0;

	lTotalUnitsFail	= 0;
	lTotalUnitsInvalid = 0;
	dTotalYield = 0;


	lDieLocation			= 0;
	lDatumLocation			= 0;
	lTiltInsp				= 0;
	lCoverLayerInspection	= 0;
	lCoverLayerAlignment	= 0;
	lEncapInsufficient		= 0;
	lEncapExcess			= 0;
	lEncapCrack				= 0;
	lEncapBlowHole			= 0;
	lEncapContamination		= 0;
	lSlot					= 0;
	lDieEdge				= 0;
	lUserModifiedFalseAcceptance = 0;
	lUserModifiedFalseRejection = 0;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			lPviDefectCounter[i][j] = 0;

	for (int k= 0; k< nNoOfFov;k++)
		for (int l = 0; l< theApp.m_nTotalViews ; l++)
			for (int i = 0; i < 10; i++)
				for (int j = 0; j < 10; j++)
					lTotalPviDefectCounter[k][l][i][j] = 0;
}