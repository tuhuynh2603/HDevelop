/// AppPropWnd.cpp : implementation of the AppPropWnd class
/// Udupa; April'2012
/// Udupa; Jan'2013; Added logging feature (parameter value changes) and Unit of measurement option

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "StatisticsView.h"

#include "AppDefectPropWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CAppDefectPropWnd::CAppDefectPropWnd()
{
	Initialize();
}

BEGIN_MESSAGE_MAP(CAppDefectPropWnd, CEvDefectPropertiesWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CAppDefectPropWnd::Initialize()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	m_nSets = 1;
	m_nUnitIndex = pApp->m_nUnitIndex;
}

void CAppDefectPropWnd::RefreshPropList()
{
	CEvDefectPropertiesWnd::RefreshPropList();
}

void CAppDefectPropWnd::InitPropList()
{
	CApplication* pApp = (CApplication*)AfxGetApp();

	//RefreshPropList();

	int nCount = pApp->m_ParametersDefectCode.GetCount();
	for(int i=0; i<pApp->m_ParametersDefectCode.GetCount(); i++)
		InitGroup(pApp->m_ParametersDefectCode[i], i);
	OnExpandAllProperties();
	m_wndPropLists[m_nCurSet].ShowWindow(SW_SHOW);
}

void CAppDefectPropWnd::SaveProperties()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	for(int i=0; i<pApp->m_ParametersDefectCode.GetCount(); i++)
		SaveGroup(pApp->m_ParametersDefectCode[i], i);

	pApp->SaveDefectCodeParameters(pApp->m_strConfigDir,pApp->m_strConfigFile);

	pApp->LoadDefectCodeParameters(pApp->m_strConfigDir, pApp->m_strConfigFile);

	m_wndPropLists[m_nCurSet].ResetOriginalValues();
	SetModified(FALSE);
}

void CAppDefectPropWnd::SaveAllProperties()
{
	int nOriginalSet = m_nCurSet;
	CApplication* pApp = (CApplication*) AfxGetApp();
	for(int i=0; i<m_nSets; i++) {
		if(m_wndPropLists[i].m_bIsModified) {
			m_nCurSet = i;
			SaveProperties();
		}
	}
	m_nCurSet = nOriginalSet;
}

void CAppDefectPropWnd::LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CString strMessage;
	strMessage.Format("Defect Parm-Change [%s.%s.%s] %3.1f->%3.1f",pParm->strInspection,
		pParm->arrParmInfo[nInfoIndex]->strSection,
		pParm->arrParmInfo[nInfoIndex]->strName,
		dOldValue,
		dNewValue);
	pApp->AddLogEvent(strMessage);
}
