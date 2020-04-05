// ImageSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "ImageSelectionDlg.h"
#include "afxdialogex.h"
#include "AppView.h"
#include <direct.h>
#include "DefectErrorCodeSelection.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"

void DefectContainer::addGroup(const std::string & name, int id)
{
	if (std::none_of(_group.begin(), _group.end(), [id](const GroupInfo & info) { return info.id == id; })) {
		_group.push_back(GroupInfo(name, id));
	}
}

void DefectContainer::addDefect(const std::string & description, const std::string & code, int id, int groupId, bool good)
{
	addDefect(description, code, id, groupId, RGB(128, 128, 128), good);
}

void DefectContainer::addDefect(const std::string & description, const std::string & code, int id, int groupId,
	COLORREF color, bool good)
{
	if (std::none_of(_defect.begin(), _defect.end(), [id](const ItemInfo & info) { return info.id == id; })) {
		_defect.push_back(ItemInfo(description, code, id, groupId, color, good));
	}
}

void DefectContainer::setDefect(const ItemInfo & info)
{
	std::vector < ItemInfo >::iterator v = std::find_if(_defect.begin(), _defect.end(),
		[info](const ItemInfo & item) { return info.id == item.id; });

	if (v != _defect.end())
		v->set(info);
}

const std::vector < ItemInfo > & DefectContainer::getDefect() const
{
	return _defect;
}

std::string DefectContainer::getCode(int id) const
{
	std::vector < ItemInfo >::const_iterator v = std::find_if(_defect.begin(), _defect.end(),
		[id](const ItemInfo & item) { return id == item.id; });

	if (v != _defect.end())
		return v->code;
	else
		return "unknown code";
}

int DefectContainer::getId(int id) const
{
	std::vector < ItemInfo >::const_iterator v = std::find_if(_defect.begin(), _defect.end(),
		[id](const ItemInfo & item) { return id == item.id; });

	if (v != _defect.end())
		return v->id;
	else
		return UNKNOWN_DEFECT;
}

int DefectContainer::getCode(std::string code) const					//Girish
{
	std::vector < ItemInfo >::const_iterator v = std::find_if(_defect.begin(), _defect.end(),
		[code](const ItemInfo & item) { return strcmp(code.c_str(), item.code.c_str()) == 0; });

	if (v != _defect.end())
		return v->id;
	else
		return UNKNOWN_DEFECT;
}

int DefectContainer::getErrorDefectCode(int inspectionErrorCode) const
{
	return atoi(getInfo(getId(inspectionErrorCode))->code.c_str());
}

std::vector < ItemInfo >::iterator DefectContainer::getInfo(int id)
{
	return std::find_if(_defect.begin(), _defect.end(), [id](const ItemInfo & item) { return id == item.id; });
}

std::vector < ItemInfo >::const_iterator DefectContainer::getInfo(int id) const
{
	return std::find_if(_defect.begin(), _defect.end(), [id](const ItemInfo & item) { return id == item.id; });
}

ItemInfo DefectContainer::getDefect(int nIndex)
{
	if (nIndex < 0 || nIndex > _defect.size())
		return _defect[0];
	else
		return _defect[nIndex];
}

bool DefectContainer::isGood(int id) const
{
	std::vector < ItemInfo >::const_iterator v = std::find_if(_defect.begin(), _defect.end(),
		[id](const ItemInfo & item) { return id == item.id; });

	if (v != _defect.end())
		return v->good;
	else
		return false;
}

GroupInfo DefectContainer::getGroup(int id) const
{
	std::vector < GroupInfo >::const_iterator v = std::find_if(_group.begin(), _group.end(),
		[id](const GroupInfo & item) { return id == item.id; });

	if (v != _group.end())
		return *v;
	else
		return GroupInfo();
}

const std::vector < GroupInfo > & DefectContainer::getGroup() const
{
	return _group;
}

void DefectContainer::load(const std::string & name)
{
	int i = 0;
	CIniFile parameter;

	parameter.SetPath(name.c_str());
	if (parameter.ReadFile()) {

		for (std::vector < ItemInfo >::iterator item = _defect.begin(); item != _defect.end(); ++item) {
			CString sectionName;

			//sectionName.Format("Defect %d", item->id);
			sectionName = item->description.c_str();

			int nTrackIdx, nDocIdx;
			theApp.GetInformationFromErrorCode(item->id, sectionName, nTrackIdx, nDocIdx);
			if (nTrackIdx != -1 && nDocIdx != -1) {
				CString strDoc;
				strDoc = nTrackIdx == 0 ? theApp.m_strTopCameraName[nDocIdx] : (nTrackIdx == 1 ? theApp.m_strBottomCameraName[nDocIdx] : theApp.m_strSideCameraName[nDocIdx]);
				sectionName.Format("%s_%s", strDoc, item->description.c_str());
			}

			item->color = parameter.GetLong(sectionName.GetString(), "Color", item->color);
			item->code = parameter.GetCString(sectionName.GetString(), "Error Code", item->code.c_str());
			setDefect(_defect[item - _defect.begin()]);
			//theApp.UpdateDefectCodes(item->description.c_str(), item->code.c_str());
		}
	}
	save(name);
}

void DefectContainer::save(const std::string & name) const
{
	int i = 0;
	CIniFile parameter;

	parameter.SetPath(name.c_str());

	for (std::vector < ItemInfo >::const_iterator item = _defect.begin(); item != _defect.end(); ++item) {
		CString sectionName;

		sectionName = item->description.c_str();

		int nTrackIdx, nDocIdx;
		theApp.GetInformationFromErrorCode(item->id, sectionName, nTrackIdx, nDocIdx);
		if (nTrackIdx != -1 && nDocIdx != -1) {
			CString strDoc;
			strDoc = nTrackIdx == 0 ? theApp.m_strTopCameraName[nDocIdx] : (nTrackIdx == 1 ? theApp.m_strBottomCameraName[nDocIdx] : theApp.m_strSideCameraName[nDocIdx]);
			sectionName.Format("%s_%s", strDoc, item->description.c_str());
		}

		parameter.SetLong(sectionName.GetString(), "Color", item->color);
		parameter.SetCString(sectionName.GetString(), "Error Code", item->code.c_str());
		//	theApp.defctCodeArr[i++] = item->code;
	}

	parameter.WriteFile();
}


// CImageSelectionDlg dialog

IMPLEMENT_DYNAMIC(CImageSelectionDlg, CDialogEx)

CImageSelectionDlg::CImageSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageSelectionDlg::IDD, pParent)
{

	m_pApp = (CApplication*) AfxGetApp();
	bCloseLButtonThread = FALSE;
	/*nMaxNoDisplay = (m_pApp->m_nNoOfFov*m_pApp->m_nNoOfDies);
	nImageSets = m_pApp->m_nNoOfDies;*/
	nPosNew = -1;
	nPosOld = -1;
	m_ctrlDieImages = NULL;
	m_prgnDieMapPos = NULL;
	m_prgnDieBounday = NULL;
	bFovSelected = NULL;
	m_pBrushTemp[0].CreateSolidBrush(RGB(255, 0, 0));
	m_pBrushTemp[1].CreateSolidBrush(RGB(0, 255, 0));
	m_pBrushTemp[2].CreateSolidBrush(RGB(128, 128, 128));
	for (int i = 0; i < 5; i++) {
		m_bCheckPass[i] = FALSE;
		m_nKeyInErrorCode[i] = 98765;
		m_strKeyInErrorCode[i] = "98765";
	}
	m_brushBorder.CreateSolidBrush(RGB(0, 0, 0));
	m_brushBorderSelected.CreateSolidBrush(RGB(255, 255, 0));
	m_brushDeviceBorder.CreateSolidBrush(RGB(255, 0, 255));

	addGroup("Good unit", GOOD_UNIT);
	addGroup("Not inspected", NOT_INSPECTED);
	addGroup("Unknown defect", UNKNOWN_DEFECT);
	//addGroup("Process Error", ERROR_PROCESS);

	addDefect("Good unit", "Good unit", GOOD_UNIT, GOOD_UNIT, RGB(20, 230, 20), true);
	addDefect("Not inspected", "Not inspected", NOT_INSPECTED, NOT_INSPECTED, RGB(64, 64, 64), true);
	addDefect("Unknown defect", "Unknown defect", UNKNOWN_DEFECT, UNKNOWN_DEFECT, RGB(255, 0, 0), false);
	//addDefect("Process Error", "Process Error", ERROR_PROCESS, ERROR_PROCESS, RGB(0, 0, 255), true);

}

CImageSelectionDlg::~CImageSelectionDlg()
{

	if(WaitForSingleObject(m_hLButtonDownThread, 1000) != WAIT_OBJECT_0)///
		TerminateThread(m_hLButtonDownThread, 0);/// 

	bCloseLButtonThread = TRUE;
	//SetEvent(m_hPicCtrlImgHistory);
	CloseHandle(m_hPicCtrlImgHistory);///
	if (m_ctrlDieImages!= NULL) {
		delete[] m_ctrlDieImages;
		m_ctrlDieImages = NULL;
	}

	if (bFovSelected != NULL)
		delete[] bFovSelected;

	if (m_prgnDieMapPos != NULL) {
		for (int i = 0; i<nMaxNoDisplay; i++)
			m_prgnDieMapPos[i].DeleteObject();
		delete[] m_prgnDieMapPos;
	}
	m_prgnDieMapPos = NULL;

	if (m_prgnDieBounday != NULL)
		delete[] m_prgnDieBounday;
	

	m_brushUncheck.DeleteObject();
	m_brushBorder.DeleteObject();

	m_pBrushTemp[0].DeleteObject();
	m_pBrushTemp[1].DeleteObject();
	m_pBrushTemp[2].DeleteObject();
}

void CImageSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < MAX_DIES; i++) {
		DDX_Check(pDX, IDC_CHECK_PART_1 + i, m_bCheckPass[i]);
		DDX_Text(pDX, IDC_EDIT_ERROR_CODE_PART1+i, m_strKeyInErrorCode[i]);
	}
	
}


BEGIN_MESSAGE_MAP(CImageSelectionDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_RESULT_CONFIRM, &CImageSelectionDlg::OnBnClickedButtonResultConfirm)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHECK_PART_1, &CImageSelectionDlg::OnBnClickedCheckPart1)
	ON_BN_CLICKED(IDC_CHECK_PART_2, &CImageSelectionDlg::OnBnClickedCheckPart2)
	ON_BN_CLICKED(IDC_CHECK_PART_3, &CImageSelectionDlg::OnBnClickedCheckPart3)
	ON_BN_CLICKED(IDC_CHECK_PART_4, &CImageSelectionDlg::OnBnClickedCheckPart4)
	ON_BN_CLICKED(IDC_CHECK_PART_5, &CImageSelectionDlg::OnBnClickedCheckPart5)
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_RESCAN, &CImageSelectionDlg::OnBnClickedButtonRescan)
END_MESSAGE_MAP()


// CImageSelectionDlg message handlers


BOOL CImageSelectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rctDesktop;
	SystemParametersInfo(SPI_GETWORKAREA,NULL, &rctDesktop, NULL);
	m_fontList.CreateFont((int) (rctDesktop.Width() / 1280.0 * 15.52), 0, 0, 0, 350, FALSE, FALSE, FALSE, 
						ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
						PROOF_QUALITY, FIXED_PITCH|FF_MODERN, "Segoe UI");

	return TRUE;
}

void CImageSelectionDlg::Initialize()
{
	m_hPicCtrlImgHistory = CreateEventA(NULL, FALSE, FALSE, NULL);///
	m_hLButtonDownThread = AfxBeginThread(LButtonDownThread, this, THREAD_PRIORITY_NORMAL);///

	InitiDieControls();
}

void CImageSelectionDlg::InitiDieControls()
{
//	nMaxNoDisplay = (m_pApp->m_nNoOfFov * m_pApp->m_nNoOfDies);
//	nImageSets = m_pApp->m_nNoOfDies;

	int nBlockX, BlockY;
	int nFovX, nFovY;

	m_pApp->motionParameters.GetMappingParameters(nBlockX, BlockY, nFovX, nFovY);
	nMaxNoDisplay = nFovX * nFovY * nBlockX * BlockY;

	nImageSets = nFovX * nFovY;


	if (m_ctrlDieImages!= NULL)
		delete[] m_ctrlDieImages;

	if (bFovSelected != NULL) {
		delete[] bFovSelected;
		bFovSelected = NULL;
	}

	bFovSelected = new BOOL[nMaxNoDisplay];
	m_ctrlDieImages = new CPictureCtrl[nMaxNoDisplay];
	
	CSize szTemp = CSize(100, 100);
	for (int i = 0; i < nMaxNoDisplay; i++) {
	
		m_ctrlDieImages[i].Create("", 1 , CRect(0, 0, 100, 100), this);
		//enum AdjustType { adjustBorder = 0, adjustOutside = 1 };
	}
	for (int i = 0; i < MAX_DIES; i++) {
		bColorFailPass[i] = FALSE;
		bColorVerifyFailPass[i] = FALSE;
	}
	m_nDisplayImgSet = nMaxNoDisplay/nImageSets;

	if(IsWindow(m_ctrlDieImages->m_hWnd)) {
		//for(int i=0; i</*nMaxNoDisplay*/nMaxNoDisplay; i++) {
		//	CImageBuffer bufImg;
		//	szTemp = CSize(100, 100);
		//	bufImg.Allocate(szTemp);
		//	bufImg.ClrImg();
		//	m_ctrlDieImages[i].ImportBuffer(bufImg, 0);
		//	//m_ctrlDieImages[i].ShowWindow(FALSE);
		//	m_ctrlDieImages[i].EnableToolTips(TRUE);
		//}
	}
	for (int i = 0; i < MAX_DIES; i++) {
		GetDlgItem(IDC_STATIC_PART1 + i)->ShowWindow(FALSE);
		GetDlgItem(IDC_CHECK_PART_1 + i)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->ShowWindow(FALSE);
	}

	GetDlgItem(IDC_BUTTON_RESULT_CONFIRM)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RESCAN)->ShowWindow(FALSE);

	GetDlgItem(IDC_STATIC_CLIENT_ID)->ShowWindow(FALSE);


	if (nPosOld > -1 && nPosOld<nMaxNoDisplay) {
		m_ctrlDieImages[nPosOld].m_bSelected = FALSE;
		m_ctrlDieImages[nPosOld].Invalidate();
		bFovSelected[nPosOld] = FALSE;
		nPosOld = -1;
		m_pApp->m_nSaveAllDieIndex = -1;
		Invalidate();
	}
	else {
		nPosOld = -1;
	}
	EnableToolTips();

	EnableVerification(m_pApp->m_bShowDieVerification);
}


void CImageSelectionDlg::SetSize(int cx,int cy)
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);

	CRect rect1;
	GetWindowRect(&rect1);
	int nHeight, nWidth;
	int nID = IDC_STATIC_IMG_SEL1;

	int nDeviceNoPerSubstrateX, nDeviceNoPerSubstrateY;
	int nDieNoPerDeviceX, nDieNoPerDeviceY;

	m_pApp->motionParameters.GetMappingParameters(nDeviceNoPerSubstrateX, nDeviceNoPerSubstrateY, nDieNoPerDeviceX, nDieNoPerDeviceY);
	int m_nNoOfDies = nDeviceNoPerSubstrateX * nDeviceNoPerSubstrateY;
	nMaxNoDisplay = m_nNoOfDies * nDieNoPerDeviceX * nDieNoPerDeviceY;
	int nNoofFovs = nDieNoPerDeviceX * nDieNoPerDeviceY;


	if (m_prgnDieMapPos != NULL) {
		for (int i = 0; i<nMaxNoDisplay; i++)
			m_prgnDieMapPos[i].DeleteObject();	
		delete[] m_prgnDieMapPos;
		m_prgnDieMapPos = NULL;
	}
	m_prgnDieMapPos = new CRgn[nMaxNoDisplay];

	if (m_prgnDieBounday != NULL) {
		for (int i = 0; i < m_nNoOfDies; i++)
			m_prgnDieBounday[i].DeleteObject();
		delete[] m_prgnDieBounday;
		m_prgnDieBounday = NULL;
	}
	m_prgnDieBounday = new CRgn[m_nNoOfDies];

	m_diePicCtrl = new CRect[nMaxNoDisplay];

	//Invalidate();
	nWidth = 40;//cx/m_pApp->m_nNoOfFov;//5;
	nHeight = 40;//cy/nImageSets;//3;
	int nTempHeight;
	int nTempWdt, nTempId, nTempMul=-1;
	CRect rectDieMap;


	int nDieOffsetX = 0;
	int nDieOffsetY = 0;
	int nDiePerLine = nDeviceNoPerSubstrateX * nDieNoPerDeviceX;

	int nDieDeviceIDX;
	int nDieDeviceIDY;

	int nX;
	int nY;

	int nDieRectSize = 40;
	int nDieShiftX;
	int nDieShiftY;

	BOOL bHorizontalDie;

	theApp.motionParameters.getDeviceDirection(bHorizontalDie);

	if (bHorizontalDie) {
		nDieShiftX = 0.5;
		nDieShiftY = 2; 
	}
	else {
		nDieShiftX = 2;
		nDieShiftY = 0.5;
	}

	long rectLeft[MAX_DIES];
	long rectTop[MAX_DIES];
	long rectRight[MAX_DIES];
	long rectBottom[MAX_DIES];

	for(int i=0; i<nMaxNoDisplay; i++) {

		nDieOffsetX = i % nDiePerLine;
		nDieDeviceIDX = nDieOffsetX / nDieNoPerDeviceX;
		nDieOffsetX = nDieOffsetX % nDieNoPerDeviceX;

		nDieOffsetY = i / nDiePerLine;
		nDieDeviceIDY = nDieOffsetY / nDieNoPerDeviceY;
		nDieOffsetY = nDieOffsetY % nDieNoPerDeviceY;

		nX = nDieDeviceIDX * (4 + 1 + (nDieRectSize + 1) * nDieNoPerDeviceX) + 2 + (nDieRectSize + nDieShiftX) * nDieOffsetX + 4;
		nY = nDieDeviceIDY * (4 + 1 + (nDieRectSize + 1) * nDieNoPerDeviceY) + 2 + (nDieRectSize + nDieShiftY) * nDieOffsetY + 4;

		nTempWdt = i%nDeviceNoPerSubstrateX;
		m_ctrlDieImages[i].SetWindowPos(NULL, nX, nY, nDieRectSize, nDieRectSize, SWP_NOZORDER);
		m_ctrlDieImages[i].GetWindowRect(&rectDieMap);
		ScreenToClient(&rectDieMap);
		m_prgnDieMapPos[i].CreateRoundRectRgn(rectDieMap.left, rectDieMap.top, rectDieMap.right , rectDieMap.bottom , 2, 2);
		m_ctrlDieImages[i].ShowWindow(TRUE);
		bFovSelected[i] = FALSE;


		int nFov, nColumn, nRow, nDevice;
		theApp.motionParameters.getImageMappingDetails(i, nRow, nColumn, nDevice, nFov);

		if (nFov == 0){
			rectLeft[nDevice] = rectDieMap.left -1;
			rectTop[nDevice] = rectDieMap.top - 1;
	//		m_prgnDieBounday[nDevice].CreateRoundRectRgn(rectDieMap.left -1 , rectDieMap.top -1, rectDieMap.right * ,)
		}

		if (nFov == nNoofFovs - 1) {
			rectRight[nDevice] = rectDieMap.right + 1;
			rectBottom[nDevice] = rectDieMap.bottom + 1;
		}

	}

	for (int i = 0; i < m_nNoOfDies; i++) {
		m_prgnDieBounday[i].CreateRoundRectRgn(rectLeft[i], rectTop[i], rectRight[i], rectBottom[i], 2, 2);
	}

	if (nPosOld > -1)
		bFovSelected[nPosOld] = TRUE;

	CRect rectPartError, rectPartErrorCheck, rectPartErrorEdit;
	for (int i = 0; i < nDeviceNoPerSubstrateX * nDeviceNoPerSubstrateY; i++) {
		GetDlgItem(IDC_STATIC_PART1 + i)->GetWindowRect(rectPartError);
		GetDlgItem(IDC_CHECK_PART_1 + i)->GetWindowRect(rectPartErrorCheck);
		GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->GetWindowRect(rectPartErrorEdit);
		GetDlgItem(IDC_STATIC_PART1 + i)->SetWindowPos(NULL, nWidth*(nDieNoPerDeviceX * nDeviceNoPerSubstrateX + 2), nHeight *i + /*(nHeight * nDieNoPerDeviceY)*/ +2, rectPartError.Width(), nHeight, SWP_NOZORDER);
		GetDlgItem(IDC_CHECK_PART_1 + i)->SetWindowPos(NULL, nWidth*(nDieNoPerDeviceX * nDeviceNoPerSubstrateX + 2 ) + rectPartError.Width(), nHeight * i /*+ (nHeight*nDieNoPerDeviceY)*/, rectPartErrorCheck.Width(), rectPartErrorCheck.Height(), SWP_NOZORDER);
		GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->SetWindowPos(NULL, nWidth*(nDieNoPerDeviceX * nDeviceNoPerSubstrateX + 2) + rectPartError.Width()+ rectPartErrorCheck.Width(), nHeight *i /* + (nHeight*nDieNoPerDeviceY)*/, rectPartErrorEdit.Width(), rectPartErrorEdit.Height(), SWP_NOZORDER);
		GetDlgItem(IDC_STATIC_PART1 + i)->SetFont(&m_fontList);
		GetDlgItem(IDC_CHECK_PART_1 + i)->SetFont(&m_fontList);
	}
 	CRect rectPartButton;
	GetDlgItem(IDC_BUTTON_RESULT_CONFIRM)->GetWindowRect(rectPartButton);
	GetDlgItem(IDC_BUTTON_RESULT_CONFIRM)->SetWindowPos(NULL, nWidth*(nDieNoPerDeviceX * nDeviceNoPerSubstrateX + 6), nHeight * (m_nNoOfDies + 1) /*( nHeight*nDieNoPerDeviceY)*/, rectPartButton.Width(), rectPartButton.Height(), SWP_NOZORDER);
	
	GetDlgItem(IDC_BUTTON_RESCAN)->GetWindowRect(rectPartButton);
	GetDlgItem(IDC_BUTTON_RESCAN)->SetWindowPos(NULL, nWidth*(nDieNoPerDeviceX * nDeviceNoPerSubstrateX + 2), nHeight * (m_nNoOfDies + 1) /*( nHeight*nDieNoPerDeviceY)*/, rectPartButton.Width(), rectPartButton.Height(), SWP_NOZORDER);

	CRect rectClientID;
	GetDlgItem(IDC_STATIC_CLIENT_ID)->GetWindowRect(rectClientID);

	GetDlgItem(IDC_STATIC_CLIENT_ID)->SetWindowPos(NULL, nWidth*(nDieNoPerDeviceX * nDeviceNoPerSubstrateX + 6) + rectPartButton.Width() + 20
		, nHeight * (m_nNoOfDies + 1), rectClientID.Width(), 2*rectClientID.Height(), SWP_NOZORDER);

	CRect rectMsg;
	GetDlgItem(IDC_STATIC_DISPLAY_MSG)->GetWindowRect(rectMsg);
	GetDlgItem(IDC_STATIC_DISPLAY_MSG)->SetWindowPos(NULL,0, nHeight*nDieNoPerDeviceX + nDeviceNoPerSubstrateX +rectMsg.Height(), rectMsg.Width(), rectMsg.Height(), SWP_NOZORDER);
//	GetDlgItem(IDC_STATIC_DISPLAY_MSG)->SetFont(&m_fontList);
	GetDlgItem(IDC_STATIC_DISPLAY_MSG)->ShowWindow(FALSE);

}


void CImageSelectionDlg::DisplayImages(int nTrack)
{
	//for(int i=0; i<IMAGE_SETS; i++){
	//	switch(nTrack)
	//	{
	//		case 0:m_staticBrightImages[i].ImportBuffer(m_bufBrightImages[i], i);
	//				OutputDebugLogTo(7,TRUE,"Display Image Bright Doc%d",nTrack);
	//				break;
	//		case 1:m_staticDarkImages[i].ImportBuffer(m_bufDarkImages[i], i);		
	//				OutputDebugLogTo(7,TRUE,"Display Image DrakSpot Doc%d",nTrack);
	//				break;
	//		case 2:m_staticSingleSpotImages[i].ImportBuffer(m_bufSingleSpotImages[i], i);
	//				OutputDebugLogTo(7,TRUE,"Display Image SingleSpot Doc%d",nTrack);
	//				break;
	//	}

	//}
}

void CImageSelectionDlg::OnLButtonDown(UINT nFlags, CPoint point) //JP
{
	ptLButtonDownPoint = point;///
	SetEvent(m_hPicCtrlImgHistory);///
	CDialog::OnLButtonDown(nFlags, point);
}


void CImageSelectionDlg::CopyImagesToStats(CImageBuffer** pBufs)
{
	/*CApplication* pApp = (CApplication*)AfxGetApp();
	CImageBuffer* bufTemp;
	CImageBuffer* bufTemp1;
	int nDoc = 0 ;
	CInspection* pInsp = &pApp->m_pTracks[0].m_pDoc[nDoc]->m_InspectionHandler.m_Inspection;
	for(nDoc=0;nDoc<pApp->m_nTotalViews;nDoc++){
		for(int i = IMAGE_SETS - 1;i>=0;i--){
			
			switch(nDoc)
			{
			case 0: bufTemp = &m_ImageHistoryBright[i].m_StorageBuffer;
					bufTemp1 = &m_ImageHistoryBright[i+1].m_StorageBuffer;
					break;
			case 1: bufTemp = &m_ImageHistoryDark[i].m_StorageBuffer;
					bufTemp1 = &m_ImageHistoryDark[i+1].m_StorageBuffer;
					break;
			case 2: bufTemp = &m_ImageHistorySingle[i].m_StorageBuffer;
					bufTemp1 = &m_ImageHistorySingle[i+1].m_StorageBuffer;
					break;
		
			}
			CSize sizeBuf = pApp->m_pTracks[0].m_pDoc[nDoc]->m_Buffer.GetImgSize();
			CRect rectBuf = CRect(CPoint(0, 0), sizeBuf);

			if(i!=IMAGE_SETS-1){
				ImgCopy(bufTemp, &rectBuf, bufTemp1, &rectBuf);
			}
			if(i==0)
			{
				ImgCopy(pBufs[nDoc], &rectBuf, bufTemp, &rectBuf);
			}
		}
	
	
		for(int i=0; i<IMAGE_SETS; i++){
			switch(nDoc)
			{
				case 0: pInsp->ZoomImage(&m_ImageHistoryBright[i].m_StorageBuffer, &m_ImageHistoryZoomBright[i].m_StorageBufferZoom,nDoc);
						ImgCopyToStorageBufZoom(nDoc, i,&m_ImageHistoryZoomBright[i].m_StorageBufferZoom);
						m_staticBrightImages[i].ImportBuffer(m_ImageHistoryZoomBright[i].m_StorageBufferZoom, nDoc);
						OutputDebugLogTo(7,TRUE,"Display Image Bright Doc%d",nDoc);
						break;
				case 1: pInsp->ZoomImage(&m_ImageHistoryDark[i].m_StorageBuffer, &m_ImageHistoryZoomDark[i].m_StorageBufferZoom,nDoc);
						ImgCopyToStorageBufZoom(nDoc, i,&m_ImageHistoryZoomDark[i].m_StorageBufferZoom);
						m_staticDarkImages[i].ImportBuffer(m_ImageHistoryZoomDark[i].m_StorageBufferZoom, nDoc);		
						OutputDebugLogTo(7,TRUE,"Display Image DrakSpot Doc%d",nDoc);
						break;
				case 2: pInsp->ZoomImage(&m_ImageHistorySingle[i].m_StorageBuffer, &m_ImageHistoryZoomSingle[i].m_StorageBufferZoom,nDoc);
						ImgCopyToStorageBufZoom(nDoc, i,&m_ImageHistoryZoomSingle[i].m_StorageBufferZoom);
						m_staticSingleSpotImages[i].ImportBuffer(m_ImageHistoryZoomSingle[i].m_StorageBufferZoom, nDoc);
						OutputDebugLogTo(7,TRUE,"Display Image SingleSpot Doc%d",nDoc);
						break;
			}

		}
	}
*/
}


void CImageSelectionDlg::ImgCopyToStorageBufZoom(int nDoc, int nImgIndexCount,CImageBuffer* bufTemp)
{
	//CApplication* pApp = (CApplication*)AfxGetApp();
	////CSize szImg = pApp->m_pTracks[nTrack].m_pDoc[0]->m_Buffer.GetImgSize();
	////CRect rectImg = CRect(CPoint(0, 0), szImg);
	//CAppDoc *pDoc = pApp->m_pTracks[0].m_pDoc[nDoc];
	//CInspection* pInsp = &pApp->m_pTracks[0].m_pDoc[0]->m_InspectionHandler[pDoc->m_pTrack->m_nFOVIndex].m_Inspection;

	//HString strType;
	//Hlong nWidth, nHeight;

	//BYTE* pBuf = (BYTE*) pInsp->m_arrayOverlay[nDoc][0].hImage.GetImagePointer1(&strType, &nWidth, &nHeight);
	//if(pBuf) {
	//	CSize szSrc = CSize((int) nWidth, (int) nHeight);
	//	CRect rectSrc = CRect(CPoint(0,0), szSrc);

	//	CImageBuffer bufZoomTemp;
	//	bufZoomTemp.ClrImg();
	//	bufZoomTemp.AllocImgIndirect(szSrc, pBuf);
	//	bufTemp->Allocate(szSrc);
	//	
	//	ImgCopy(&bufZoomTemp, &rectSrc, bufTemp, &rectSrc);
	////	m_ctrlPicture[nImgIndexCount].m_nResultFound = m_ImageHistory[nImgIndexCount].m_nResult;
	////	m_ctrlPicture[nImgIndexCount].m_bResultDisplay = TRUE;
	//}
}

int CImageSelectionDlg::NavigateDieImages(int nKey)
{
	CApplication* pApp = (CApplication*) AfxGetApp();

	/*if(!pApp->m_bLoadMultiImagesFromFile)
		return -1;
*/

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	CRect rect;
	int nPos, nDevice, nSel;
	BOOL bSelChange = FALSE;
	CString strToolTipTextNew;
	nDevice = nPosNew/ m_nNoOfFov;
	nPos = nPosNew % m_nNoOfFov;
	if(nKey == (int)VK_UP || nKey == (int)VK_DOWN) {
		BOOL bKeyUp = (nKey == (int)VK_UP);	
		if(bKeyUp) {
			if(nDevice > 0) {
				nDevice -= 1;
				bSelChange = TRUE;
			}
		}
		else {
			if(nDevice < m_nNoOfDies-1) {
				nDevice += 1;
				bSelChange = TRUE;
			}
		}	
	}
	else if(nKey == (int)VK_LEFT || nKey == (int)VK_RIGHT) {
		BOOL bKeyLeft = (nKey == (int)VK_LEFT);
		if(bKeyLeft) {
			if(nPos>0) {
				nPos -= 1;
				bSelChange = TRUE;
			}
		}
		else {
			if(nPos< m_nNoOfFov-1) {
				nPos += 1;
				bSelChange = TRUE;
			}
		}
		if(bSelChange)
			nPosNew = ((nDevice*m_nNoOfFov)+ (nPos% m_nNoOfFov));
	}

	if(bSelChange && (nPosOld > -1)) {
		int nOldDevice = nPosOld/ m_nNoOfFov;
		nSel = ((nOldDevice*m_nNoOfFov) + (nPosOld% m_nNoOfFov));
		m_ctrlDieImages[nSel].m_bSelected = FALSE;
		m_ctrlDieImages[nSel].Invalidate();

		strToolTipTextNew.Format("Row %d, Column %d: %d", nDevice+1, nPos+1,pApp->m_nPartErrorCode[nDevice]);
		GetDlgItem(IDC_STATIC_DISPLAY_MSG)->SetWindowTextA(strToolTipTextNew);
		CStringArray strArray;
		pApp->m_pTracks[0].m_pDoc[0]->GetImageFilePaths(m_pApp->m_strLoadMultiImagesFolderPath+"\\", strArray);
		for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++)
			pApp->m_pTracks[nTrack].m_pDoc[0]->LoadMultipleImagesToDoc(strArray, nPos, nDevice);
		//for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++)
			//pApp->m_pTracks[nTrack].m_pDoc[0]->Inspect(0);

		nSel = ((nDevice*m_nNoOfFov)+ (nPosNew % m_nNoOfFov));
		m_ctrlDieImages[nSel].m_bSelected = !m_ctrlDieImages[nSel].m_bSelected;
		m_ctrlDieImages[nSel].Invalidate();
		nPosOld = nPosNew = nSel;
	}

	return 0;
}

UINT LButtonDownThread(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CImageSelectionDlg* pImgSelDlg = (CImageSelectionDlg*) pParam;	
	CTrackHandler* pTrack = &pApp->m_pTracks[0];
	CAppDoc* pDoc = pTrack->m_pDoc[0];
	int nError =0;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	while(TRUE) {
		WaitForSingleObject(pImgSelDlg->m_hPicCtrlImgHistory, INFINITE);
		
		if(pImgSelDlg->bCloseLButtonThread)
			return 0;

		int nSel = -1;
		CRect rect;
		for(int nPos=0; nPos<pImgSelDlg->nMaxNoDisplay; nPos++) {
			//pImgSelDlg->GetDlgItem(IDC_STATIC_IMG_SEL1 + nPos)->GetWindowRect(&rect);
			pImgSelDlg->m_ctrlDieImages[nPos].GetWindowRect(&rect);      
			pImgSelDlg->ScreenToClient(&rect);
			if(rect.PtInRect(pImgSelDlg->ptLButtonDownPoint) && pImgSelDlg->m_ctrlDieImages[nPos].IsWindowVisible()) {
				pImgSelDlg->nPosNew = nSel = nPos;


				break;
			}
		}

		if(nSel > -1) {
			BOOL bIsStoredEnabled = FALSE;
			BOOL bIsStepEnabled = FALSE;
			BOOL bIsStepDebugEnabled = FALSE;
			for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
				bIsStoredEnabled |= pApp->m_pTracks[nTrack].m_pDoc[0]->m_bStoredImages;
				bIsStepDebugEnabled |= pApp->m_pTracks[nTrack].m_pDoc[0]->m_bInspectionDebug;
			}
			if (!bIsStepEnabled /*&& !bIsStoredEnabled */&& !bIsStepDebugEnabled &!pApp->m_bTeaching) {
				if (pImgSelDlg->nPosOld > -1) {
					pImgSelDlg->m_ctrlDieImages[pImgSelDlg->nPosOld].m_bSelected = FALSE;
					pImgSelDlg->m_ctrlDieImages[pImgSelDlg->nPosOld].Invalidate();
					pImgSelDlg->bFovSelected[pImgSelDlg->nPosOld] = FALSE;
					pApp->m_nSaveAllDieIndex = -1;
					pImgSelDlg->Invalidate();
				}
				if (/*pImgSelDlg->nPosOld != nSel*/!pApp->m_bUpdatingMapping && !pApp->m_bScanning) {
					pImgSelDlg->nPosOld = nSel;

					int nFov, nColumn, nRow, nDevice;
					pApp->motionParameters.getImageMappingDetails(nSel, nRow, nColumn, nDevice, nFov);
			
					for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
						pApp->m_pTracks[nTrack].m_nFOVIndex = nFov;
					}


					if (pApp->m_bLoadMultiImagesFromFile && !((pApp->m_bOnline && pApp->m_bShowDieVerification) || pApp->m_bInspSequenceOffline)) {
						for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
							pApp->m_pTracks[nTrack].m_pDoc[0]->LoadMultipleImagesToDoc(pApp->m_strLoadMultiImagesArray, nFov, nDevice);
						}
					}
					else if (!pApp->m_bInspSequenceOffline || pApp->m_bOnline) {
						//Get Station Id from Centralized Queue
						int nStationId = -1;
						EnterCriticalSection(&theApp.m_csCentralizedQueue);
						if (!theApp.m_CentralizedStationQueue.empty()) {
							nStationId = theApp.m_CentralizedStationQueue.front();
						}
						LeaveCriticalSection(&theApp.m_csCentralizedQueue);


						for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
							pApp->m_pTracks[nTrack].m_nFOVIndex = nFov;
							for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++){
								pApp->m_pTracks[nTrack].m_pDoc[nDoc]->DrawView(0);
								pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.ClrImg();
								if (pApp->motionParameters.IsGrabEnabled(nTrack, nDoc, nFov) || pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
									int nXSize, nYSize;
									nXSize = pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize().cx;
									nYSize = pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize().cy;
									CRect rect = CRect(0, 0, nXSize, nYSize);
									if (pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
										if (nStationId != -1) {
											ImgCopy(&pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_CentralizedBufferDie[nStationId][nDevice][nFov], &rect, &pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect);
										}
										else if(pApp->m_nCentralizedStationLatest != -1){
											ImgCopy(&pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_CentralizedBufferDie[pApp->m_nCentralizedStationLatest][nDevice][nFov], &rect, &pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect);
										}
									}
									else {
										ImgCopy(&pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov], &rect, &pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect);

									}
									pApp->m_pTracks[nTrack].m_pDoc[nDoc]->DrawView(1);
								}
								
							}
							//	if(pApp->m_bShowDieVerification)
							pApp->m_pTracks[nTrack].m_pDoc[0]->m_lDebugFlag &= ~DEBUG_STEP_MODE;
							pApp->m_bDisplayAllStatsOffline = FALSE;
							if (!pApp->m_CentralizedCommInterface.m_bIsTCPServer && pApp->m_bIsMasterApp)
								pApp->m_pTracks[nTrack].m_pDoc[0]->InspectAllDoc(nError);
							else {
								if(nStationId != -1)
									pApp->m_pTracks[nTrack].m_pDoc[0]->InspectionResultAllDoc(nStationId, nDevice);
							}
								

							//Show the fail doc
							int nPriorityErrorCode = pApp->m_nDefectErrorCodesPriority.GetCount();
							int nSelectedView = -1;
							for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
								int nView;
								pApp->motionParameters.GetViewID(nTrack, nDoc, nView);
								int nErrorCode = pApp->m_nErrorCode[pApp->m_pTracks[nTrack].m_pDoc[0]->m_pTrack->m_nDieIndex][nFov][nView];
								if (nErrorCode != GOOD_UNIT && nErrorCode != NOT_INSPECTED) {
									if (pApp->m_mapDefectErrorCodesPriority.find(nErrorCode) != pApp->m_mapDefectErrorCodesPriority.end()) {
										if (pApp->m_mapDefectErrorCodesPriority[nErrorCode] < nPriorityErrorCode) {
											nPriorityErrorCode = pApp->m_mapDefectErrorCodesPriority[nErrorCode];
											nSelectedView = nView;
										}
									}
								}
							}
							if(nSelectedView != -1)
								pApp->m_pTracks[nTrack].m_pDoc[0]->DisplaySelectedDocView(nSelectedView);
						}
					}

					pApp->m_nSaveAllDieIndex = nDevice;
					pImgSelDlg->m_ctrlDieImages[nSel].m_bSelected = !pImgSelDlg->m_ctrlDieImages[nSel].m_bSelected;
					pImgSelDlg->bFovSelected[nSel] = TRUE;
					pImgSelDlg->Invalidate();

					CString strTemp;
					strTemp.Format("%d%d", pApp->m_nActiveDocIdx, nFov);
					pApp->m_pMainWnd->SendMessageA(WM_SET_CUR_POS_PARAM_DISP, (WPARAM)pApp->m_nActiveTrackIdx, (LPARAM)&strTemp);
				}
			
			}
		}
	}
	return 0;
}

void CImageSelectionDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages
	CDC* pDC = &dc;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	int nTemp = 0;

	for (int nDie = 0;nDie < m_nNoOfDies; nDie++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			CBrush tempBrush;
			int nPosition;
			theApp.motionParameters.getImageMappingDetails(nDie, nFov, nTemp);


			tempBrush.CreateSolidBrush(getInfo(getId(m_pApp->m_nFovPartErrorCode[nDie][nFov]))->color);
			pDC->FillRgn(&m_prgnDieMapPos[nTemp], &tempBrush);
			if(bFovSelected[nTemp])
				pDC->FrameRgn(&m_prgnDieMapPos[nTemp], &m_brushBorderSelected, 4, 4);
			else
				pDC->FrameRgn(&m_prgnDieMapPos[nTemp], &m_brushBorder, 2, 2);
		//	nTemp++;
		}

		pDC->FrameRgn(&m_prgnDieBounday[nDie], &m_brushDeviceBorder, 2, 2);
	}

	/*for (int y = 0; y<nMaxNoDisplay;y++) {
		pDC->FillRgn(&m_prgnDieMapPos[y], &m_brushUncheck);
		pDC->FrameRgn(&m_prgnDieMapPos[y], &m_brushBorder, 2, 2);
	}*/
}

void CImageSelectionDlg::EnableDieRect(int nPos,int nDevice)
{
	int nSel;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	nSel = (nDevice* nDevicePerX)+nPos;
	m_ctrlDieImages[nSel].m_bSelected = TRUE;
	bFovSelected[nSel] = TRUE;
	m_ctrlDieImages[nSel].Invalidate();
	if (nPosOld > -1) {
		m_ctrlDieImages[nPosOld].m_bSelected = FALSE;
		bFovSelected[nPosOld] = FALSE;
		m_ctrlDieImages[nPosOld].Invalidate();
		Invalidate();
	}
	nPosOld = nSel;
	for (int nTrack = 0; nTrack<m_pApp->m_nTotalTracks; nTrack++)
		m_pApp->m_pTracks[nTrack].m_pDoc[0]->LoadMultipleImagesToDoc(m_pApp->m_strLoadMultiImagesArray, nPos, nDevice);

	Invalidate();
}

void CImageSelectionDlg::EnableVerification(BOOL bEnable)
{
	CString str;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	if (bEnable) {
		for (int i = 0; i < m_nNoOfDies; i++) {
			m_bCheckPass[i] = FALSE;
			str.Format("%s : %s", m_pApp->m_strPartFlex[i], getInfo(getId(m_pApp->m_nPartErrorCode[i]))->code.c_str());
			GetDlgItem(IDC_STATIC_PART1 + i)->SetWindowTextA(str);
			if (m_pApp->m_nPartErrorCode[i] != GOOD_UNIT/*m_pApp->m_pDefectCode[0].nErrorCode*/) {
				bColorFailPass[i] = FALSE;
				bColorVerifyFailPass[i] = FALSE;
			}
			else {
				bColorFailPass[i] = TRUE;
				bColorVerifyFailPass[i] = TRUE;
			}
			GetDlgItem(IDC_STATIC_PART1 + i)->ShowWindow(bEnable);
			//if(m_pApp->m_bPartInspect[nDie])
			GetDlgItem(IDC_CHECK_PART_1 + i)->ShowWindow(bEnable);
			GetDlgItem(IDC_CHECK_PART_1 + i)->EnableWindow(m_pApp->m_bPartInspect[i]);
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->ShowWindow(bEnable);
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->EnableWindow(FALSE);

			if (m_pApp->m_bCentralizedVerificationMode && m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
				//Get Station Id from Centralized Queue
				int nStationId = -1;
				EnterCriticalSection(&m_pApp->m_csCentralizedQueue);
				if (!m_pApp->m_CentralizedStationQueue.empty()) {
					nStationId = m_pApp->m_CentralizedStationQueue.front();
				}
				LeaveCriticalSection(&m_pApp->m_csCentralizedQueue);

				if (nStationId >= 0 && nStationId < NO_OF_PORT_CENTRALIZED) {
					GetDlgItem(IDC_STATIC_CLIENT_ID)->SetWindowTextA("Machine : " + m_pApp->m_strClientMachineID[nStationId]);
					GetDlgItem(IDC_STATIC_CLIENT_ID)->ShowWindow(bEnable);
				}
				
			}
		}
	}
	else
	{
		for (int i = 0; i < m_nNoOfDies; i++) {
			GetDlgItem(IDC_STATIC_PART1 + i)->ShowWindow(bEnable);
			GetDlgItem(IDC_CHECK_PART_1 + i)->ShowWindow(bEnable);
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->ShowWindow(bEnable);
		}
		GetDlgItem(IDC_STATIC_CLIENT_ID)->ShowWindow(bEnable);
	}
	
	GetDlgItem(IDC_BUTTON_RESULT_CONFIRM)->ShowWindow(bEnable);
	GetDlgItem(IDC_BUTTON_RESCAN)->ShowWindow(bEnable);

	Invalidate();
	if(bEnable)
		UpdateData(FALSE);
}

void CImageSelectionDlg::OnBnClickedButtonResultConfirm()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	BOOL bSendResultConfirm = FALSE;
	if (m_bCheckPass[0] || m_bCheckPass[1] || m_bCheckPass[2] || m_bCheckPass[3] || m_bCheckPass[4]) {
		bSendResultConfirm = TRUE;
	}
	else {
		bSendResultConfirm = AfxMessageBox("Confirm To Send The Result", MB_YESNO) == IDYES;
	}

	if (bSendResultConfirm) {
		for (int i = 0; i < m_nNoOfDies; i++) {
			if (m_bCheckPass[i]) {
				CString strMsg;
				strMsg.Format("%s THA Error code: %s  modified to: %s", m_pApp->m_strPartFlex[i], 
								getInfo(getId(m_pApp->m_nPartErrorCode[i]))->code.c_str(), getInfo(getId(m_nKeyInErrorCode[i]))->code.c_str());
				m_pApp->AddLogEvent(strMsg);

				if (m_pApp->m_nPartErrorCode[i] != m_nKeyInErrorCode[i]) {
					//False Acceptance
					if (m_pApp->m_nPartErrorCode[i] == GOOD_UNIT) {
						for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
							for (int nView = 0; nView < m_pApp->m_nTotalViews; nView++) {
								m_pApp->m_nErrorCode[i][nFov][nView] = -USER_MODIFIED;
							}
						}
						m_pApp->m_nDieErrorCodeStats[i] = -USER_MODIFIED;
					}
					else {
						//False Rejection
						if (m_nKeyInErrorCode[i] == GOOD_UNIT) {
							for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
								for (int nView = 0; nView < m_pApp->m_nTotalViews; nView++) {
									m_pApp->m_nErrorCode[i][nFov][nView] = GOOD_UNIT;
								}
							}
							m_pApp->m_nDieErrorCodeStats[i] = -USER_MODIFIED_FALSE_REJECTION;
						}
						else {
							m_pApp->m_nDieErrorCodeStats[i] = m_nKeyInErrorCode[i];
						}
					}

					m_pApp->m_nPartErrorCode[i] = m_nKeyInErrorCode[i];
				}
			}
		}
		m_pApp->m_bShowDieVerification = FALSE;

		//Send verification result to client so that they can send back the final result to HVFA Comm
		if (m_pApp->m_bCentralizedVerificationMode && m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
			//Get Station Id from Centralized Queue
			int nStationId = -1;
			EnterCriticalSection(&m_pApp->m_csCentralizedQueue);
			if (!m_pApp->m_CentralizedStationQueue.empty()) {
				nStationId = m_pApp->m_CentralizedStationQueue.front();
			}
			LeaveCriticalSection(&m_pApp->m_csCentralizedQueue);

			if (nStationId >=0 && nStationId < NO_OF_PORT_CENTRALIZED) {
				int nError = m_pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->SendMappingResult(VERIFICATION_RESULT);
				if (nError) {
					m_pApp->m_bShowDieVerification = TRUE;
					CString strMsg;
					strMsg.Format("Can not send verification result to %s...Please check connection!!!", 
						m_pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_Monitor.GetClientAcceptedMachineID());

					AfxMessageBox(strMsg);
				}
				else {
					m_pApp->ClearResults();
					SetEvent(m_pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_eventVerificationDone);
					//m_pApp->OnGooffline();
				}
			}
		}
		else
			SetEvent(m_pApp->m_EventCommSendMsg);

		if (m_pApp->m_bStream)
			m_pApp->m_bStream = FALSE;

		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
				for (int nView = 0; nView < m_pApp->m_nTotalViews; nView++) {
					if (m_pApp->m_bPartInspect[nDie])
						m_pApp->UpdateLotSummary(nDie, nFov, nView, m_pApp->m_nErrorCode[nDie][nFov][nView]);
				}
			}
			m_pApp->UpdateTotalLotSummary(nDie, m_pApp->m_nDieErrorCodeStats[nDie]);
		}
		m_pApp->UpdateStatisticsWindow();
		EnableVerification(m_pApp->m_bShowDieVerification);
	}
	else
		return;
}

void CImageSelectionDlg::OnBnClickedButtonRescan()
{
	m_pApp->m_bShowDieVerification = FALSE;
	if (theApp.m_bCentralizedVerificationMode && theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
		CString strMessage, str;

		strMessage = "Vision: RESCAN";

		//Get Station Id from Centralized Queue
		int nStationId = -1;
		EnterCriticalSection(&m_pApp->m_csCentralizedQueue);
		if (!m_pApp->m_CentralizedStationQueue.empty()) {
			nStationId = m_pApp->m_CentralizedStationQueue.front();
		}
		LeaveCriticalSection(&m_pApp->m_csCentralizedQueue);

		if (nStationId >= 0 && nStationId < NO_OF_PORT_CENTRALIZED) {
			int nSuccess = theApp.m_CentralizedCommInterface.m_pCommManager[nStationId]->SendDataMessage(RESCAN, strMessage);
			if (!nSuccess) {
				m_pApp->m_bShowDieVerification = TRUE;
				CString strMsg;
				strMsg.Format("Can not send Re-scan request to %s...Please check connection!!!",
								m_pApp->m_CentralizedCommInterface.m_pCommManager[nStationId]->m_Monitor.GetClientAcceptedMachineID());
				AfxMessageBox(strMsg);
			}
			else {
				m_pApp->ClearResults();
				SetEvent(theApp.m_CentralizedCommInterface.m_pCommManager[nStationId]->m_eventVerificationDone);
				//m_pApp->OnGooffline();
			}
		}
	}
	else {
		m_pApp->m_bReScanSequence = TRUE;

		if (m_pApp->m_bStream)
			m_pApp->m_bStream = FALSE;
	}
	
	EnableVerification(m_pApp->m_bShowDieVerification);
}


HBRUSH CImageSelectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	m_pBrush = &m_pBrushTemp[3];

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	for (int i = 0; i < m_nNoOfDies; i++) {
		if (m_pApp->m_bPartInspect[i]) {
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_PART1 + i) {
				m_pBrush = &m_pBrushTemp[bColorFailPass[i]];
			}

			if (pWnd->GetDlgCtrlID() == IDC_EDIT_ERROR_CODE_PART1 + i) {
				if (m_bCheckPass[i]) {
					m_pBrush = &m_pBrushTemp[bColorVerifyFailPass[i]];
				}
			}
		}
		else
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_PART1 + i) {
				m_pBrush = &m_pBrushTemp[2];
			}
	}
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 0, 0));

	return *m_pBrush;
}


void CImageSelectionDlg::OnBnClickedCheckPart1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//GetDlgItem(IDC_EDIT_ERROR_CODE_PART1)->EnableWindow(m_bCheckPass[0]);
	if (m_bCheckPass[0]) {
		CDefectErrorCodeSelection dlg;
		if (dlg.DoModal() == IDOK) {
			m_nKeyInErrorCode[0] = dlg.m_nSelectedErrorCode;
			m_strKeyInErrorCode[0].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART1)->SetWindowTextA(m_strKeyInErrorCode[0]);

			if (m_nKeyInErrorCode[0] == GOOD_UNIT)
				bColorVerifyFailPass[0] = TRUE;
			else
				bColorVerifyFailPass[0] = FALSE;

			int nIndex = 0;
			if (dlg.m_bIsConfirm) {
				OnBnClickedButtonResultConfirm();
			}
			else if (dlg.m_bIsApplyAllDevice) {
				int nDevicePerX, nDevicePerY;
				int nFovPerX, nFovPerY;
				m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
				int m_nNoOfFov = nFovPerX * nFovPerY;
				int m_nNoOfDies = nDevicePerX * nDevicePerY;
				for (int i = 0; i < m_nNoOfDies; i++) {
					if (i != nIndex) {
						m_nKeyInErrorCode[i] = dlg.m_nSelectedErrorCode;
						m_strKeyInErrorCode[i].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
						GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->SetWindowTextA(m_strKeyInErrorCode[i]);

						if (m_nKeyInErrorCode[i] == GOOD_UNIT)
							bColorVerifyFailPass[i] = TRUE;
						else
							bColorVerifyFailPass[i] = FALSE;

						m_bCheckPass[i] = TRUE;
					}
				}
			}
		}
		else
		{
			m_bCheckPass[0] = FALSE;
		}
	}
	UpdateData(FALSE);
	Invalidate();
}


void CImageSelectionDlg::OnBnClickedCheckPart2()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	//GetDlgItem(IDC_EDIT_ERROR_CODE_PART2)->EnableWindow(m_bCheckPass[1]);
	if (m_bCheckPass[1]) {
		CDefectErrorCodeSelection dlg;
		if (dlg.DoModal() == IDOK) {
			m_nKeyInErrorCode[1] = dlg.m_nSelectedErrorCode;
			m_strKeyInErrorCode[1].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART2)->SetWindowTextA(m_strKeyInErrorCode[1]);

			if (m_nKeyInErrorCode[1] == GOOD_UNIT)
				bColorVerifyFailPass[1] = TRUE;
			else
				bColorVerifyFailPass[1] = FALSE;

			int nIndex = 1;
			if (dlg.m_bIsConfirm) {
				OnBnClickedButtonResultConfirm();
			}
			else if (dlg.m_bIsApplyAllDevice) {
				int nDevicePerX, nDevicePerY;
				int nFovPerX, nFovPerY;
				m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
				int m_nNoOfFov = nFovPerX * nFovPerY;
				int m_nNoOfDies = nDevicePerX * nDevicePerY;
				for (int i = 0; i < m_nNoOfDies; i++) {
					if (i != nIndex) {
						m_nKeyInErrorCode[i] = dlg.m_nSelectedErrorCode;
						m_strKeyInErrorCode[i].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
						GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->SetWindowTextA(m_strKeyInErrorCode[i]);

						if (m_nKeyInErrorCode[i] == GOOD_UNIT)
							bColorVerifyFailPass[i] = TRUE;
						else
							bColorVerifyFailPass[i] = FALSE;

						m_bCheckPass[i] = TRUE;
					}
				}
			}
		}
		else
		{
			m_bCheckPass[1] = FALSE;
		}
	}
	UpdateData(FALSE);
	Invalidate();
}


void CImageSelectionDlg::OnBnClickedCheckPart3()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	//GetDlgItem(IDC_EDIT_ERROR_CODE_PART3)->EnableWindow(m_bCheckPass[2]);
	if (m_bCheckPass[2]) {
		CDefectErrorCodeSelection dlg;
		if (dlg.DoModal() == IDOK) {
			m_nKeyInErrorCode[2] = dlg.m_nSelectedErrorCode;
			m_strKeyInErrorCode[2].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART3)->SetWindowTextA(m_strKeyInErrorCode[2]);

			if (m_nKeyInErrorCode[2] == GOOD_UNIT)
				bColorVerifyFailPass[2] = TRUE;
			else
				bColorVerifyFailPass[2] = FALSE;

			int nIndex = 2;
			if (dlg.m_bIsConfirm) {
				OnBnClickedButtonResultConfirm();
			}
			else if (dlg.m_bIsApplyAllDevice) {
				int nDevicePerX, nDevicePerY;
				int nFovPerX, nFovPerY;
				m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
				int m_nNoOfFov = nFovPerX * nFovPerY;
				int m_nNoOfDies = nDevicePerX * nDevicePerY;
				for (int i = 0; i < m_nNoOfDies; i++) {
					if (i != nIndex) {
						m_nKeyInErrorCode[i] = dlg.m_nSelectedErrorCode;
						m_strKeyInErrorCode[i].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
						GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->SetWindowTextA(m_strKeyInErrorCode[i]);

						if (m_nKeyInErrorCode[i] == GOOD_UNIT)
							bColorVerifyFailPass[i] = TRUE;
						else
							bColorVerifyFailPass[i] = FALSE;

						m_bCheckPass[i] = TRUE;
					}
				}
			}
		}
		else
		{
			m_bCheckPass[2] = FALSE;
		}
	}
	UpdateData(FALSE);
	Invalidate();
}


void CImageSelectionDlg::OnBnClickedCheckPart4()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	//GetDlgItem(IDC_EDIT_ERROR_CODE_PART4)->EnableWindow(m_bCheckPass[3]);
	if (m_bCheckPass[3]) {
		CDefectErrorCodeSelection dlg;
		if (dlg.DoModal() == IDOK) {
			m_nKeyInErrorCode[3] = dlg.m_nSelectedErrorCode;
			m_strKeyInErrorCode[3].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART4)->SetWindowTextA(m_strKeyInErrorCode[3]);

			if (m_nKeyInErrorCode[3] == GOOD_UNIT)
				bColorVerifyFailPass[3] = TRUE;
			else
				bColorVerifyFailPass[3] = FALSE;

			int nIndex = 3;
			if (dlg.m_bIsConfirm) {
				OnBnClickedButtonResultConfirm();
			}
			else if (dlg.m_bIsApplyAllDevice) {
				int nDevicePerX, nDevicePerY;
				int nFovPerX, nFovPerY;
				m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
				int m_nNoOfFov = nFovPerX * nFovPerY;
				int m_nNoOfDies = nDevicePerX * nDevicePerY;
				for (int i = 0; i < m_nNoOfDies; i++) {
					if (i != nIndex) {
						m_nKeyInErrorCode[i] = dlg.m_nSelectedErrorCode;
						m_strKeyInErrorCode[i].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
						GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->SetWindowTextA(m_strKeyInErrorCode[i]);

						if (m_nKeyInErrorCode[i] == GOOD_UNIT)
							bColorVerifyFailPass[i] = TRUE;
						else
							bColorVerifyFailPass[i] = FALSE;

						m_bCheckPass[i] = TRUE;
					}
				}
			}
		}
		else
		{
			m_bCheckPass[3] = FALSE;
		}
	}
	UpdateData(FALSE);
	Invalidate();
}


void CImageSelectionDlg::OnBnClickedCheckPart5()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	//GetDlgItem(IDC_EDIT_ERROR_CODE_PART5)->EnableWindow(m_bCheckPass[4]);
	if (m_bCheckPass[4]) {
		CDefectErrorCodeSelection dlg;
		if (dlg.DoModal() == IDOK) {
			m_nKeyInErrorCode[4] = dlg.m_nSelectedErrorCode;
			m_strKeyInErrorCode[4].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());;
			GetDlgItem(IDC_EDIT_ERROR_CODE_PART5)->SetWindowTextA(m_strKeyInErrorCode[4]);

			if (m_nKeyInErrorCode[4] == GOOD_UNIT)
				bColorVerifyFailPass[4] = TRUE;
			else
				bColorVerifyFailPass[4] = FALSE;

			int nIndex = 4;
			if (dlg.m_bIsConfirm) {
				OnBnClickedButtonResultConfirm();
			}
			else if (dlg.m_bIsApplyAllDevice) {
				int nDevicePerX, nDevicePerY;
				int nFovPerX, nFovPerY;
				m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
				int m_nNoOfFov = nFovPerX * nFovPerY;
				int m_nNoOfDies = nDevicePerX * nDevicePerY;
				for (int i = 0; i < m_nNoOfDies; i++) {
					if (i != nIndex) {
						m_nKeyInErrorCode[i] = dlg.m_nSelectedErrorCode;
						m_strKeyInErrorCode[i].Format("%s", getInfo(getId(dlg.m_nSelectedErrorCode))->code.c_str());
						GetDlgItem(IDC_EDIT_ERROR_CODE_PART1 + i)->SetWindowTextA(m_strKeyInErrorCode[i]);

						if (m_nKeyInErrorCode[i] == GOOD_UNIT)
							bColorVerifyFailPass[i] = TRUE;
						else
							bColorVerifyFailPass[i] = FALSE;

						m_bCheckPass[i] = TRUE;
					}
				}
			}
		}
		else
		{
			m_bCheckPass[4] = FALSE;
		}
	}
	UpdateData(FALSE);
	Invalidate();
}

BOOL CImageSelectionDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialogEx::PreTranslateMessage(pMsg);
}

INT_PTR CImageSelectionDlg::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
	pTI->hwnd = m_hWnd;
	pTI->uFlags = 0; // we need to differ tools by ID not window handle
	pTI->lpszText = LPSTR_TEXTCALLBACK; // tell tooltips to send TTN_NEEDTEXT	
	int iIndx;
	CRect rect;
	for (iIndx = 0; iIndx < nMaxNoDisplay; iIndx++)
	{
		m_ctrlDieImages[iIndx].GetWindowRect(&rect);
		ScreenToClient(&rect);
		if (rect.PtInRect(point))
		{
			// point is in this rectangle. 
			pTI->rect = rect;
			// TTN_NEEDTEXT Message will use this ID to fill NMHDR structure
			pTI->uId = iIndx;
			break;
		}
	}

	return iIndx; // return index of rectangle
}

void CImageSelectionDlg::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
{
	WCHAR m_szTipBufW[MAX_PATH];
	char m_szTipBufA[MAX_PATH];

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;


	*pResult = 1;
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	CString strToolTipTextNew;

	UINT uiID = pNMHDR->idFrom;

	int iID = GetDlgCtrlID();

	CString csIn;
	GetWindowText(csIn);
	
//	int nPos, nDevice;
//	nPos = uiID % m_nNoOfFov;
//	nDevice = uiID / m_nNoOfFov;

	int nFov, nColumn, nRow, nDevice;
	m_pApp->motionParameters.getImageMappingDetails(uiID, nRow, nColumn, nDevice, nFov);
//	m_pApp->motionParameters.getDieMappingFovNo(nFov, nFov);


	//if (m_pApp->m_bShowDieVerification)
	strToolTipTextNew.Format("Device %d, Fov %d, Row %d, Column %d :%s", nDevice + 1, nFov + 1, nRow + 1, nColumn + 1, getInfo(getId(m_pApp->m_nFovPartErrorCode[nDevice][nFov]))->code.c_str());
	//else
	//	strToolTipTextNew.Format("Row %d, Column %d", nDevice + 1, nPos + 1);


#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
	{
		lstrcpyn(m_szTipBufA, strToolTipTextNew, _countof(m_szTipBufA));
		pTTTA->lpszText = m_szTipBufA;
	}
	else
	{
		_mbstowcsz(m_szTipBufW, strToolTipTextNew, _countof(m_szTipBufW));
		pTTTW->lpszText = m_szTipBufW;
	}
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
	{
		_wcstombsz(m_szTipBufA, csToolText, _countof(m_szTipBufA));
		pTTTA->lpszText = m_szTipBufA;
	}
	else
	{
		lstrcpyn(m_szTipBufW, csToolText, _countof(m_szTipBufW));
		pTTTW->lpszText = m_szTipBufW;
	}
#endif
	//	return TRUE;
}

CString CImageSelectionDlg::GetDefectName(int nError)
{

	CString strDefectName;
	strDefectName = getInfo(getId(nError))->code.c_str();
	return strDefectName;
}

int CImageSelectionDlg::GetDefectID(int nErrorCode)
{
	int DefectID = getInfo(getId(nErrorCode))->id;
	return DefectID;
}

ItemInfo CImageSelectionDlg::GetDefectCode(int nIndex)
{
	 return getDefect(nIndex);
}


IMPLEMENT_DYNAMIC(CDialogPane,CDockablePane)
BEGIN_MESSAGE_MAP(CDialogPane,CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CDialogPane::CDialogPane(){}
CDialogPane::~CDialogPane(){}

int CDialogPane::OnCreate(LPCREATESTRUCT lp)
{
	
	if(CDockablePane::OnCreate(lp)==-1)
		return -1;
	if(!m_wndDlg.Create(CImageSelectionDlg::IDD,this))
		return -1;
	bIsClosed = FALSE;
	bHideEnable = FALSE;
	m_wndDlg.Initialize();
	m_wndDlg.ShowWindow(SW_SHOWDEFAULT);
	return  0;
}
void CDialogPane::OnSize(UINT nType,int cx,int cy)
{
	CDockablePane::OnSize(nType,cx,cy);
	//m_wndDlg.SetSize(cx,cy);
	m_wndDlg.SetSize(200,200);
	m_wndDlg.SetWindowPos(NULL,0,0,cx,cy,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndDlg.Invalidate();
}


void CDialogPane::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();
	bIsClosed = TRUE;
}



void CDialogPane::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	return;
}