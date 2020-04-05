/// Light Control Dialog: New interface for controlling all light heads in a single interface
/// Udupa; Sept'2012


#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "LightControlDlg.h"
#include "GardaLightCtrl.h"
#include "IOCode.h"
#include "AppDoc.h"
#include "StationMapping.h"
#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

//#define SKIPCHANNELS 0

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//// Rakshith; derived class for slider control; event handling for LButtonDown/Up;
CCustomSliderCtrl::CCustomSliderCtrl() 
{
	pApp = (CApplication*)AfxGetApp();
}

CCustomSliderCtrl::~CCustomSliderCtrl()
{
}

BEGIN_MESSAGE_MAP(CCustomSliderCtrl, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CCustomSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	pApp->m_pLightCtrlDlg->m_bLButtonDownOnSlider = TRUE;
	//OutputDebugLogTo(9, TRUE, "CCustomSliderCtrl::OnLButtonDown nFlag %d", nFlags);
	CSliderCtrl::OnLButtonDown(nFlags, point);
}

void CCustomSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	pApp->m_pLightCtrlDlg->m_bLButtonDownOnSlider = FALSE;
	//OutputDebugLogTo(9, TRUE, "CCustomSliderCtrl::OnLButtonUp nFlag %d", nFlags);
	CSliderCtrl::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CLightControlDlg dialog

	
CLightControlDlg::CLightControlDlg(CWnd* pParent)
	: CDialog(CLightControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLightControlDlg)
	//}}AFX_DATA_INIT

	m_nTrackId = 0;
	m_nDocId = 0;
	m_nFOV = 0;
	m_nLightValue = 0;
	m_nPulseWidth = 0;

	m_prgnLightSegment = NULL;
	m_bSelection = NULL;
	m_strLightValue= "";
	m_strCurrentRange = "";
	m_pApp = (CApplication*) AfxGetApp();
	m_pTrack = &m_pApp->m_pTracks[0];
	m_pLightHandler = &m_pApp->m_LightHandler;
	m_pLight = &m_pLightHandler->m_pController[0];
	m_nNoOfSegments = m_pLightHandler->m_bShareAllChannel ? m_pLightHandler->m_nSegmentsPerTrack : m_pApp->m_nTotalTracks * m_pLightHandler->m_nSegmentsPerTrack;
	if(m_nNoOfSegments > 8)
		m_nNoOfSegments = 8;
	for(int i=0;i<m_nNoOfSegments;i++)
	{
		m_nLightPulseChannels[i] = 0;
		bDocChannels[i]=FALSE;
	}
	m_bSkipSelection = NULL;
	m_bActiveChannel = NULL;

	m_brushActive.CreateSolidBrush(RGB(255,0,0));
	m_brushInActive.CreateSolidBrush(RGB(255,255,0));
	m_brushGrayed.CreateSolidBrush(RGB(192,192,192)); 
	m_brushBorder.CreateSolidBrush(RGB(0,0,0));

	m_bLButtonDownOnSlider = FALSE;
}

CLightControlDlg::~CLightControlDlg()
{
	if(m_bSelection != NULL)
		delete[] m_bSelection;

	if(m_bSkipSelection != NULL)
		delete[] m_bSkipSelection;

	if (m_bActiveChannel != NULL)
		delete[] m_bActiveChannel;
	
	DeleteObject(m_brushActive);
	DeleteObject(m_brushInActive);
	DeleteObject(m_brushGrayed);
	DeleteObject(m_brushBorder);

	if(m_prgnLightSegment != NULL) {
		for(int i=0; i<m_nNoOfSegments; i++)
			m_prgnLightSegment[i].DeleteObject();
		delete[] m_prgnLightSegment;
	}
	m_prgnLightSegment = NULL;
}


void CLightControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightControlDlg)
	DDX_Control(pDX, IDC_LIGHT_SCROLLBAR, m_ctrlLightScrollbar);
	DDX_Control(pDX, IDC_PULSEWIDTH_SLIDER, m_ctrlPulseWidthSlider);
	//}}AFX_DATA_MAP
	//	DDX_Check(pDX, IDC_RECALCFACTOR_CHECK, m_bRecalcFactor);
	//	DDX_Radio(pDX, IDC_INDIVIDUAL_RADIO, %m_nIndividualSeg);
	DDX_Text(pDX, IDC_PULSEWIDTH_US, m_nPulseWidth);
	DDX_Text(pDX, IDC_LIGHT_VALUE, m_strLightValue);
	DDX_Text(pDX, IDC_EDIT_LIGHT, m_strLightName);
	DDX_Text(pDX, IDC_STATIC_LIGHT_NAME, m_strLightTrackName);
	DDX_Text(pDX, IDC_CURRENTRANGE_STATIC, m_strCurrentRange);
	//DDV_MinMaxInt(pDX, m_nPulseWidth, 0, m_pLightHandler->m_nMaxPulseWidth);
	//DDX_Radio(pDX, IDC_RADIO_CAM1, m_nTrackId);
}


BEGIN_MESSAGE_MAP(CLightControlDlg, CDialog)
	//{{AFX_MSG_MAP(CLightControlDlg)
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_LIGHT_ON, OnAllLightsOn)
	ON_BN_CLICKED(IDC_LIGHT_OFF, OnAllLightsOff)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_PULSEWIDTH_US, OnEnChangePulsewidthUs)
	//ON_COMMAND_RANGE(IDC_RADIO_CAM1, IDC_RADIO_CAM2, OnRadioCam)
	ON_COMMAND_RANGE(IDC_RADIO_TYPE1, IDC_RADIO_TYPE5, OnRadioCam)
	ON_BN_CLICKED(IDC_CHECK_LIGHTSELECTALL, OnBnClickedCheckLightselectall)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightControlDlg message handlers

BOOL CLightControlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_prgnLightSegment!=NULL)
		delete [] m_prgnLightSegment;
	if(m_bSelection != NULL)
		delete[] m_bSelection;
	if(m_bSkipSelection != NULL)
		delete[] m_bSkipSelection;
	if (m_bActiveChannel != NULL)
		delete[] m_bActiveChannel;

	m_prgnLightSegment = new CRgn[m_nNoOfSegments];
	m_bSelection = new BOOL[m_nNoOfSegments];
	m_bSkipSelection = new BOOL[m_nNoOfSegments];
	m_bActiveChannel = new BOOL[m_nNoOfSegments];
	//m_strLightName.Format("Light%d", m_nDocId+1);
	m_strLightName.Format("");

	/*CString str[] = {"Top Dome Light(Inner)","Top Dome Light(Outer)","Top Coaxial", "T4","T5",
					"Bottom Dome Light(Inner)","Bottom Dome Light(Outer)","Bottom Coaxial","B4","B5",
					"Side Coaxial","S2"};*/
	for(int n=0; n<5; n++)
		GetDlgItem(IDC_RADIO_TYPE1+n)->ShowWindow(FALSE);

	CString strTemp;
	int nCount = m_pApp->m_nTotalViewDocs[m_nTrackId];
	for(int n=0; n<nCount; n++) {
		strTemp = "";
		int nIDx = !m_nTrackId ? n : (m_nTrackId==1 ? m_pApp->nTop+n : m_pApp->nTop+m_pApp->nBottom+n);
		if(nIDx < m_pApp->m_strLightChannelName.GetCount())
			strTemp.Format("%s", m_pApp->m_strLightChannelName.GetAt(nIDx));
		//strTemp = str[(MAX_TRACKS*m_nTrackId)+n];
		GetDlgItem(IDC_RADIO_TYPE1+n)->SetWindowTextA(strTemp);
		GetDlgItem(IDC_RADIO_TYPE1+n)->ShowWindow(TRUE);
	}

	CRect rect;
	for(int i=0; i<m_nNoOfSegments; i++) {
		m_bSelection[i] = FALSE;
		//m_bSkipSelection[i] = FALSE;
	/*	if(i>=SKIPCHANNELS)
			GetDlgItem(IDC_SEGMENT1 + i+SKIPCHANNELS)->GetWindowRect(&rect);
		else*/
			GetDlgItem(IDC_SEGMENT1 + i)->GetWindowRect(&rect);
			
		ScreenToClient(&rect);
		m_prgnLightSegment[i].CreateRoundRectRgn(rect.left-2, rect.top-2, rect.right+2, rect.bottom+2, 2, 2);
	}

	BOOL bSelectAll = FALSE;//GetRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SelectAllSegments", FALSE);
	((CButton*) GetDlgItem(IDC_CHECK_LIGHTSELECTALL))->SetCheck(bSelectAll);

	int nTempDoc = 0;
	for (int i = 0; i <= m_nTrackId; i++) {
		for (int nDoc = 0; nDoc < m_pApp->m_pTracks[i].m_nTotalDocs; nDoc++) {
			if (m_nDocId == nDoc && m_nTrackId == i)
				break;
			nTempDoc++;
		}
	}

//	m_pLightHandler->UpdateLightStep(0, nTempDoc);
	if (!m_pApp->m_bStationMapDlgOpen) {
		m_pLightHandler->LoadLightSettings(m_nDocId, m_nTrackId, TRUE);
	}
	SetCamera(TRUE);

	m_ctrlLightScrollbar.SetScrollRange(0, 100);
	///m_nLightValue = 100;
	m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
	UpdateLightValue();

	m_ctrlPulseWidthSlider.SetLineSize(1);
	m_ctrlPulseWidthSlider.SetPageSize(10);
	m_ctrlPulseWidthSlider.SetRange(0, m_pLightHandler->m_pMaxPulseWidth[0], TRUE);
	//if(m_pApp->m_nStartSegmentsOfImage[m_nDocId]>=SKIPCHANNELS)
	//	m_nPulseWidth = m_pLight->GetPulseWidth(m_pApp->m_nStartSegmentsOfImage[m_nDocId]+SKIPCHANNELS);
	//else
	//	m_nPulseWidth = m_pLight->GetPulseWidth(m_pApp->m_nStartSegmentsOfImage[m_nDocId]);
	m_nPulseWidth = m_pLight->GetPulseWidth(m_pApp->m_pTracks[m_nTrackId].m_nSegmentsOfImage[m_nDocId][0]);

	m_ctrlPulseWidthSlider.SetPos(m_nPulseWidth);

	for (int i = 0; i < m_nNoOfSegments; i++)
	{
		m_nOldLightValueChannels[i] = m_pLight->GetLight(i);
		m_nOldLightPulseChannels[i] = m_pLight->GetPulseWidth(i);
	}

	if(m_pApp->m_bSynchronousGrabEnable)
	{
		//GetDlgItem(IDC_RADIO_CAM1)->EnableWindow(FALSE);
		//GetDlgItem(IDC_RADIO_CAM2)->EnableWindow(FALSE);
	}
	((CButton*) GetDlgItem(IDC_RADIO_TYPE1+m_nDocId))->SetCheck(TRUE);

	if(!m_pTrack->m_pDoc[m_nDocId]->m_bLightCalibration &&!m_pTrack->m_bLiveCalibration && !m_pTrack->m_bLiveImage && !m_pApp->m_bOnline && !m_pTrack->m_bContInsp && !m_pTrack->m_pDoc[m_nDocId]->m_bInspSavedImage)
		m_pTrack->m_pDoc[m_nDocId]->CameraLive();
	UpdateData(FALSE);

	GetDlgItem(IDC_LIGHTHEAD_STATIC)->GetWindowRect(&m_rectLightHead);
	ScreenToClient(&m_rectLightHead);
	//SetWindowText(m_strTitle);
	if(m_pTrack->m_pDoc[m_nDocId]->m_bLightCalibration && !m_pTrack->m_bLiveCalibration && !m_pTrack->m_bLiveImage && !m_pApp->m_bOnline && !m_pTrack->m_bContInsp && !m_pTrack->m_pDoc[m_nDocId]->m_bInspSavedImage) {
		for(int i=0;i<m_nNoOfSegments;i++)
		{
			m_nLightPulseChannels[i] = 0;
			bDocChannels[i]=FALSE;
		}
		nCalibLightValue = 0;
		m_pApp->m_bCalib = FALSE;
		//SetAutoLightIntensity(0,nCalibGreyValue,TRUE);
		//m_pTrack->m_pDoc[m_nDocId]->CameraLiveCalibration();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLightControlDlg::SetCamera(BOOL bRefresh)
{
	int nTrack = m_nTrackId;
	int nDoc = m_nDocId;
	m_pTrack = &m_pApp->m_pTracks[nTrack];
	
	if(bRefresh) {
		for(int nController=0; nController< m_pLightHandler->m_nControllerCount; nController++)
			m_pLightHandler->RefreshLight(nController, nDoc, nTrack);
	}
	m_pLight = &m_pLightHandler->m_pController[m_pTrack->m_nLightControllerId[0]];

//	m_nStartSegment = m_pTrack->m_nLightStartSegmentNo[nDoc];

	//if(nDoc == m_pApp->m_nTotalViews-1)
	//	m_nEndSegment = m_pLightHandler->m_nSegmentsPerTrack;
	//else
	//	m_nEndSegment = m_pTrack->m_nLightStartSegmentNo[nDoc+1];//m_nStartSegment + m_pLightHandler->m_nSegmentsPerTrack;
//	m_nEndSegment = m_pTrack->m_nSegmentsOfImage[nDoc][0] + m_pLightHandler->m_nLightNoOfSegments[nDoc1];
	
	/*if(m_nStartSegment >= m_nEndSegment)
		m_nEndSegment = m_nStartSegment + 1;*/
	m_nStartSegment = m_pTrack->m_nSegmentsOfImage[nDoc][0];
	int nLast = m_pTrack->m_nSegmentsOfImage[nDoc].GetCount();
	m_nEndSegment = m_pTrack->m_nSegmentsOfImage[nDoc][nLast-1];

	SkipSegments(nDoc);

	if(((CButton*) GetDlgItem(IDC_CHECK_LIGHTSELECTALL))->GetCheck() /*&& m_nDocId!=6*/) {
		SelectSegments(m_nStartSegment, m_nEndSegment, TRUE);
	}
	//else if(m_nDocId==6)
	//{
	//	/*for(int i=0; i<m_nNoOfSegments; i++)
	//		m_bSelection[i] = FALSE;*/

	//	m_bSelection[0] = TRUE;
	//	m_bSelection[1] = FALSE;
	//	m_bSelection[2] = TRUE;

	//}
	else {
		for(int i=0; i<m_nNoOfSegments; i++)
			m_bSelection[i] = FALSE;
		//m_bSelection[m_nStartSegment] = TRUE;
		int nVal;
		for(int x=m_nStartSegment; x >= 0 && x<=m_nEndSegment; x++) {
			//nVal = (x>=m_nNoOfSegments) ? (x%m_nNoOfSegments) : x;
			//if(!m_bSkipSelection[x])
			if(m_bActiveChannel[x])
				m_bSelection[x] = TRUE;
		}

		//m_nStartSegment>=SKIPCHANNELS?nTempVar = m_nStartSegment+SKIPCHANNELS:nTempVar = m_nStartSegment;
		m_nLightValue = m_pLight->GetLight(m_nStartSegment);
		if (m_nLightValue < 0)
			m_nLightValue = 0;
		InvalidateRect(&m_rectLightHead);
		UpdateLightValue();
	}
}

void CLightControlDlg::OnRadioCam(UINT id)
{
	UpdateData(TRUE);
	m_nDocId = id - IDC_RADIO_TYPE1;

	SetCamera(TRUE);

	int nDoc = m_nDocId;//(MAX_TRACKS*m_nTrackId)+m_nDocId;
	m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
//	m_nPulseWidth = m_pLight->GetPulseWidth(m_pTrack->m_nLightStartSegmentNo[nDoc]);
	m_nPulseWidth = m_pLight->GetPulseWidth(m_pTrack->m_nSegmentsOfImage[nDoc][0]);
	m_ctrlPulseWidthSlider.SetPos(m_nPulseWidth);
	UpdateData(FALSE);
	
	if(!m_pTrack->m_bLiveImage && !m_pApp->m_bOnline && !m_pTrack->m_bContInsp && !m_pTrack->m_pDoc[m_nDocId]->m_bInspSavedImage)
		m_pTrack->m_pDoc[m_nDocId]->CameraLive();
}

void CLightControlDlg::UpdateLightValue()
{
	double dMinCurrent = INT_MAX;
	double dMaxCurrent = 0;
	//int nTempVar=0;
	int nVal;
	for(int i=m_nStartSegment, j=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++, j++) {
		//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
		if(m_bSelection[i] && !m_bSkipSelection[i]) {
			//i>=SKIPCHANNELS?nTempVar = j+SKIPCHANNELS:nTempVar=j;
			if(dMinCurrent > m_pLightHandler->m_pMaxCurrents[j])
				dMinCurrent = m_pLightHandler->m_pMaxCurrents[j];
			if(dMaxCurrent < m_pLightHandler->m_pMaxCurrents[j])
				dMaxCurrent = m_pLightHandler->m_pMaxCurrents[j];
		}
	}
	if(dMinCurrent==INT_MAX) {
		m_strLightValue = "";
		m_strCurrentRange = "";
	}
	else {
		if(dMaxCurrent != dMinCurrent) {
			m_strLightValue.Format("%d%%", m_nLightValue);
			m_strCurrentRange.Format("Max %.2f-%.2f", dMinCurrent, dMaxCurrent);
		}
		else {
			double dTempCurrent;

//			m_strLightValue.Format("%.3f", m_nLightValue * dMinCurrent / MAX_LIGHT_VALUE);
//			dTempCurrent = dMaxCurrent > 2. ? (Round((m_nLightValue * dMinCurrent / MAX_LIGHT_VALUE) *10.) / 10.) : (Round((m_nLightValue * dMinCurrent / MAX_LIGHT_VALUE) *10.) / 10.);
			dTempCurrent = Round((m_nLightValue * dMinCurrent / MAX_LIGHT_VALUE) *10.) / 10.;
			m_strLightValue.Format("%.3f", dTempCurrent);

			m_strCurrentRange.Format("Max %.2f", dMinCurrent);

			/*if ((m_nLightValue * dMinCurrent / 100.) < dTempCurrent)
				m_nLightValue++;*/
		
		}
	}

	UpdateData(FALSE);
}

void CLightControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar->m_hWnd == m_ctrlLightScrollbar.m_hWnd && nSBCode != SB_ENDSCROLL) {
		switch (nSBCode)
		{
			case SB_LEFT:
				m_nLightValue = 0;
				break;

			case SB_RIGHT:   
				m_nLightValue = 100;
				break;

			case SB_PAGELEFT:
				m_nLightValue -= 4;
				if (m_nLightValue < 0)
					m_nLightValue = 0;
				break;           

			case SB_PAGERIGHT:
				m_nLightValue += 4;
				if (m_nLightValue > 100)
					m_nLightValue = 100;
				break;

			case SB_THUMBPOSITION :
				break;

			case SB_THUMBTRACK :
				m_nLightValue = nPos;
				if (m_nLightValue < 0)
					m_nLightValue = 0;
				if (m_nLightValue > 100)
					m_nLightValue = 100;
				break;

			case SB_LINELEFT:
				m_nLightValue--;
				if (m_nLightValue < 0)
					m_nLightValue = 0;
				break;

			case SB_LINERIGHT:
				m_nLightValue++;
				if (m_nLightValue > 100)
					m_nLightValue = 100;
				break;
		}
		int nSingleSegmentSelected = -1;
		int nSegmentSelectedCount = 0;
		for (int i = m_nStartSegment; i >= 0 && i <= m_nEndSegment; i++) {
			if (m_bSelection[i] && !m_bSkipSelection[i]) {
				nSegmentSelectedCount++;
				nSingleSegmentSelected = i;
			}
		}

		if (nSegmentSelectedCount == 1 && nSingleSegmentSelected >= 0) {
			//Adjust so that a step is 0.1 mA
			if (nSBCode == SB_LINELEFT) {
				m_nLightValue++;

				double dLightValueCurrent = m_nLightValue * m_pLightHandler->m_pMaxCurrents[nSingleSegmentSelected] / 100;
				dLightValueCurrent = dLightValueCurrent - 0.1;
				m_nLightValue = Round(dLightValueCurrent * 100 / m_pLightHandler->m_pMaxCurrents[nSingleSegmentSelected]);

				//m_nLightValue = m_nLightValue - (0.1 * 100 / m_pLightHandler->m_pMaxCurrents[nSingleSegmentSelected]);

				if (m_nLightValue < 0)
					m_nLightValue = 0;
			}
			else if (nSBCode == SB_LINERIGHT) {
				m_nLightValue--;

				double dLightValueCurrent = m_nLightValue * m_pLightHandler->m_pMaxCurrents[nSingleSegmentSelected] / 100;
				dLightValueCurrent = dLightValueCurrent + 0.1;
				m_nLightValue = Round(dLightValueCurrent * 100 / m_pLightHandler->m_pMaxCurrents[nSingleSegmentSelected]);
				
				//m_nLightValue = m_nLightValue + (0.1 * 100 / m_pLightHandler->m_pMaxCurrents[nSingleSegmentSelected]);
				
				if (m_nLightValue > 100)
					m_nLightValue = 100;
			}
		}

		m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
		UpdateLightValue();
		int nVal;
		for(int i=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++) {
			//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
			if(m_bSelection[i] && !m_bSkipSelection[i]){
				/*if(i>=SKIPCHANNELS)
					m_pLight->SetLight(i+SKIPCHANNELS, m_nLightValue, FALSE);
				else*/
					m_pLight->SetLight(i, m_nLightValue, TRUE);
			}
			
		}
//		m_pLight->SetAllLights();
	}
	else if(pScrollBar->m_hWnd == m_ctrlPulseWidthSlider.m_hWnd && nSBCode != SB_ENDSCROLL) {
		//OutputDebugLogTo(9, TRUE, "Code %d, SliderDown %d", nSBCode, m_bLButtonDownOnSlider);
		if(m_bLButtonDownOnSlider)
			ApplyPulseWidth();
	}

	UpdateData(FALSE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//int CLightControlDlg::SelectLightController(int nSelRgn)
//{
//	int nIndex = nSelRgn / m_nNoOfSegments;
//	m_pLight = &m_pLightHandler->m_pControllers[nIndex];
//
//	return nIndex;
//}

void CLightControlDlg::ApplyPulseWidth()
{
	//OutputDebugLogTo(9, TRUE, "CLightControlDlg::ApplyPulseWidth");
	m_nPulseWidth = m_ctrlPulseWidthSlider.GetPos();
	int nVal;
	for(int i=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++){
		//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
		if(m_bSelection[i] && !m_bSkipSelection[i]){
			/*if(i>=SKIPCHANNELS)
				m_pLight->SetPulseWidth(i+SKIPCHANNELS, m_nPulseWidth, FALSE);
			else*/
				m_pLight->SetPulseWidth(i, m_nPulseWidth, TRUE);//FALSE
		}

	}
	//m_pLight->SetAllPulseWidth();//ll
}

void CLightControlDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nSelRgn = -1;

	CRect rect;
	int nVal;
	for(int i=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++) {
		//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
		if(m_prgnLightSegment[i].PtInRegion(point)  && !m_bSkipSelection[i]) {
			nSelRgn = i;
			break;
		}
	}
	if (nSelRgn != -1) {
		for(int i=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++) {
			//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
			if(!m_bSkipSelection[i])
				m_bSelection[i] = FALSE;
		}
		m_bSelection[nSelRgn] = TRUE;
		m_bActiveChannel[nSelRgn] = TRUE;
		//nSelRgn>=SKIPCHANNELS?(nSelRgn+=SKIPCHANNELS):nSelRgn;//JP
		m_nLightValue = m_pLight->GetLight(nSelRgn);
		if (m_nLightValue < 0)
			m_nLightValue = 0;
		m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
		m_nPulseWidth = m_pLight->GetPulseWidth(nSelRgn);
		m_ctrlPulseWidthSlider.SetPos(m_nPulseWidth);
		UpdateLightValue();
		UpdateData(FALSE);
		InvalidateRect(&m_rectLightHead);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CLightControlDlg::OnPaint() 
{
	CPaintDC dc(this);

	CDC* pDC = &dc;
	BOOL bSelected = FALSE;
	//int nVal=m_nEndSegment;
	for(int i=0; i<m_nNoOfSegments; i++) {
		if(m_bSelection[i]  && !m_bSkipSelection[i]) {
			pDC->FillRgn(&m_prgnLightSegment[i], &m_brushActive);
			bSelected = TRUE;
		}
		else {
			if(/*i>=m_nStartSegment && i<m_nEndSegment  &&*/ !m_bSkipSelection[i])
				pDC->FillRgn(&m_prgnLightSegment[i], &m_brushInActive);
			else
				pDC->FillRgn(&m_prgnLightSegment[i], &m_brushGrayed);
		}
		if(!m_bActiveChannel[i])
			pDC->FillRgn(&m_prgnLightSegment[i], &m_brushInActive);
		pDC->FrameRgn(&m_prgnLightSegment[i], &m_brushBorder, 1, 1);
	}
	m_ctrlLightScrollbar.EnableWindow(bSelected);
	m_ctrlPulseWidthSlider.EnableWindow(bSelected);
}

void CLightControlDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//int nSelRgn = -1;
	//int nVal;
	//for(int i=m_nStartSegment; i<=m_nEndSegment; i++) {
	//	//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
	//	if(m_prgnLightSegment[i].PtInRegion(point) && !m_bSkipSelection[i]) {
	//		nSelRgn = i;
	//		break;
	//	}
	//}
	//if (nSelRgn != -1) {
	//	m_bSelection[nSelRgn] = !m_bSelection[nSelRgn];
	////	nSelRgn>=SKIPCHANNELS?(nSelRgn+=SKIPCHANNELS):nSelRgn;//JP
	//	m_nLightValue = m_pLight->GetLight(nSelRgn);
	//	m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
	//	m_nPulseWidth = m_pLight->GetPulseWidth(nSelRgn);
	//	m_ctrlPulseWidthSlider.SetPos(m_nPulseWidth);
	//	UpdateLightValue();
	//	UpdateData(FALSE);
	//	InvalidateRect(&m_rectLightHead);
	//}

	CDialog::OnRButtonDown(nFlags, point);
}


void CLightControlDlg::OnAllLightsOn() 
{
	m_nLightValue = MAX_LIGHT_VALUE;
	m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);

	int nVal;
	for (int i=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++) {
		//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
		if(!m_bSkipSelection[i])
			m_bSelection[i] = TRUE;
			m_bActiveChannel[i] = TRUE;
		/*if(i>=SKIPCHANNELS)
			m_pLight->SetLight(i+SKIPCHANNELS, m_nLightValue, FALSE);
		else*/
			m_pLight->SetLight(i, m_nLightValue, TRUE);//
	}
	InvalidateRect(&m_rectLightHead);
	((CButton*) GetDlgItem(IDC_CHECK_LIGHTSELECTALL))->SetCheck(TRUE);

//	m_pLight->SetAllLights();
	UpdateLightValue();
}

void CLightControlDlg::OnAllLightsOff() 
{
	m_nLightValue = 0;
	m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
	UpdateLightValue();

	int nVal;
	for (int i=m_nStartSegment; i >= 0 && i<=m_nEndSegment; i++) {
		//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
		if(!m_bSkipSelection[i])
			m_bSelection[i] = TRUE;
		m_bActiveChannel[i] = FALSE;
		/*if(i>=SKIPCHANNELS)
			m_pLight->SetLight(i+SKIPCHANNELS, m_nLightValue, FALSE);
		else*/
			m_pLight->SetLight(i, m_nLightValue, TRUE);//
	}
	InvalidateRect(&m_rectLightHead);
	((CButton*) GetDlgItem(IDC_CHECK_LIGHTSELECTALL))->SetCheck(TRUE);

//	m_pLight->SetAllLights();
	UpdateLightValue();
}

void CLightControlDlg::OnOK() 
{
	if(m_pTrack->m_bLiveImage)
		m_pTrack->m_pDoc[m_nDocId]->CameraLive();

	
	/*if(m_pTrack->m_bLiveCalibration)
		m_pTrack->m_pDoc[m_nDocId]->CameraLiveCalibration();*/

	m_pApp->m_bCalib = FALSE;
	m_pApp->m_bLightCtrlDlgOpen = FALSE;
	
	//for(int nTrack=0; nTrack<m_pApp->m_nTotalTracks; nTrack++)
	if (!m_pApp->m_bStationMapDlgOpen) {
		m_pLightHandler->SaveLightSettings(m_nDocId, m_nTrackId);
	}
//	m_pLightHandler->UpdateLightStep(0, -2);
	if(m_pApp->m_bStationMapDlgOpen)
		m_pApp->m_pStationMapDlg->m_bLightDlgOpen = TRUE;

	CString strLightLogMsg;
	CString strIntensity;
	m_nTrackId == 0 ? strIntensity = m_pApp->m_strTopCameraName[m_nDocId] : m_nTrackId == 1 ? strIntensity = m_pApp->m_strBottomCameraName[m_nDocId] : strIntensity = m_pApp->m_strSideCameraName[m_nDocId];
	/*for (int i = 0; i < m_nNoOfSegments; i++)
	{
		strLightLogMsg.Format("[Light Settings:  Position%d, IntenSity:%s: Ch=%d  Li=%d->%d, PW=%d->%d,", m_nFOV + 1, strIntensity,i, m_nOldLightValueChannels[i], m_pLight->GetLight(i), m_nOldLightPulseChannels[i], m_pLight->GetPulseWidth(i));
		m_pApp->AddLogEvent(strLightLogMsg, LIGHT_LOG);
	}
	*/
	//m_pLightHandler->SaveLightSettings(m_nDocId);
	//m_pLightHandler->RefreshLight(m_nTrackId, -1);
//	m_pLightHandler->RefreshLight(m_pTrack->m_nLightControllerId[0], m_nTrackId);
//	m_pLightHandler->ResetLightSteps();

	//PutRegistryEntry(WRITE_REGPATH + "Light", "f. Pulse Width", m_nPulseWidth);
	m_pApp->StationMapDlgControls();


	CDialog::OnOK();
}

void CLightControlDlg::OnCancel() 
{
	if(m_pTrack->m_bLiveImage)
		m_pTrack->m_pDoc[m_nDocId]->CameraLive();

	/*if(m_pTrack->m_bLiveCalibration)
		m_pTrack->m_pDoc[m_nDocId]->CameraLiveCalibration();*/

	//m_pLightHandler->SetPulseWidth(m_pLightHandler->m_nPulseWidth, FALSE);
	if(!m_pApp->m_bStationMapDlgOpen)
		m_pLightHandler->LoadLightSettings(m_nDocId, m_nTrackId);

	m_pApp->m_bLightCtrlDlgOpen = FALSE;

	if(m_pApp->m_bStationMapDlgOpen)
		m_pApp->m_pStationMapDlg->m_bLightDlgOpen = FALSE;
	//m_pLightHandler->RefreshLight(m_nTrackId, -1);
//	m_pLightHandler->RefreshLight(m_pTrack->m_nLightControllerId[0], m_nTrackId);
//	m_pLightHandler->ResetLightSteps();
	m_pApp->StationMapDlgControls();
//	m_pLightHandler->UpdateLightStep(0, -2);

	CDialog::OnCancel();
}

void CLightControlDlg::SelectSegments(int nStartSegment, int nEndSegment, BOOL bSelect)
{
	if (nStartSegment >= 0 && nEndSegment >= 0) {
		///BOOL bSelect;

	///bSelect = !(m_bSelection[nStartSegment] && m_bSelection[nEndSegment-1]);

		int nStartVal = 0;
		for (int i = nStartVal; i < nStartSegment; i++)
			m_bSelection[i] = FALSE;
		int nVal;
		for (int i = nStartSegment; i <= nEndSegment; i++) {
			//nVal = (i>=m_nNoOfSegments) ? (i%m_nNoOfSegments) : i;
			if (!m_bSkipSelection[i])
				m_bSelection[i] = bSelect;
			else
				m_bSelection[i] = FALSE;//!bSelect;
			m_bActiveChannel[i] = bSelect;
		}
		for (int i = nEndSegment + 1; i < m_nNoOfSegments; i++)
			m_bSelection[i] = FALSE;
		/*if(nStartSegment>=SKIPCHANNELS)
			nStartSegment +=SKIPCHANNELS;*/
		m_nLightValue = m_pLight->GetLight(nStartSegment);
		if (m_nLightValue < 0)
			m_nLightValue = 0;
		m_ctrlLightScrollbar.SetScrollPos(m_nLightValue);
		m_nPulseWidth = m_pLight->GetPulseWidth(nStartSegment);
		m_ctrlPulseWidthSlider.SetPos(m_nPulseWidth);
		UpdateLightValue();

		UpdateData(FALSE);
		InvalidateRect(&m_rectLightHead);
	}
}

void CLightControlDlg::OnEnChangePulsewidthUs()
{
	UpdateData(TRUE);
	if(m_nPulseWidth > m_pLightHandler->m_pMaxPulseWidth[m_nDocId])
		m_nPulseWidth = m_pLightHandler->m_pMaxPulseWidth[m_nDocId];
	m_ctrlPulseWidthSlider.SetPos(m_nPulseWidth);

	ApplyPulseWidth();
}

void CLightControlDlg::OnBnClickedCheckLightselectall()
{
	BOOL bChecked = ((CButton*) GetDlgItem(IDC_CHECK_LIGHTSELECTALL))->GetCheck();
	SelectSegments(m_nStartSegment, m_nEndSegment, bChecked);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SelectAllSegments", bChecked);
}

void CLightControlDlg::SkipSegments(int nDoc)
{
	int nVal;
	int nDoc1 = (MAX_TRACKS*m_nTrackId)+m_nDocId;
	for (int i = 0; i < m_nNoOfSegments; i++)
	{
		m_bSkipSelection[i] = TRUE;
		m_bActiveChannel[i] = TRUE;
	}
		

	//if(m_nStartSegment != m_nEndSegment) {
	if(m_pLightHandler->m_nLightNoOfSegments[nDoc1]) {
		int a[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
		for(int x=0; x<m_pTrack->m_nSegmentsOfImage[nDoc].GetCount(); x++)
			a[x] = m_pTrack->m_nSegmentsOfImage[nDoc].GetAt(x);
		
		for(int z=m_nStartSegment; z >= 0 && z<=m_nEndSegment; z++) {
			//nVal = (z>=m_nNoOfSegments) ? (z%m_nNoOfSegments) : z;
			if(find(std::begin(a), std::end(a), z) != std::end(a))
				m_bSkipSelection[z] = FALSE;
			else
				m_bSkipSelection[z] = TRUE;
		}
	}

	// JY: select active channel
	if (m_pLightHandler->m_nLightNoOfSegments[nDoc1]) {
		int a[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
		for (int x = 0; x<m_pTrack->m_nSelectedSegmentsOfImage[nDoc].GetCount(); x++)
			a[x] = m_pTrack->m_nSelectedSegmentsOfImage[nDoc].GetAt(x);

		for (int z = m_nStartSegment; z >= 0 && z <= m_nEndSegment; z++) {
			if (find(std::begin(a), std::end(a), z) != std::end(a))
				m_bActiveChannel[z] = TRUE;
			else
				m_bActiveChannel[z] = FALSE;
		}
	}
}

int CLightControlDlg::SetAutoLightIntensity(int nIsRange,int nGreyValueOut,BOOL bInitialize)
{
	 //if(bInitialize){
		// //m_pLightHandler->LoadLightSettings(m_nDocId, 0/*m_nFOV*/, m_nTrackId);
		// OutputDebugLogTo(7,TRUE,"[Light Calibration]Load Light Setting nDoc = %d nTrack = %d",m_nDocId,m_nTrackId);
		// for(int i=0;i<m_nNoOfSegments;i++)
		// {
		//	 m_nLightPulseChannels[i] = m_pLight->GetPulseWidth(i);
		//	 if(m_nLightPulseChannels[i]>0)
		//		 bDocChannels[i]=TRUE;
		//	 else
		//		 bDocChannels[i]=FALSE;

		//	 OutputDebugLogTo(7,TRUE,"[Light Calibration]Before Initializing 1 for Light Channels %d,Light PulseWidth = %d",i+1,m_nLightPulseChannels[i]);
		//	 //nLightValueChannels[i] = 0;
		// }
		// for(int i = 0;i<m_nNoOfSegments;i++){
		//	if(bDocChannels[i])
		//	{
		//		m_nLightPulseChannels[i] = nCalibLightValue;
		//		OutputDebugLogTo(7,TRUE,"[Light Calibration]Before Initializing 2 for Light Channels %d,Light Values  = %d",i+1,m_nLightPulseChannels[i]);
		//	}
		// }
		// for(int i = 0;i<m_nNoOfSegments;i++){
		//	 m_pLight->SetPulseWidth(i, m_nLightPulseChannels[i], FALSE);
		//	//m_nLightValuesDisp[i] = (m_nLightValueChannels[i]*m_pLightHandler->m_pMaxCurrents[i])/100;
		// }
		// m_pLight->SetAllPulseWidth();
	 //}
	 //else if(nIsRange!=0)
	 //{
		// OutputDebugLogTo(8,TRUE,"[Light Calibration]After isRange 0 Light Values = %d",nCalibLightValue);

		// for(int i = 0;i<m_nNoOfSegments;i++){
		//	 if(bDocChannels[i])
		//	 {
		//		  nCalibVariation = m_pLightHandler->m_pMaxPulseWidth[i]/NO_OF_ITERATION;
		//		  if(nIsRange==-1)
		//			nCalibLightValue -= nCalibVariation;
		//		  else if(nIsRange==1)
		//			nCalibLightValue += nCalibVariation;

		//		 m_nLightPulseChannels[i] = nCalibLightValue;

		//		 if(m_nLightPulseChannels[i]<0) 
		//				m_nLightPulseChannels[i] = 0;

		//		 else if(m_nLightPulseChannels[i]>=m_pLightHandler->m_pMaxPulseWidth[i]) 
		//				m_nLightPulseChannels[i] = m_pLightHandler->m_pMaxPulseWidth[i];
		//		 OutputDebugLogTo(8,TRUE,"[Light Calibration]After Initializing 1 for Light Channels %d,Light Values = %d",i+1,m_nLightPulseChannels[i]);
		//	 }
		// }
		//for(int i = 0;i<m_nNoOfSegments;i++){
		//	m_pLight->SetPulseWidth(i, m_nLightPulseChannels[i], FALSE);
		//	//m_nLightValuesDisp[i] = (m_nLightValueChannels[i]*m_pLightHandler->m_pMaxCurrents[i])/100;
		//}
		////m_pLight->SetAllLights();
		// m_pLight->SetAllPulseWidth();
	 //}
	 //else if(nIsRange == 0){
	 // //OnOK();
	 // m_pTrack->m_pDoc[m_nDocId]->m_bLightCalibration = FALSE;
	 // UpdateStatusBar(&CString("Light Calibration Success"));
	 // //AfxMessageBox("Please Train for New Light Setting!");
	 //}

 return 0;
}