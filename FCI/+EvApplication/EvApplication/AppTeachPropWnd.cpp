
#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "StatisticsView.h"

#include "AppTeachPropWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CAppTeachPropWnd::CAppTeachPropWnd()
{
	Initialize();
}

BEGIN_MESSAGE_MAP(CAppTeachPropWnd, CEvTeachPropertiesWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CAppTeachPropWnd::Initialize()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	m_nPos = MAX_FOV;
	m_nSets = pApp->m_nTotalViews;
	m_nTopSets = pApp->m_nTop;
	m_nBottomSets = pApp->m_nBottom;
	m_nSideSets = pApp->m_nSide;

	strTopName.RemoveAll();
	strBottomName.RemoveAll();
	strSideName.RemoveAll();

	strTopName.Copy(pApp->m_strTopCameraName);
	strBottomName.Copy(pApp->m_strBottomCameraName);
	strSideName.Copy(pApp->m_strSideCameraName);

	m_nUnitIndex = pApp->m_nUnitIndex;
}

void CAppTeachPropWnd::RefreshPropList(CEvImageDoc* pDoc, int nFOV)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pAppDoc = (CAppDoc*) pDoc;


	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	int nTrack, nDocId, nView;
	nTrack = pAppDoc->m_pTrack->m_nTrack;

	if (pAppDoc->m_pTrack->m_pActiveDocTeach != NULL)
		nDocId = pAppDoc->m_pTrack->m_pActiveDocTeach->m_nDocIdx;
	else
		nDocId = pAppDoc->m_nDocIdx;

	m_wndObjectComboPos.ResetContent();
	for (int i = 0; i < m_nNoOfFov; i++) {
		CString str;
		str.Format("Position %d", i + 1);
		m_wndObjectComboPos.AddString(str);
	}
	m_wndObjectComboPos.SetCurSel(nFOV);
	
//	int nView;
	theApp.motionParameters.GetViewID(nTrack, nDocId, nView);
//	nView = !nTrack ? nDocId : (nTrack==1 ? m_nTopSets+nDocId : m_nTopSets+m_nBottomSets+nDocId);
	m_wndObjectCombo.SetCurSel(nView);

	CEvTeachPropertiesWnd::RefreshPropList(pDoc, nFOV);
}

void CAppTeachPropWnd::InitPropList(CEvImageDoc* pDoc, int nFOV)
{
	if(!pDoc) {
		/*for(int i=0; i<m_nSets; i++)
			m_wndPropLists[i].RemoveAll();*/
		for(int i=0; i<m_nPos; i++) {
			for(int j=0; j<m_nSets; j++) {
				m_wndPropLists[i][j].RemoveAll();
			}
		}
	}
	else {
		CAppDoc* pAppDoc = (CAppDoc*) pDoc;
		RefreshPropList(pDoc, nFOV);
		for(int i=0; i<pAppDoc->m_InspectionHandler[nFOV].m_ParametersTeach.GetCount(); i++)
			InitGroup(&pDoc->m_ImageViewInfo, pAppDoc->m_InspectionHandler[nFOV].m_ParametersTeach[i], i);
		//OnExpandAllProperties();
	}
}

void CAppTeachPropWnd::SaveProperties()
{
	BOOL bAutoTeach = TRUE;
	CAppDoc* pDoc = (CAppDoc*) m_pDoc;

	int nDeviceDatumBk = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nDattamSearchNoOfDattams;
	int nCLRegionBk = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nCLInspRegion;
	int nCLDatumBk = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nCLDatum;
	int nPointerBk = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nPointer;
	//int nNozzleBk = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nNozzleTeachRegion;

	for(int i=0; i<pDoc->m_InspectionHandler[m_nCurPos].m_ParametersTeach.GetCount(); i++)
		SaveGroup(pDoc->m_InspectionHandler[m_nCurPos].m_ParametersTeach[i], i);

	CApplication* pApp = (CApplication*) AfxGetApp();
	pDoc->m_InspectionHandler[m_nCurPos].SaveTeachParameters(pDoc->m_strConfigDir, 
												 pApp->m_strConfigFile, m_nCurPos);

	pDoc->ApplyParameters(m_nCurPos);
	pDoc->m_pTrack->UpdateLotDefectsList();

	//Need to Re-Teach or AutoTeach for these parameters
	int nDeviceDatumUpdate = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nDattamSearchNoOfDattams;
	int nCLRegionUpdate = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nCLInspRegion;
	int nCLDatumUpdate = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nCLDatum;
	int nPointerUpdate = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nPointer;
	//int nNozzleUpdate = pDoc->m_InspectionHandler[m_nCurPos].m_Inspection.m_TrainingData.nNozzleTeachRegion;

	if ((nDeviceDatumBk != nDeviceDatumUpdate) || (nCLRegionBk != nCLRegionUpdate) || (nCLDatumBk != nCLDatumUpdate) || (nPointerBk != nPointerUpdate))
		bAutoTeach = FALSE;

	if (bAutoTeach) {
		int nTotalDocs = pDoc->m_pTrack->m_nTotalDocs;
		int nTrack = pDoc->m_pTrack->m_nTrack;
		for (int nDoc = 0; nDoc < pDoc->m_pTrack->m_nTotalDocs; nDoc++)
		{
			pApp->m_pTracks[nTrack].m_pDoc[nDoc]->SaveTeachParmAutoTeach(m_nCurPos);
		}
		//pDoc->SaveTeachParmAutoTeach(m_nCurPos);
	}
	else {
		CString str;
		str.Format("ReTeach");
		pApp->AddLogEvent(str);
		AfxMessageBox("Please Re-Teach");
	}

	//m_wndPropLists[m_nCurSet].ResetOriginalValues();
	m_wndPropLists[m_nCurPos][m_nCurSet].ResetOriginalValues();
	SetModified(FALSE);
}

void CAppTeachPropWnd::SaveAllProperties()
{
	int nOriginalPos = m_nCurPos;
	int nOriginalSet = m_nCurSet;
	
	CApplication* pApp = (CApplication*) AfxGetApp();
	for(int i=0; i<m_nPos; i++) {
		for(int j=0; j<m_nSets; j++) {
			//if(m_wndPropLists[i].m_bIsModified) {	
			if(m_wndPropLists[i][j].m_bIsModified) {	
				int nTrack, nDoc;
				nTrack = j<m_nTopSets ? 0 : (j>=(m_nTopSets+m_nBottomSets) ? 2 : 1);
				nDoc = !nTrack ? j : (nTrack==1 ? (j%(m_nTopSets+m_nBottomSets))-m_nTopSets : j%(m_nTopSets+m_nBottomSets));
				m_nCurSet = j;
				m_nCurPos = i;
				m_pDoc = pApp->m_pTracks[nTrack].m_pDoc[nDoc];
				SaveProperties();
			}
		}
	}

	m_nCurSet = nOriginalSet;
	m_nCurPos = nOriginalPos;
	int nCurTrack = m_nCurSet<m_nTopSets ? 0 : (m_nCurSet>=(m_nTopSets+m_nBottomSets) ? 2 : 1);
	int nOrginalDoc = m_nCurSet<m_nTopSets ? m_nCurSet : (m_nCurSet>=(m_nTopSets+m_nBottomSets) ? m_nCurSet%(m_nTopSets+m_nBottomSets) : (m_nCurSet%(m_nTopSets+m_nBottomSets))-m_nTopSets);
	m_pDoc = pApp->m_pTracks[nCurTrack].m_pDoc[nOrginalDoc];
}

void CAppTeachPropWnd::SelChangedCamerasCombo()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	int nTrack, nDoc;
	nTrack = m_nCurSet<m_nTopSets ? 0 : (m_nCurSet>=(m_nTopSets+m_nBottomSets) ? 2 : 1);
	nDoc = !nTrack ? m_nCurSet%m_nTopSets : (nTrack==2 ? m_nCurSet%m_nSideSets : m_nCurSet%m_nBottomSets);
	m_pDoc = pApp->m_pTracks[nTrack].m_pDoc[nDoc];
}

void CAppTeachPropWnd::SelChangedCamerasComboPos()
{
	//CAppDoc* pDoc = (CAppDoc*) m_pDoc;	
	//for(int i=0; i<pDoc->m_InspectionHandler[m_nCurPos].m_ParametersTeach.GetCount(); i++)
	//	InitGroup(&pDoc->m_ImageViewInfo, pDoc->m_InspectionHandler[m_nCurPos].m_ParametersTeach[i], i);
}

void CAppTeachPropWnd::LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pDoc = (CAppDoc*) m_pDoc;
	int nFOV = pDoc->m_pTrack->m_nFOVIndex;
	CString strMessage;
	strMessage.Format("Teach Parm-Change [Position%d,Intensity:%s, %s.%s.%s] %3.1f->%3.1f",
		nFOV+1, pDoc->m_pTrack->m_nTrack == 0 ? pApp->m_strTopCameraName[pDoc->m_nDocIdx] : pDoc->m_pTrack->m_nTrack == 1 ? pApp->m_strBottomCameraName[pDoc->m_nDocIdx] : pApp->m_strSideCameraName[pDoc->m_nDocIdx],
		pParm->strInspection,
		pParm->arrParmInfo[nInfoIndex]->strSection,
		pParm->arrParmInfo[nInfoIndex]->strName,
		dOldValue,
		dNewValue);
	pApp->AddLogEvent(strMessage);
}

void CAppTeachPropWnd::SetCurView(CEvImageDoc* pDoc)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pAppDoc = (CAppDoc*) pDoc;
	pApp->m_pTracks[pAppDoc->m_pTrack->m_nTrack].m_pActiveDocTeach = pAppDoc;

	pApp->m_nActiveTrackIdx = pAppDoc->m_pTrack->m_nTrack;
	pApp->m_nActiveDocIdx = pAppDoc->m_nDocIdx;
//	RefreshPropList(pAppDoc);
}