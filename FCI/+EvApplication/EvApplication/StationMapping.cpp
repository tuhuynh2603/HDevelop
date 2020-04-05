// StationMapping.cpp : implementation file
//

#include "stdafx.h"
#include "StationMapping.h"
#include "afxdialogex.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "LightHandler.h"
#include "MotionControlDialog.h"
#include "LightHandler.h"
#include "LightControl.h"
#include "MotionParameters.h"

// CStationMapping dialog

static const UINT_PTR STATION_MAPPING_TIMER = 54;


IMPLEMENT_DYNAMIC(CStationMapping, CDialogEx)

CStationMapping::CStationMapping(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStationMapping::IDD, pParent)
	, _motorId(0)
{
	pApp = (CApplication*) AfxGetApp();

	m_prgnPositionSelection = NULL;
	m_bPosSelection = NULL;
	m_bDieMapPosSel = NULL;

	m_diePicCtrl = NULL;
	m_brushCheck.CreateSolidBrush(RGB(20,255,0));
	m_brushUncheck.CreateSolidBrush(RGB(125,125,120));
	m_brushBorder.CreateSolidBrush(RGB(0,0,0));
	
	//m_dZMotorRelativeMoveDisp = 0.0;
	m_strZMotorRelativeMoveDisp.Format("0.0");
	m_nZMotorPosDisp = 0;
	m_dZMotorScanMaxDisp = 0.0;

	for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
		m_dZMotorRelativeMove[nTrack] = 0.0;
		m_nZMotorPos[nTrack] = 0;
		m_dZMotorScanMax[nTrack] = 0.0;
		m_bRelativeCheck[nTrack] = TRUE;//FALSE;
	}
//	m_bPosSelection = new BOOL[MAX_DIE_FOVS];

//	brushCell[0].CreateSolidBrush(RGB(64, 64, 64)); // Available to use
//	brushCell[1].CreateSolidBrush(RGB(200, 200, 200)); // Not available
//	brushCell[2].CreateSolidBrush(RGB(0, 240, 0)); // Selected


	brushBackground.CreateSolidBrush(RGB(220, 220, 220));

	brushSelectedCell.CreateSolidBrush(RGB(0, 255, 0));

//	prcDie = NULL;
	nMouseSelectedDie = -1;

	nZoom = 1;
	nPrevZoom = 0;

	bRedrawing = FALSE;

	scrollBarDeviceHRect = CRect();
	scrollBarDeviceVRect = CRect();

	scrollBarDieHRect = CRect();
	scrollBarDieVRect = CRect();
	m_nStartScanFOV = 0;

}

CStationMapping::~CStationMapping()
{
	if(m_bDieMapPosSel != NULL)
		delete[] m_bDieMapPosSel;

	if(m_bPosSelection != NULL)
		delete[] m_bPosSelection;

	m_brushCheck.DeleteObject();
	m_brushUncheck.DeleteObject();
	m_brushBorder.DeleteObject();

	if(m_prgnPositionSelection != NULL) {
//		for(int i=0; i<m_nDeviceSelectionCount; i++)
//			m_prgnPositionSelection[i].DeleteObject();
		delete[] m_prgnPositionSelection;
	}
	m_prgnPositionSelection = NULL;


	if (m_diePicCtrl != NULL)
		delete[] m_diePicCtrl;
	m_diePicCtrl = NULL;
}

void CStationMapping::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_H_SCROLLBAR, scrollBarDeviceH);
//	DDX_Control(pDX, IDC_V_SCROLLBAR, scrollBarDeviceV);
//	DDX_Control(pDX, IDC_H_SCROLLBAR2, scrollBarDieH);
//	DDX_Control(pDX, IDC_V_SCROLLBAR2, scrollBarDieV);
	DDX_Control(pDX, IDC_SM_CAMID_COMBO, m_ctrlCameraID);
	DDX_Control(pDX, IDC_SM_DIE_MAP_LIST, m_ctrlZLevelMap);
	DDX_Control(pDX, IDC_SM_DIE_MAP_POSID_LIST, m_ctrlDieZLevelID);
	//DDX_Text(pDX, IDC_SM_Z_MTR_SERVO_EDIT, m_dZMotorRelativeMoveDisp);
	DDX_Text(pDX, IDC_SM_Z_MTR_SERVO_EDIT, m_strZMotorRelativeMoveDisp);
	DDX_Text(pDX, IDC_SM_START_EDIT, m_nZMotorPosDisp);
	DDX_Control(pDX, IDC_SM_START_EDIT, ctrlMotorPos);
//	DDX_Text(pDX, IDC_SM_MAX_EDIT, m_dZMotorScanMaxDisp);
	DDX_Control(pDX, IDC_MOTOR_ID_COMBO, _motorIdCombobox);
	DDX_CBIndex(pDX, IDC_MOTOR_ID_COMBO, _motorId);
	DDX_Control(pDX, IDC_SM_Z_MTR_SERVO_DOWN_MFCBUTTON, _negativeMovementButton);
	DDX_Control(pDX, IDC_SM_Z_MTR_SERVO_UP_MFCBUTTON, _positiveMovementButton);
}


BEGIN_MESSAGE_MAP(CStationMapping, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_VKEYTOITEM()
	ON_WM_TIMER()
	ON_WM_CLOSE()

	ON_CBN_SELCHANGE(IDC_SM_CAMID_COMBO, &CStationMapping::OnSelchangeSmCamidCombo)
	ON_BN_CLICKED(IDC_SM_SELECTALL_BUTTON, &CStationMapping::OnBnClickedSmSelectallButton)	

	ON_BN_CLICKED(IDC_SM_DIE_MAP_ADD_MFCBUTTON, &CStationMapping::OnBnClickedSmDieMapAddMfcbutton)
	ON_BN_CLICKED(IDC_SM_DIE_MAP_REMOVE_MFCBUTTON, &CStationMapping::OnBnClickedSmDieMapRemoveMfcbutton)
	ON_BN_CLICKED(IDC_SM_DIE_MAP_RETAKE_MFCBUTTON, &CStationMapping::OnBnClickedSmDieMapRetakeMfcbutton)
	//ON_LBN_DBLCLK(IDC_SM_DIE_MAP_LIST, &CStationMapping::OnDblclkSmDieMapList)
	ON_LBN_SELCHANGE(IDC_SM_DIE_MAP_LIST, &CStationMapping::OnSelchangeSmDieMapList)
	ON_BN_CLICKED(IDC_SM_DIE_MAP_SWAP_MFCBUTTON, &CStationMapping::OnBnClickedSmDieMapSwapMfcbutton)
	ON_BN_CLICKED(IDC_SM_DIE_MAP_POSMOVE_SETLIGHT_BUTTON, &CStationMapping::OnBnClickedSmDieMapPosmoveSetlightButton)
	ON_BN_CLICKED(IDC_SM_DIE_MAP_POSID_REMOVE_MFCBUTTON, &CStationMapping::OnBnClickedSmDieMapPosidRemoveMfcbutton)
	ON_BN_CLICKED(IDC_SM_DIE_MAP_LIVECAM_LIGHT_BUTTON, &CStationMapping::OnBnClickedSmDieMapLivecamLightButton)

	ON_BN_CLICKED(IDC_SM_SAVE_ALL_TEACH_BUTTON, &CStationMapping::OnBnClickedSmSaveAllTeachButton)
	ON_BN_CLICKED(IDC_SM_SAVE_ONE_TEACH_BUTTON, &CStationMapping::OnBnClickedSmSaveOneTeachButton)
	ON_BN_CLICKED(IDC_SM_LOAD_TEACH_BUTTON, &CStationMapping::OnBnClickedSmLoadTeachButton)
	ON_BN_CLICKED(IDC_SM_SET_IMG_ID_BUTTON, &CStationMapping::OnBnClickedSmSetImgIdButton)
	ON_EN_UPDATE(IDC_SM_START_EDIT, &CStationMapping::OnUpdateSmStartEdit)
	ON_EN_UPDATE(IDC_SM_MAX_EDIT, &CStationMapping::OnUpdateSmMaxEdit)

	ON_EN_UPDATE(IDC_SM_Z_MTR_SERVO_EDIT, &CStationMapping::OnUpdateSmZMtrServoEdit)
//	ON_BN_CLICKED(IDC_SM_Z_MTR_SERVO_CHECK, &CStationMapping::OnClickedSmZMtrServoCheck)
	ON_BN_CLICKED(IDC_SM_Z_MTR_SERVO_UP_MFCBUTTON, &CStationMapping::OnBnClickedSmZMtrServoUpMfcbutton)
	ON_BN_CLICKED(IDC_SM_Z_MTR_SERVO_DOWN_MFCBUTTON, &CStationMapping::OnBnClickedSmZMtrServoDownMfcbutton)

	ON_BN_CLICKED(IDOK, &CStationMapping::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CStationMapping::OnBnClickedCancel)

	ON_BN_CLICKED(IDC_SM_NEGLECT_MFCBUTTON, &CStationMapping::OnBnClickedSmNeglectMfcbutton)
	ON_BN_CLICKED(IDC_SM_SAVE_TEACH_IMAGE_ALL_BUTTON, &CStationMapping::OnBnClickedSmSaveTeachImageAllButton)
	ON_CBN_SELCHANGE(IDC_MOTOR_ID_COMBO, &CStationMapping::OnSelchangeMotorIdCombo)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_START_POSITION, &CStationMapping::OnBnClickedButtonStartPosition)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TO_START_POS, &CStationMapping::OnBnClickedButtonMoveToStartPos)
	ON_BN_CLICKED(IDC_BUTTON_SET_START_POSITION, &CStationMapping::OnBnClickedButtonSetStartPosition)
END_MESSAGE_MAP()


// CStationMapping message handlers

BOOL CStationMapping::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetClientRect(rcClientWnd);


	_positiveMovementButton.LoadBitmaps(IDB_ARROWRIGHT, IDB_ARROWRIGHT_CLICKED);
	_negativeMovementButton.LoadBitmaps(IDB_ARROWLEFT, IDB_ARROWLEFT_CLICKED);

	_motorIdCombobox.ResetContent();

	std::vector< int > axisId;
	axisId.push_back(X_SERVO_MOTOR);
	axisId.push_back(Y_SERVO_MOTOR);
	axisId.push_back(Z_SERVO_TOP_MOTOR);
	axisId.push_back(Z_SERVO_BOTTOM_MOTOR);

	CString name;
	for (size_t id = 0; id < axisId.size(); ++id) {
		theApp.motionParameters.GetMotorName(axisId[id], name);
		_motorIdCombobox.AddString(name);
	}


	//Set default selection as first item
	_motorIdCombobox.SetCurSel(0);

	m_ctrlCameraID.ResetContent();

	for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
		if (nTrack == STATION_TOP)
			m_ctrlCameraID.AddString("Top Station");
		if(nTrack == STATION_BOTTOM)
			m_ctrlCameraID.AddString("Bottom Station");
		if(nTrack == STATION_SIDE)
			m_ctrlCameraID.AddString("Side Station");
	}

	m_ctrlCameraID.SetCurSel(0);

	OnSelchangeMotorIdCombo();

	// TODO:  Add extra initialization here
	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStationMapping::Initialize()
{

	CRect rcWindow;
	CRect rcItem;

	int nDeviceNoPerBlockX, nDeviceNoPerBlockY;
	int nBlockX, nBlockY;
	int nWindowOffset = 5;

	if (m_prgnPositionSelection != NULL)
		delete[] m_prgnPositionSelection;

	if (m_diePicCtrl != NULL)
		delete[] m_diePicCtrl;


	pApp->motionParameters.GetMappingParameters(nBlockX, nBlockY, nDeviceNoPerBlockX, nDeviceNoPerBlockY);
	m_nDeviceSelectionCount = nBlockX * nBlockY;

	OutputDebugLogTo(3, TRUE, "m_nDeviceSelectionCount %d", m_nDeviceSelectionCount);

	m_nDieSelectionCount = nDeviceNoPerBlockX * nDeviceNoPerBlockY;

	OutputDebugLogTo(3, TRUE, "m_nDieSelectionCount %d", m_nDieSelectionCount);


	m_prgnPositionSelection = new CRect[m_nDeviceSelectionCount];

	m_diePicCtrl = new CRect[m_nDieSelectionCount];


	m_bPosSelection = new BOOL[m_nDeviceSelectionCount];
	memset((void*)m_bPosSelection, 0, m_nDeviceSelectionCount * sizeof(BOOL));

//	m_prgnDieMapPositionSel = new CRgn[m_nDieSelectionCount];
	m_bDieMapPosSel = new BOOL[m_nDieSelectionCount];
	memset((void*)m_bDieMapPosSel, 0, m_nDieSelectionCount * sizeof(BOOL));


	/*for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
		m_strDieMapContents[nTrack].RemoveAll();
		m_strPulseValue[nTrack].RemoveAll();
		for(int a=0; a<m_nDieSelectionCount; a++)
			m_strDiePosIDContents[nTrack][a].RemoveAll();
	}
	m_ctrlZLevelMap.ResetContent();
	m_ctrlDieZLevelID.ResetContent();*/

	GetDlgItem(IDC_DEVICE_SEL)->GetWindowRect(&rcDeviceMappingRelative);
	ScreenToClient(&rcDeviceMappingRelative);

	GetDlgItem(IDC_STATIC_FOV_SEL)->GetWindowRect(&rcDieRelative);
	ScreenToClient(&rcDieRelative);


	//if (rcDieRelative.Width() < rcDieRelative.Height())
	//	rcDieRelative.bottom = rcDieRelative.top + rcDieRelative.Width();
	//else
	//	rcDieRelative.right = rcDieRelative.left + rcDieRelative.Height();

	//GetDlgItem(IDC_STATIC_FOV_SEL)->SetWindowPos(NULL, rcDieRelative.left, rcDieRelative.top, rcDieRelative.Width(), rcDieRelative.Width(), SWP_NOZORDER);


	const int nScreenXSize = rcDeviceMappingRelative.Width();
	const int nScreenYSize = rcDeviceMappingRelative.Height();

	const int nDiePerX = nBlockX;
	const int nDiePerY = nBlockY;

	int nDeviceBordersX = nBlockX * 3 - 1;
	int nDeviceBordersY = nBlockY * 3 - 1;

	int nFreeSpaceDieX = nDiePerX + nBlockX;
	int nFreeSpaceDieY = nDiePerY + nBlockY;

	int nMaxScreenDrawX = nScreenXSize - nDeviceBordersX - nFreeSpaceDieX - nWindowOffset;
	int nMaxScreenDrawY = nScreenYSize - nDeviceBordersY - nFreeSpaceDieY - nWindowOffset;

	nDieRectSize = nMaxScreenDrawX / nDiePerX < nMaxScreenDrawY / nDiePerY ? nMaxScreenDrawX / nDiePerX : nMaxScreenDrawY / nDiePerY;

	if (nDieRectSize < 20)
		nDieRectSize = 20;

	if (nDieRectSize > 30)
		nDieRectSize = 30;

	//Creat die rectangles
	int nDieOffsetX = 0;
	int nDieOffsetY = 0;
	int nDiePerLine = nBlockX;

	int nX;
	int nY;
	int nDieDeviceIDX;
	int nDieDeviceIDY;

	for (int i = 0; i<m_nDeviceSelectionCount; i++) {

		nDieOffsetX = i % nDiePerLine;
		nDieDeviceIDX = nDieOffsetX / nBlockX;
		nDieOffsetX = nDieOffsetX % nBlockX;

		nDieOffsetY = i / nDiePerLine;
		nDieDeviceIDY = nDieOffsetY / nBlockY;
		nDieOffsetY = nDieOffsetY % nBlockY;

		nX = nDieDeviceIDX * (3 + 1 + (nDieRectSize + 1) * nBlockX) + 2 + (nDieRectSize + 1) * nDieOffsetX + nWindowOffset / 2 + rcDeviceMappingRelative.left;
		nY = nDieDeviceIDY * (3 + 1 + (nDieRectSize + 1) * nBlockY) + 2 + (nDieRectSize + 1) * nDieOffsetY + nWindowOffset / 2 + rcDeviceMappingRelative.top;

		m_prgnPositionSelection[i] = CRect(nX, nY, nX + nDieRectSize, nY + nDieRectSize);
	}

	nDiePerLine = nDeviceNoPerBlockX;

	OutputDebugLogTo(3, TRUE, "After BLock");


	for (int i = 0; i < m_nDieSelectionCount; i++) {
	
		nDieOffsetX = i % nDiePerLine;
		nDieDeviceIDX = nDieOffsetX / nDeviceNoPerBlockX;
		nDieOffsetX = nDieOffsetX % nDeviceNoPerBlockX;

		nDieOffsetY = i / nDiePerLine;
		nDieDeviceIDY = nDieOffsetY / nDeviceNoPerBlockY;
		nDieOffsetY = nDieOffsetY % nDeviceNoPerBlockY;

		nX = nDieDeviceIDX * (3 + 1 + (nDieRectSize + 1) * nBlockX) + 2 + (nDieRectSize + 1) * nDieOffsetX + nWindowOffset / 2 + rcDieRelative.left;
		nY = nDieDeviceIDY * (3 + 1 + (nDieRectSize + 1) * nBlockY) + 2 + (nDieRectSize + 1) * nDieOffsetY + nWindowOffset / 2 + rcDieRelative.top;

		m_diePicCtrl[i] = CRect(nX, nY, nX + nDieRectSize, nY + nDieRectSize);
	}

	nZoom = 1;

	OutputDebugLogTo(3, TRUE, "After Die");

	GetWindowRect(&rcWindow);


	//nScrollBarHMax = rcDeviceMappingRelative.Width() - nWindowOffset;
	//nScrollBarVMax = rcDeviceMappingRelative.Height() - nWindowOffset;
	//nHorizontalScrollBar = 0;
	//nVerticalScrollBar = 0;

	//CRect ClientRect;
	//GetClientRect(ClientRect);
	//int nVScrollMax;
	//int nHScrollMax;
	//if (ClientRect.Height() < rcDeviceMappingRelative.Height())
	//	nVScrollMax = (rcDeviceMappingRelative.Height() - ClientRect.Height());// + nWindowOffset + 20/*scrolbar width*/;
	//else
	//	nVScrollMax = 0;

	//if (ClientRect.Width() < rcDeviceMappingRelative.Width())
	//	nHScrollMax = rcDeviceMappingRelative.Width() - ClientRect.Width() + nWindowOffset;
	//else
	//	nHScrollMax = 0;


	//SCROLLINFO info;
	//info.cbSize = sizeof(SCROLLINFO);
	//info.fMask = SIF_ALL;
	//info.nMin = 0;
	//info.nMax = nScrollBarHMax*nZoom;
	//info.nPage = (nScrollBarHMax*nZoom) / 10;//rcClientWnd.Width();
	//info.nPos = nHorizontalScrollBar;
	//info.nTrackPos = nHorizontalScrollBar;

	//scrollBarH.SetScrollInfo(&info);

	//info.nMax = nScrollBarVMax*nZoom;
	//info.nPage = (nScrollBarVMax*nZoom) / 10;//rcClientWnd.Height();
	//info.nPos = nVerticalScrollBar;
	//info.nTrackPos = nVerticalScrollBar;

	//scrollBarV.SetScrollInfo(&info);

	Invalidate();

	
	///*if(m_bPosSelection != NULL)
	//	delete[] m_bPosSelection;*/
	//if(m_prgnDieMapPositionSel!=NULL)
	//	delete [] m_prgnDieMapPositionSel;
	//if(m_bDieMapPosSel != NULL)
	//	delete[] m_bDieMapPosSel;

	//if (m_diePicCtrl != NULL)
	//	delete[] m_diePicCtrl;

	//for (int i = 0; i < m_nDieSelectionCount; i++) {
	//	m_diePicCtrl[i].Create("", 1, CRect(0, 0, 100, 100), this);
	//	m_diePicCtrl[i].ShowWindow(TRUE);
	//}
	////m_bPosSelection = new BOOL[m_nDeviceSelectionCount];
	//m_prgnDieMapPositionSel = new CRgn[m_nDieSelectionCount];
	//m_bDieMapPosSel = new BOOL[m_nDieSelectionCount];
	//
	//CRect rectDie, rectDieMap;
	//int nSel, nDeviceNoPerBlock, nTempMul=-1;
	//nDeviceNoPerBlock = (pApp->m_nDeviceNoPerBlock[1] > MAX_TRACKS) ? MAX_TRACKS : pApp->m_nDeviceNoPerBlock[1];
	//for(int i=0; i<m_nDeviceSelectionCount; i++) {
	//	m_bPosSelection[i] = FALSE;
	//	if((i%nDeviceNoPerBlock)==0)
	//		nTempMul++;
	//	nSel = (nTempMul*(MAX_DIE_FOVS/MAX_TRACKS))+(i%nDeviceNoPerBlock);
	//	GetDlgItem(IDC_SM_SEL_STATIC1 + nSel)->GetWindowRect(&rectDie);		
	//	ScreenToClient(&rectDie);
	//	m_prgnPositionSelection[i].CreateRoundRectRgn(rectDie.left-2, rectDie.top-2, rectDie.right+2, rectDie.bottom+2, 2, 2);
	//}
	//CRect rectTemp;
	//GetDlgItem(IDC_SM_DIE_MAP_SEL_GRP)->GetWindowRect(&rectTemp);
	//ScreenToClient(&rectTemp);
	//int nFovWidthRgn = (rectTemp.Width() / pApp->m_nNoOfFovX )-5;
	//int nFovHeightRgn = (rectTemp.Height() / pApp->m_nNoOfFovY)-5;
	//int nXPos = rectTemp.left + 15;
	//int nYPos = rectTemp.top + 25;
	//int nTemp = 0;
	//for(int j=0; j<pApp->m_nNoOfFovY; j++) {
	//	for (int i = 0; i < pApp->m_nNoOfFovX; i++) {
	//		m_bDieMapPosSel[nTemp] = (nTemp>0) ? FALSE : TRUE;
	//		m_diePicCtrl[nTemp].SetWindowPos(NULL, nXPos + (i*nFovWidthRgn),(j==0?nYPos: nYPos-15) + (j*nFovHeightRgn), nFovWidthRgn -5, nFovWidthRgn -5, SWP_NOZORDER);
	//		m_diePicCtrl[nTemp].GetWindowRect(&rectDieMap);
	//		ScreenToClient(&rectDieMap);
	//		m_prgnDieMapPositionSel[nTemp].CreateRoundRectRgn(rectDieMap.left - 2, rectDieMap.top - 2, rectDieMap.right + 2, rectDieMap.bottom + 2, 2, 2);
	//		nTemp++;
	//	}
	//	
	//}

	///*for(int k=0; k<MAX_DIE_MAP_FOVS; k++) {
	//	CString strText;
	//	strText.Format("FOV%d", k+1);
	//	GetDlgItem(IDC_SM_DIE_MAP_SEL_STATIC1 + k)->SetWindowTextA(strText);
	//	GetDlgItem(IDC_SM_DIE_MAP_SEL_STATIC1 + k)->ShowWindow((k<m_nDieSelectionCount)?TRUE:FALSE);
	//}*/

	//GetDlgItem(IDC_SM_SELECTION_GRP)->GetWindowRect(&rcDeviceMappingRelative);		
	//ScreenToClient(&rcDeviceMappingRelative);
	//GetDlgItem(IDC_SM_DIE_MAP_SEL_GRP)->GetWindowRect(&rcDieRelative);		
	//ScreenToClient(&rcDieRelative);	
	
	RedrawPositions();

	OutputDebugLogTo(3, TRUE, "After Redraw");

	m_bLightDlgOpen = FALSE;
	m_nCurSelectedDieMapPosID = 0;
	m_nCurSelectedFovPosID = 0;
	m_ctrlCameraID.SetCurSel(0);
	StationMappingFile();

	OutputDebugLogTo(3, TRUE, "Save map file");

	ResetListBoxContents(m_nCurSelectedFovPosID);
	OutputDebugLogTo(3, TRUE, "after list");

//	pApp->ScannedFOVSequenceData(TRUE);
	pApp->motionParameters.LoadScanningParm();

	OutputDebugLogTo(3, TRUE, "after load parm");

	for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++)
		pApp->m_pTracks[nTrack].m_nFOVIndex = m_nCurSelectedDieMapPosID;//0;
	//ScrollBarEnable(TRUE);
}

void CStationMapping::ClearAll()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);

	for (int nTrack = 0; nTrack< theApp.m_nTotalTracks; nTrack++) {

		int nCount = m_strDieMapContents[nTrack].GetCount();
		if (!m_strDieMapContents[nTrack].IsEmpty()) {
			m_strDieMapContents[nTrack].RemoveAll();
		}
		m_strPulseValue[nTrack].RemoveAll();
		for (int a = 0; a< nFovPerX * nFovPerY; a++)
			m_strDiePosIDContents[nTrack][a].RemoveAll();
	}
	m_ctrlZLevelMap.ResetContent();
	m_ctrlDieZLevelID.ResetContent();
//	pApp->motionParameters.ClearEnableGrab();

}

void CStationMapping::OnPaint()
{
	//if (bRedrawing)
	//	return;
	//else
	//	bRedrawing = TRUE;

	int i;

	CPaintDC dc(this); // device context for painting
	CDC dcMemory;
	CRect ClientRect;
	GetClientRect(ClientRect);

	dcMemory.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	CBitmap * pbitmap;

	bitmap.CreateCompatibleBitmap(&dc, ClientRect.Width(), ClientRect.Height());

	pbitmap = dcMemory.SelectObject(&bitmap);

	::FillRect(dcMemory.GetSafeHdc(), ClientRect, brushBackground);

	CRect  * prcPosPtr = m_prgnPositionSelection;
	BOOL * pnPosStatePtr = m_bPosSelection;
	CRect rcRect;

//	if (nZoom == 1) {
		for (i = 0; i<m_nDeviceSelectionCount; i++, pnPosStatePtr++, prcPosPtr++) {
			rcRect = CRect(prcPosPtr->left /*- nHorizontalScrollBar*/,
				prcPosPtr->top /*- nVerticalScrollBar*/,
				prcPosPtr->right/* - nHorizontalScrollBar*/,
				prcPosPtr->bottom /*- nVerticalScrollBar*/);

			if (CheckRect(&rcDeviceMappingRelative, &rcRect)) continue;

			dcMemory.FillRect(rcRect, m_bPosSelection[i] ? &m_brushCheck : &m_brushUncheck);
		}

		CRect  * prcDiePtr = m_diePicCtrl;
		BOOL * pnDieStatePtr = m_bDieMapPosSel;

		for (i = 0; i<m_nDieSelectionCount; i++, pnDieStatePtr++, prcDiePtr++) {
			rcRect = CRect(prcDiePtr->left,
				prcDiePtr->top,
				prcDiePtr->right,
				prcDiePtr->bottom);

			if (CheckRect(&rcDieRelative, &rcRect)) continue;

			dcMemory.FillRect(rcRect, &m_bDieMapPosSel[i] ? &m_brushCheck : &m_brushUncheck);
		}

		dc.BitBlt(0, 0, ClientRect.Width(), ClientRect.Height(), &dcMemory, 0, 0, SRCCOPY);

		dcMemory.SelectObject(pbitmap);

		dcMemory.DeleteDC();
		bitmap.DeleteObject();

		bRedrawing = FALSE;

//	}
///	else {
		/*for (i = 0; i<m_nDeviceSelectionCount; i++, pnDieStatePtr++, prcDiePtr++) {
			rcRect = CRect(prcDiePtr->left   * nZoom - nHorizontalScrollBar,
				prcDiePtr->top    * nZoom - nVerticalScrollBar,
				prcDiePtr->right  * nZoom - nHorizontalScrollBar,
				prcDiePtr->bottom * nZoom - nVerticalScrollBar);

			if (CheckRect(&rcDeviceMappingRelative, &rcRect)) continue;

			dcMemory.FillRect(rcRect, &brushCell[*pnDieStatePtr]);
		}*/
//	}





	//CDC* pDC = &dc;
	//
	//for(int x=0; x<m_nDeviceSelectionCount; x++) {
	//	pDC->FillRgn(&m_prgnPositionSelection[x], m_bPosSelection[x] ? &m_brushCheck : &m_brushUncheck);
	//	pDC->FrameRgn(&m_prgnPositionSelection[x], &m_brushBorder, 1, 1);
	//}
	//for(int y=0; y<m_nDieSelectionCount; y++) {
	//	pDC->FillRgn(&m_prgnDieMapPositionSel[y], m_bDieMapPosSel[y] ? &m_brushCheck : &m_brushUncheck);
	//	pDC->FrameRgn(&m_prgnDieMapPositionSel[y], &m_brushBorder, 1, 1);
	//}
}


void CStationMapping::ScrollBarEnable(BOOL bHorizontal)
{
	CString str;
    CSize sz;
    int dx = 0;
	CDC* pDC = m_ctrlZLevelMap.GetDC();
    for(int i=0; i<m_ctrlZLevelMap.GetCount(); i++){
        m_ctrlZLevelMap.GetText(i,str);
        sz = pDC->GetTextExtent(str);
        if(sz.cx > dx)
            dx = sz.cx;
    }
    m_ctrlZLevelMap.ReleaseDC(pDC);

    if(bHorizontal && m_ctrlZLevelMap.GetHorizontalExtent() < dx ){
        m_ctrlZLevelMap.SetHorizontalExtent(dx);
        ASSERT(m_ctrlZLevelMap.GetHorizontalExtent() == dx);
    }
}

//// *************************************************************************************** ////

void CStationMapping::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint pointRel = point;

	BOOL bFoundPtInDieMap = FALSE;

	if (PtInRect(&pointRel, &rcClientWnd)) {

		CRect  * prcPosPtr = m_prgnPositionSelection;

		for (int i = 0; i<m_nDeviceSelectionCount; i++, prcPosPtr++) {
			CRect rcDie = CRect(prcPosPtr->left * nZoom,
				prcPosPtr->top * nZoom,
				prcPosPtr->right * nZoom,
				prcPosPtr->bottom * nZoom);

			CheckRect(&rcClientWnd, &rcDie);

			if (RectInRect(rcClientWnd, rcDie)) {

				if (PtInRect(&pointRel, &rcDie)) {
					m_bPosSelection[i] = !m_bPosSelection[i];
					theApp.motionParameters.setDeviceStatus(i, m_bPosSelection[i]);

					bFoundPtInDieMap = FALSE;
					break;
				}
			}
		}

		CRect  * prcDiePtr = m_diePicCtrl;

		for (int i = 0; i<m_nDieSelectionCount; i++, prcDiePtr++) {
			CRect rcDie = CRect(prcDiePtr->left,
				prcDiePtr->top * nZoom,
				prcDiePtr->right * nZoom,
				prcDiePtr->bottom * nZoom);

			CheckRect(&rcClientWnd, &rcDie);

			if (RectInRect(rcClientWnd, rcDie)) {

				if (PtInRect(&pointRel, &rcDie)) {
					bFoundPtInDieMap = TRUE;
					int nFov;
					int nError = pApp->motionParameters.getDieMappingFovNo(i, nFov);
					m_nCurSelectedDieMapPosID = i;
					m_nCurSelectedFovPosID = nFov;
					int nTrack = m_ctrlCameraID.GetCurSel();
					pApp->m_pTracks[nTrack].m_nFOVIndex = nFov;
					break;
				}
			}
		}
	}


	//BOOL bFoundPtInDieMap = FALSE;
	//for(int i=0; i<m_nDeviceSelectionCount; i++) {
	//	if(m_prgnPositionSelection[i].PtInRegion(point)) {
	//		m_bPosSelection[i] = !m_bPosSelection[i];
	//		bFoundPtInDieMap = FALSE;
	//		break;
	//	}
	//}

	//for(int j=0; j<m_nDieSelectionCount; j++) {
	//	if(m_prgnDieMapPositionSel[j].PtInRegion(point)) {
	//		//m_bDieMapPosSel[j] = TRUE;//!m_bDieMapPosSel[j];
	//		bFoundPtInDieMap = TRUE;
	//		m_nCurSelectedDieMapPosID = j;
	//		int nTrack = m_ctrlCameraID.GetCurSel();
	//		pApp->m_pTracks[nTrack].m_nFOVIndex = j;
	//		break;
	//	}	
	//}
	if(bFoundPtInDieMap) {
		for(int j=0; j<m_nDieSelectionCount; j++)
			m_bDieMapPosSel[j] = FALSE;
		m_bDieMapPosSel[m_nCurSelectedDieMapPosID] = TRUE;
		ResetListBoxContents(m_nCurSelectedFovPosID);
	}

	InvalidateRect(!bFoundPtInDieMap ? &rcDeviceMappingRelative : rcDieRelative);

	CDialogEx::OnLButtonDown(nFlags, point);
}


//void CStationMapping::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	CDialogEx::OnRButtonDown(nFlags, point);
//}

void CStationMapping::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int nTrack = m_ctrlCameraID.GetCurSel();

	CRect  * prcDiePtr = m_diePicCtrl;

	CPoint pointRel = point;

	if (PtInRect(&pointRel, &rcClientWnd)) {

		for (int i = 0; i < m_nDieSelectionCount; i++, prcDiePtr++) {
			CRect rcDie = CRect(prcDiePtr->left,
				prcDiePtr->top * nZoom,
				prcDiePtr->right * nZoom,
				prcDiePtr->bottom * nZoom);

			CheckRect(&rcClientWnd, &rcDie);

			if (RectInRect(rcClientWnd, rcDie)) {
				if (PtInRect(&pointRel, &rcDie))
					MoveToFOVPosAndSetLight();
			}
		}
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

int CStationMapping::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	// TODO: Add your message handler code here and/or call default

	int nRow = pListBox->GetCurSel();
	if(nKey == VK_UP)
		NavigateListBoxContents(nRow, FALSE, TRUE);
	else if(nKey == VK_DOWN)
		NavigateListBoxContents(nRow, TRUE, TRUE); 

	return CDialogEx::OnVKeyToItem(nKey, pListBox, nIndex);
}

void CStationMapping::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int nCurPos[2];

	if (nIDEvent == STATION_MAPPING_TIMER) {

		RedrawPositions();
	}


		//	if(pApp->m_bStationMapDlgOpen) {
		//		if(pApp->m_pMotionControl) {
		//			for(int i=0; i<pApp->m_nTotalTracks-1; i++) {
		//				nCurPos[i] = pApp->m_pMotionControl->GetCurrentPos(i+2);
		//				if(nCurPos[i] != m_nZMotorPos[i])
		//					m_nZMotorPos[i] = nCurPos[i];
		//			}
		//		}
		//		int nTrack = m_ctrlCameraID.GetCurSel();
		//		if(nTrack < pApp->m_nTotalTracks-1)
		//			m_nZMotorPosDisp = (int)(m_nZMotorPos[nTrack]/*/pApp->m_pMotionControl->m_dMuToPulses*/);
		////		if(nTrack == pApp->m_nTotalTracks - 1)
		////			m_nZMotorPosDisp = (int)(m_nZMotorPos[/*nTrack*/0]/*/pApp->m_pMotionControl->m_dMuToPulses*/);
		//		//if(::IsWindow(m_hWnd))
		//			UpdateData(FALSE);
		//	}
	CDialogEx::OnTimer(nIDEvent);
}

//// *************************************************************************************** ////

void CStationMapping::OnSelchangeSmCamidCombo()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	//m_dZMotorRelativeMoveDisp = m_dZMotorRelativeMove[nTrack];
	m_strZMotorRelativeMoveDisp.Format("%.2f", m_dZMotorRelativeMove[nTrack]);
	m_dZMotorScanMaxDisp = m_dZMotorScanMax[nTrack];
//	((CButton*) GetDlgItem(IDC_SM_Z_MTR_SERVO_CHECK))->SetCheck(m_bRelativeCheck[nTrack]);
	for(int z=0; z<m_nDieSelectionCount; z++)
		m_bDieMapPosSel[z] = (z>0)? FALSE : TRUE;

	m_nCurSelectedDieMapPosID = 0;
	m_nCurSelectedFovPosID = 0;
	ResetListBoxContents(m_nCurSelectedFovPosID);
//	UpAndDownZMotorServoButton((nTrack==pApp->m_nTotalTracks-1)?FALSE:TRUE);
	UpdateData(FALSE);
	InvalidateRect(rcDieRelative);
}


void CStationMapping::OnBnClickedSmSelectallButton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	BOOL bSelect = ((CButton*) GetDlgItem(IDC_SM_SELECTALL_BUTTON))->GetCheck();
	for(int i=0; i<m_nDeviceSelectionCount; i++)
		m_bPosSelection[i] = bSelect;

	UpdateData(FALSE);
	InvalidateRect(&rcDeviceMappingRelative);
}

//// *************************************************************************************** ////

void CStationMapping::OnBnClickedSmDieMapAddMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString str;
	int nTrack, nCount;
	nTrack = m_ctrlCameraID.GetCurSel();
	nCount = m_ctrlZLevelMap.GetCount();

	if (nCount >= theApp.m_pTracks[nTrack].m_nTotalDocs)
		return;

	str.Format("%2d: Z 0 Light: 0 0 0 0 0 0 0 0", nCount+1);
	m_ctrlZLevelMap.AddString(str);
	m_strDieMapContents[nTrack].Add(str);

	if(nCount>0)
		m_ctrlZLevelMap.SetCurSel(nCount);

	nCount = m_strPulseValue[nTrack].GetSize();
	str.Format("%2d: Pulse: 0 0 0 0 0 0 0 0", nCount+1);
	m_strPulseValue[nTrack].Add(str);

	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmDieMapRemoveMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nSel, nTrack;
	nSel = m_ctrlZLevelMap.GetCurSel();
	nTrack = m_ctrlCameraID.GetCurSel();
	if(nSel<0)
		return;

	RemoveFromDieMapPosId(nTrack, nSel);
	m_ctrlZLevelMap.DeleteString(nSel);
	//m_strDieMapContents[nTrack].RemoveAt(nSel);

	CString str, strTemp, strTemp1;
	int nCount, nPos;	
	nCount = m_ctrlZLevelMap.GetCount();
	for(int i=0; i<nCount; i++) {
		str.Empty();
		m_ctrlZLevelMap.GetText(i, str);
		strTemp.Format("%2d", i+1);
		str.Replace(str.Mid(0, str.Find(':')), strTemp);
		m_ctrlZLevelMap.DeleteString(i);
		m_ctrlZLevelMap.AddString(str);
		//m_strDieMapContents[nTrack].RemoveAt(i);
		//m_strDieMapContents[nTrack].SetAt(i, str);
	}
	
	m_strDieMapContents[nTrack].RemoveAll();
	for(int j=0; j<m_ctrlZLevelMap.GetCount(); j++) {
		CString strTempAdd;
		m_ctrlZLevelMap.GetText(j, strTempAdd);
		m_strDieMapContents[nTrack].Add(strTempAdd);
	}

	if(nCount>1)
		m_ctrlZLevelMap.SetCurSel(nCount-1);

	nCount = m_strPulseValue[nTrack].GetSize();
	CStringArray strPulseTempArr;
	int nTempPulseCount=1;
	strPulseTempArr.Copy(m_strPulseValue[nTrack]);
	m_strPulseValue[nTrack].RemoveAll();
	for(int j=0; j<nCount; j++) {
		if(j==nSel)
			continue;
		str.Empty();
		str = strPulseTempArr.GetAt(j);
		strTemp.Format("%2d", nTempPulseCount++);
		str.Replace(str.Mid(0, str.Find(':')), strTemp);
		m_strPulseValue[nTrack].Add(str);
	}
	UpdateData(FALSE);
}

void CStationMapping::ResetListBoxContents(int nFovPos)
{
	int nTrack, nSel;
	nTrack = m_ctrlCameraID.GetCurSel();

	m_ctrlZLevelMap.ResetContent();
	m_ctrlDieZLevelID.ResetContent();

	/*if(nTrack==2 && m_nCurSelectedDieMapPosID > 0 && m_nCurSelectedDieMapPosID < m_nDieSelectionCount-1)
	return;*/
	CString str, strPulse, strIntensity;
	CString strTemp, strTempAppend = "", strTempPulse = "";


	for (int a = 0; a < m_strDieMapContents[nTrack].GetCount(); a++) {
		m_ctrlZLevelMap.AddString(m_strDieMapContents[nTrack].GetAt(a));
		CString strTempZPos, strTempLPos, strTempAdd;

		m_ctrlZLevelMap.GetText(a, strTemp);
		strIntensity = strTemp.Mid(0, strTemp.Find(':'));
		int nIntensity = atoi(strIntensity) - 1;
		strTempZPos = strTemp.Mid(0, strTemp.Find('Z') + 1);
		strTempLPos = strTemp.Mid(strTemp.Find('L'));
		strTempAdd.Format("%s %d %s", strTempZPos, pApp->m_pTracks[nTrack].m_pDoc[nIntensity]->m_nZAxisPosition[nFovPos], strTempLPos);

		m_ctrlZLevelMap.DeleteString(a);
		m_ctrlZLevelMap.AddString(strTempAdd);
		//m_strDieMapContents[nTrack].RemoveAt(nSel);
	}

	for (int b = 0; b<m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetCount(); b++)
		m_ctrlDieZLevelID.AddString(m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetAt(b));
}

void CStationMapping::OnBnClickedSmDieMapRetakeMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nSel, nTrack, nCount, nChannel, nPulseVal;
	int nTempCount, nTempDoc;
	double dVal;
	CString str, strPulse, strIntensity;
	CString strTemp, strTempAppend="", strTempPulse="";
	nSel = m_ctrlZLevelMap.GetCurSel();

	if(nSel<0)
		return;

	m_ctrlZLevelMap.GetText(nSel, str);
	strIntensity = str.Mid(0, str.Find(':'));
	int nIntensity = atoi(strIntensity) - 1;
	
	nTrack = m_ctrlCameraID.GetCurSel();
	if(m_bLightDlgOpen) {
		int nStep;
		theApp.motionParameters.GetViewID(nTrack, nIntensity, nStep);

		pApp->m_LightHandler.UpdateLightStep(0, nStep);//// Set
//		for(int nView=0; nView<pApp->m_nTotalViewDocs[nTrack]; nView++) {

			nTempCount = pApp->m_LightHandler.m_nLightNoOfSegments[nStep];
			for(int n=0; n<nTempCount; n++) {
				nChannel = pApp->m_pTracks[nTrack].m_nSegmentsOfImage[nIntensity].GetAt(n);
				dVal = pApp->m_LightHandler.GetLight(0, nChannel);				
				dVal = dVal*pApp->m_LightHandler.m_pMaxCurrents[nChannel] / 100;
				strTemp.Format(" %.2f", dVal);
				strTempAppend.Append(strTemp);
				nPulseVal = pApp->m_LightHandler.GetPulseWidth(0, nChannel);
				strTempPulse.Format(" %d", nPulseVal);
				strPulse.Append(strTempPulse);
			}
//		}
		pApp->m_LightHandler.UpdateLightStep(0, -2); //// Reset
		nCount = m_ctrlZLevelMap.GetCount();
		m_ctrlZLevelMap.GetText(nSel, str);
		strTemp = str.Mid(0, str.ReverseFind(':')+1);
		strTemp += strTempAppend;
	}
	else {
		m_ctrlZLevelMap.GetText(nSel, strTemp);
	}

	CString strTempZPos, strTempLPos, strTempAdd;
	strTempZPos = strTemp.Mid(0, strTemp.Find('Z')+1);
	strTempLPos = strTemp.Mid(strTemp.Find('L'));

	int nZMotorCurrentPos = 0;
	int nError;

	if (nTrack == STATION_TOP) {
		nError = theApp.motionParameters.GetCurrentMotorPosition(Z_SERVO_TOP_MOTOR, m_nZMotorPos[nTrack]);
		if (nError) return;
	}
	else if(nTrack == STATION_BOTTOM){
		nError = theApp.motionParameters.GetCurrentMotorPosition(Z_SERVO_BOTTOM_MOTOR, m_nZMotorPos[nTrack]);
		if (nError) return;
	}


//	nError = motionParameters.SetZMotorLevelPosition(nSelectedItem, nZMotorCurrentPos);
	if(nTrack != STATION_SIDE)
		strTempAdd.Format("%s %d %s", strTempZPos, m_nZMotorPos[nTrack], strTempLPos);
	else
		strTempAdd.Format("%s 0 %s", strTempZPos, strTempLPos);

	SetZPositionValues(nTrack, m_nCurSelectedFovPosID, nIntensity, strTempAdd, m_nZMotorPos[nTrack]);

	m_ctrlZLevelMap.DeleteString(nSel);
	m_ctrlZLevelMap.AddString(strTempAdd);
	//m_strDieMapContents[nTrack].RemoveAt(nSel);
	m_strDieMapContents[nTrack].SetAt(nSel, strTempAdd);


	ScrollBarEnable(TRUE);
	//if(nCount>1)
	m_ctrlZLevelMap.SetCurSel(nSel);
	
	if(m_bLightDlgOpen) {
		nCount = m_strPulseValue[nTrack].GetSize();
		str = m_strPulseValue[nTrack].GetAt(nSel);
		strTemp = str.Mid(0, str.ReverseFind(':')+1);
		strTemp += strPulse;
		m_strPulseValue[nTrack].SetAt(nSel, strTemp);
		m_bLightDlgOpen = FALSE;
	}
	UpdateData(FALSE);
}

//void CStationMapping::OnDblclkSmDieMapList()
void CStationMapping::OnSelchangeSmDieMapList()
{
	// TODO: Add your control notification handler code here

	int nSel, nTrack;
	nSel = m_ctrlZLevelMap.GetCurSel();
	nTrack = m_ctrlCameraID.GetCurSel();
	if(nSel<0 /*|| nTrack==(pApp->m_nTotalTracks-1)*/)
		return;

	//if(AfxMessageBox("Confirm to move to Z position and set light", MB_YESNO) == IDYES) {
		if(!pApp->m_pTracks[nTrack].m_bCamAvail) {
			return;
		}

		UpdateStatusBar(&CString("Updating Z Position And Light..."));
		SetCursor(LoadCursor(NULL, IDC_WAIT));

		for(int i=0; i<pApp->m_nTotalTracks; i++) {
			for(int j=0; j<pApp->m_nTotalViewDocs[i]; j++) {
				if(pApp->m_pTracks[i].m_bLiveImage)
					pApp->m_pTracks[i].m_pDoc[j]->CameraLive();
			}
		}

		CString str, strPulse;
		m_ctrlZLevelMap.GetText(nSel, str);
		strPulse = m_strPulseValue[nTrack].GetAt(nSel);
		
		CString strTemp = str.Mid(0, str.Find(':'));
		int nIntensity = atoi(strTemp) - 1;
		int nStep;
		theApp.motionParameters.GetViewID(nTrack, nIntensity, nStep);
		pApp->m_pFrames[nStep]->MDIActivate();

		pApp->m_LightHandler.UpdateLightStep(0, nStep);	//// Set
		SetLightValues(nTrack, nIntensity, str, strPulse);
		pApp->m_LightHandler.UpdateLightStep(0, -2); //// Reset
//		SetZPositionValues(nTrack, m_nCurSelectedFovPosID, 0, str);
		
		pApp->m_pTracks[nTrack].SetZPositionForSequence(nIntensity);
		int nErr = 0;//pApp->WaitForZPositionEnd(/*nTrack*/);
		if(!nErr)
			pApp->m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure(TRUE, FALSE,FALSE, TRUE);

		SetCursor(LoadCursor(NULL, IDC_ARROW));
		UpdateStatusBar(&CString(""));
	//}
}


void CStationMapping::OnBnClickedSmDieMapSwapMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString str, strTemp, strTemp2;
	int nSel, nCount, nTrack;
	nTrack = m_ctrlCameraID.GetCurSel();
	nSel = m_ctrlZLevelMap.GetCurSel();
	if(nSel<0) 
	{
		if(nSel<0)
			AfxMessageBox("Please select to transfer!");		
		return;
	}
	
	nCount = m_ctrlDieZLevelID.GetCount();
	if(nCount < pApp->m_nTotalViewDocs[nTrack]) {
		m_ctrlZLevelMap.GetText(nSel, str);
		strTemp = str.Mid(0, str.Find(':'));
		if(!nCount) {
			m_ctrlDieZLevelID.AddString(strTemp);
			m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].Add(strTemp);
		}
		else {
			int nFound = m_ctrlDieZLevelID.FindString(0, strTemp);
			if(nFound<0) {
				m_ctrlDieZLevelID.AddString(strTemp);
				m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].Add(strTemp);
			}
		}
	}

//	int nGrab = atoi(strTemp) - 1;
//	int nIntensity = 0;
////	strTemp = m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetAt(nGrab);
//	if (atoi(strTemp)) {
//		nIntensity = atoi(strTemp) - 1;
//		str = m_strDieMapContents[nTrack].GetAt(nSel);
//		m_ctrlZLevelMap.GetText(nSel, str);
//
//		SetZPositionValues(nTrack, m_nCurSelectedFovPosID, nIntensity, str);
//	}
	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmDieMapPosmoveSetlightButton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(AfxMessageBox("Confirm to move to positions and set light", MB_YESNO) == IDYES) {
		MoveToFOVPosAndSetLight();
	//}
	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmDieMapPosidRemoveMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nSel, nTrack;
	nSel = m_ctrlDieZLevelID.GetCurSel();
	nTrack = m_ctrlCameraID.GetCurSel();
	if(nSel>=0) {
		m_ctrlDieZLevelID.DeleteString(nSel);
		m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].RemoveAt(nSel);
	}
	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmDieMapLivecamLightButton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	if(!pApp->m_bLightCtrlDlgOpen) {
		int nSel, nCount;
		nCount = m_ctrlZLevelMap.GetCount();
		if(!nCount) {
			AfxMessageBox("Please add light values!");
			return;
		}

		nSel = m_ctrlZLevelMap.GetCurSel();
		if(nSel<0) {
			//nSel = 0;
			AfxMessageBox("Please select for light values!");
			return;
		}

		CString str, strPulse;
		int nStep;
		m_ctrlZLevelMap.GetText(nSel, str);
		CString strIntensity = str.Mid(0, str.Find(':'));
		int nIntensity = atoi(strIntensity) - 1;
		strPulse = m_strPulseValue[nTrack].GetAt(nSel);
		theApp.motionParameters.GetViewID(nTrack, nIntensity, nStep);

		pApp->m_pFrames[nStep]->MDIActivate();

		pApp->m_LightHandler.UpdateLightStep(0, nStep);	//// Set
		SetLightValues(nTrack, nIntensity, str, strPulse);
		pApp->m_LightHandler.UpdateLightStep(0, -2); //// Reset
		pApp->m_pTracks[nTrack].m_pDoc[nIntensity]->OnCameraLight();
	}

	UpdateData(FALSE);
}

void CStationMapping::OnBnClickedSmNeglectMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString str, strTemp, strTemp2;
	int nCount, nTrack;
	nTrack = m_ctrlCameraID.GetCurSel();
	if((nTrack==2 && m_nCurSelectedDieMapPosID > 0 && m_nCurSelectedDieMapPosID < m_nDieSelectionCount-1))
		return;

	nCount = m_ctrlDieZLevelID.GetCount();
	if(nCount < pApp->m_nTotalViewDocs[nTrack]) {
		strTemp = " -";
		m_ctrlDieZLevelID.AddString(strTemp);
		m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].Add(strTemp);
	}
	UpdateData(FALSE);
}

//// *************************************************************************************** ////

void CStationMapping::OnBnClickedSmSaveAllTeachButton()		//// Teach FOV Sequeunce 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	//pApp->m_pFrames[!nTrack?0:(nTrack==1?pApp->m_nTop:pApp->m_nBottom)]->MDIActivate();
	//pApp->m_pTracks[nTrack].m_pDoc[0]->CameraLive();
	if(pApp->m_pMotionControl && !pApp->m_bMotionCtrlDlgOpen) {
		pApp->OnMotionControl();
	}

	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmSaveOneTeachButton()	//// Pos IDs contents move down
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	int nRow = m_ctrlDieZLevelID.GetCurSel();
	NavigateListBoxContents(nRow, TRUE);

	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmLoadTeachButton()	//// Pos IDs contents move up
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	int nRow = m_ctrlDieZLevelID.GetCurSel();
	NavigateListBoxContents(nRow, FALSE);

	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmSetImgIdButton()		//// Save FOV Sequence 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nTrack = m_ctrlCameraID.GetCurSel();	
	if(pApp->m_pMotionControl /*&& nTrack < (pApp->m_nTotalTracks-1)*/) {
//		pApp->m_nXAxisPosition[/*m_nCurSelectedDieMapPosID*/0] = pApp->m_pMotionControl->GetCurrentPos(0);
//		pApp->m_nYAxisPosition[/*m_nCurSelectedDieMapPosID*/0] = pApp->m_pMotionControl->GetCurrentPos(1);
		for(int nDoc=0; nDoc<pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			CString strTemp="";
			if(nDoc < m_strDiePosIDContents[nTrack][0].GetCount()) 
				strTemp = m_strDiePosIDContents[nTrack][0].GetAt(nDoc);
			
			if(!strTemp.IsEmpty())
				SetZPositionValues(nTrack, 0, nDoc, strTemp, 0);
			else {
				if (nTrack < (pApp->m_nTotalTracks - 1)) {
					
			//		pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[0] = pApp->m_pMotionControl->GetCurrentPos(nTrack + 2);
					pApp->motionParameters.GetCurrentMotorPosition(nTrack + 2, pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[0]);
				}
				//else
				//	pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[0] = pApp->m_pMotionControl->GetCurrentPos(/*nTrack +*/ 2);
			}
		}
	}
//	pApp->ScannedFOVSequenceData(FALSE);
	pApp->motionParameters.SaveScanningParm();


	UpdateData(FALSE);
}


void CStationMapping::OnUpdateSmStartEdit()			//// Current Pos
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);

	//int nTrack = m_ctrlCameraID.GetCurSel();
	//m_nZMotorPos[nTrack] = m_nZMotorPosDisp;

	///UpdateData(FALSE);
}


void CStationMapping::OnUpdateSmMaxEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	m_dZMotorScanMax[nTrack] = m_dZMotorScanMaxDisp;

	//UpdateData(FALSE);
}

//// *************************************************************************************** ////

void CStationMapping::OnUpdateSmZMtrServoEdit()		//// Realtive Move
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);

	int nTrack = m_ctrlCameraID.GetCurSel();
	//m_dZMotorRelativeMove[nTrack] = m_dZMotorRelativeMoveDisp;
	m_dZMotorRelativeMove[nTrack] = atof(m_strZMotorRelativeMoveDisp);//m_dZMotorRelativeMoveDisp;

	//UpdateData(FALSE);
}

//void CStationMapping::OnClickedSmZMtrServoCheck()
//{
//	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//
//	int nTrack = m_ctrlCameraID.GetCurSel();
////	m_bRelativeCheck[nTrack] = ((CButton*) GetDlgItem(IDC_SM_Z_MTR_SERVO_CHECK))->GetCheck();
//
//	//UpdateData(FALSE);
//}


void CStationMapping::OnBnClickedSmZMtrServoUpMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nTrack = m_ctrlCameraID.GetCurSel();
	if (!pApp->m_pTracks[nTrack].m_bCamAvail || (pApp->m_strMotionCard.CompareNoCase("none") == 0))
		return;

	//if(pApp->m_pMotionControl) {
	//	pApp->m_pMotionControl->m_bSnapEnable = TRUE;
	//	pApp->m_pMotionControl->nTrack = nTrack;
	//	if (nTrack == pApp->m_nTotalTracks - 1)
	//		nTrack = 0;
	//	pApp->m_pMotionControl->m_dStepMM[nTrack+2] = atof(m_strZMotorRelativeMoveDisp);//m_dZMotorRelativeMoveDisp;
	//	double dValue = (pApp->m_pMotionControl->m_dMotorResolution/(double)pApp->m_pMotionControl->GetMMPerSec(nTrack+2));
	//	//dValue = (dValue*m_dZMotorRelativeMoveDisp*10)/10.0;	//// round off
	//	dValue = (dValue*atof(m_strZMotorRelativeMoveDisp)*10)/10.0;	//// round off
	//	pApp->m_pMotionControl->m_nStep[nTrack+2] = (int)dValue;
	//	pApp->m_pMotionControl->SetCommand(!nTrack ? MOVE_UP_TOP_Z : MOVE_UP_BOTTOM_Z);
	//}

	RelativeMove(_id, 1);
	
	UpdateData(FALSE);
}


void CStationMapping::OnBnClickedSmZMtrServoDownMfcbutton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nTrack = m_ctrlCameraID.GetCurSel();
	if (!pApp->m_pTracks[nTrack].m_bCamAvail || (pApp->m_strMotionCard.CompareNoCase("none") == 0))
		return;

//	if(pApp->m_pMotionControl) {
//		pApp->m_pMotionControl->m_bSnapEnable = TRUE;
//		pApp->m_pMotionControl->nTrack = nTrack;
//		if (nTrack == pApp->m_nTotalTracks - 1)
//			nTrack = 0;
//		pApp->m_pMotionControl->m_dStepMM[nTrack+2] = atof(m_strZMotorRelativeMoveDisp);//m_dZMotorRelativeMoveDisp;
//		double dValue = (pApp->m_pMotionControl->m_dMotorResolution/(double)pApp->m_pMotionControl->GetMMPerSec(nTrack+2));
		//dValue = (dValue*m_dZMotorRelativeMoveDisp*10)/10.0;	//// round off
//		dValue = (dValue*atof(m_strZMotorRelativeMoveDisp)*10)/10.0;	//// round off
//		pApp->m_pMotionControl->m_nStep[nTrack+2] = (int)dValue;

		RelativeMove(_id, -1);
//		pApp->m_pMotionControl->SetCommand(!nTrack ? MOVE_DOWN_TOP_Z : MOVE_DOWN_BOTTOM_Z);
//	}
	UpdateData(FALSE);
}

//// *************************************************************************************** ////

void CStationMapping::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UpdateStatusBar(&CString("Updating All Values..."));
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	pApp->m_bStationMapDlgOpen = FALSE;
	pApp->m_bAllowStream = FALSE;
	KillTimer(1);

	CProgressBarDlg *pProgressDlg;
	pProgressDlg = new CProgressBarDlg();

	if (!pProgressDlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd())) {
		delete pProgressDlg;
		pProgressDlg = NULL;
	}
	else {
		if (pProgressDlg != NULL) {
			pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
			pProgressDlg->CenterWindow();
			pProgressDlg->UpdateWindow();
			pProgressDlg->SetStatus("Saving Mapping parameters...", 0);
		}
	}

	StationMappingFile(FALSE);

//// Saving All Tracks - Docs - FOVs Set Light Values to Light Files(.ltv) on Click OK ////
	CString strTemp, str, strPulse;
	int nSel, nDiePosCount, nDocCount, nTempCount;
	int nPrevZValue;
	for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
		nDocCount = pApp->m_nTotalViewDocs[nTrack];
//		for(int nFOV=0; nFOV<m_nDieSelectionCount; nFOV++) {
//			nDiePosCount = m_strDiePosIDContents[nTrack][nFOV].GetCount();
//			nTempCount = (nDocCount <= nDiePosCount) ? nDocCount : nDiePosCount;
			for (int nDoc = 0; nDoc < nDocCount; nDoc++) {
				nSel = 0;
				str = "";
				strPulse = "";

				if (pProgressDlg != NULL) {
					pProgressDlg->SetStatus("Saving Light Settings....", ((nTrack * nDocCount) / (pApp->m_nTotalTracks * nDocCount))*90);
					pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
				}

				//					strTemp = m_strDiePosIDContents[nTrack][nFOV].GetAt(nDoc);
				if (m_strDieMapContents[nTrack].GetCount() > nDoc) {
					nSel = atoi(strTemp);
					//m_ctrlZLevelMap.GetText(nSel-1, str);
					str = m_strDieMapContents[nTrack].GetAt(nDoc);
					strPulse = m_strPulseValue[nTrack].GetAt(nDoc);
				}
				else {
					nSel = 0;
					str.Format("%2d: Z %d Light: 0 0 0 0 0 0 0 0", nDoc + 1, nPrevZValue);
					strPulse.Format("%2d: Pulse: 0 0 0 0 0 0 0 0", nDoc + 1);
					//bGrab = FALSE;
				}
				int nStep;
				theApp.motionParameters.GetViewID(nTrack, nDoc, nStep);

				pApp->m_LightHandler.UpdateLightStep(0, nStep);	//// Set
				SetLightValues(nTrack, nDoc, str, strPulse);
				pApp->m_LightHandler.SaveLightSettings(nDoc, nTrack);
				pApp->m_LightHandler.UpdateLightStep(0, -2); //// 
			}
			/*for (int nFOV = 0; nFOV<m_nDieSelectionCount; nFOV++) {
				for (int nDoc = 0; nDoc < nDocCount; nDoc++) {
					nPrevZValue = SetZPositionValues(nTrack, nFOV, nDoc, str);
				}
			}*/
//		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->SetStatus("Saving Scanning parameters....", 90);
		pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
	}
//	pApp->ScannedFOVSequenceData(FALSE);
	pApp->motionParameters.SaveScanningParm();

	/*for (int nFOV = 0; nFOV < pApp->m_nNoOfFov; nFOV++) {
		strTemp.Format("Position = %d,XPos = %d", nFOV + 1, pApp->m_nXAxisPosition[nFOV]);
		pApp->AddLogEvent(strTemp, MOTION_LOG);
		strTemp.Format("Position = %d,YPos = %d", nFOV + 1, pApp->m_nYAxisPosition[nFOV]);
		pApp->AddLogEvent(strTemp, MOTION_LOG);
		for (int nTrack = 0; nTrack < pApp->m_nTotalTracks - 1; nTrack++) {
			for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
				strTemp.Format("T%d FOV%d Doc%d %s = %d", nTrack + 1, nFOV + 1, nDoc + 1, !nTrack ? "TopZPos" : "BotZPos", pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV]);
				pApp->AddLogEvent(strTemp, MOTION_LOG);
			}
		}
	}*/
	//pApp->m_LightHandler.RefreshLight();
	//pApp->m_LightHandler.ResetLightSteps();
	SetEvent(pApp->m_EventRefreshLight);//// Set Event for Refresh Light

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	UpdateStatusBar(&CString(""));
	UpdateData(FALSE);

	KillTimer(STATION_MAPPING_TIMER);

	if (pProgressDlg != NULL) {
		pProgressDlg->SetStatus("Saving Scanning parameters...", 100);
		//		theApp.m_bProgressDlgOpen = FALSE;
		pProgressDlg->ShowWindow(FALSE);
		pProgressDlg->DestroyWindow();
		delete pProgressDlg;
		pProgressDlg = NULL;
	}

	CDialogEx::OnOK();
}


void CStationMapping::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UpdateStatusBar(&CString("Updating All Values..."));
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	CProgressBarDlg *pProgressDlg;
	pProgressDlg = new CProgressBarDlg();

	if (!pProgressDlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd())) {
		delete pProgressDlg;
		pProgressDlg = NULL;
	}
	else {
		if (pProgressDlg != NULL) {
			pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
			pProgressDlg->CenterWindow();
			pProgressDlg->UpdateWindow();
			pProgressDlg->SetStatus("Loading Light Settings...", 0);
		}
	}

	pApp->m_bStationMapDlgOpen = FALSE;
	pApp->m_bAllowStream = FALSE;

	for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			pApp->m_LightHandler.LoadLightSettings(nDoc, nTrack);

			if (pProgressDlg != NULL) {
				pProgressDlg->SetStatus("Loading Light Settings....", ((nTrack * pApp->m_nTotalViewDocs[nTrack]) / (pApp->m_nTotalTracks * pApp->m_nTotalViewDocs[nTrack])) * 90);
				pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
			}
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->SetStatus("Loading Light Settings...", 100);
		//		theApp.m_bProgressDlgOpen = FALSE;
		pProgressDlg->ShowWindow(FALSE);
		pProgressDlg->DestroyWindow();
		delete pProgressDlg;
		pProgressDlg = NULL;
	}
	//pApp->m_LightHandler.RefreshLight();
	//pApp->m_LightHandler.ResetLightSteps();
	SetEvent(pApp->m_EventRefreshLight);//// Set Event for Refresh Light

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	UpdateStatusBar(&CString(""));
	UpdateData(FALSE);

	KillTimer(STATION_MAPPING_TIMER);
	

	CDialogEx::OnCancel();
}


void CStationMapping::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//UpdateData(TRUE);
	//UpdateStatusBar(&CString("Updating All Values..."));
	//SetCursor(LoadCursor(NULL, IDC_WAIT));

	//pApp->m_bStationMapDlgOpen = FALSE;	
	//pApp->m_bAllowStream = FALSE;
	//KillTimer(STATION_MAPPING_TIMER);

	//for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
	//	for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
	//		pApp->m_LightHandler.LoadLightSettings(nDoc, nTrack);
	//	}
	//}
	////pApp->m_LightHandler.RefreshLight();
	////pApp->m_LightHandler.ResetLightSteps();
	//SetEvent(pApp->m_EventRefreshLight);//// Set Event for Refresh Light

	//SetCursor(LoadCursor(NULL, IDC_ARROW));
	//UpdateStatusBar(&CString(""));
	//UpdateData(FALSE);

	CDialogEx::OnClose();
}

//// *************************************************************************************** ////

void CStationMapping::UpAndDownZMotorServoButton(BOOL bEnable)
{
	GetDlgItem(IDC_SM_Z_MTR_SERVO_UP_MFCBUTTON)->EnableWindow(bEnable);
	GetDlgItem(IDC_SM_Z_MTR_SERVO_DOWN_MFCBUTTON)->EnableWindow(bEnable);
	GetDlgItem(IDC_SM_SAVE_ALL_TEACH_BUTTON)->EnableWindow(bEnable);
}



void CStationMapping::NavigateListBoxContents(int nRow, BOOL bDown, BOOL bKeyDown)
{
	CString strListBox;
	int nRowChange, nTrack, nCount;
	BOOL bNavigate = FALSE;
	
	nRowChange = bDown ? nRow+1 : nRow-1;
	nTrack = m_ctrlCameraID.GetCurSel();
	nCount = m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetCount();

	if(!bDown && nRow > 0 && nRow <= nCount-1)
		bNavigate = TRUE;
	else if(bDown && nRow >= 0 && nRow < nCount-1)
		bNavigate = TRUE;

	if(bNavigate) {
		strListBox = m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetAt(nRow);
		m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].SetAt(nRow, m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetAt(nRowChange));
		m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].SetAt(nRowChange, strListBox);
		
		m_ctrlDieZLevelID.ResetContent();
		for(int i=0; i<nCount; i++) {
			m_ctrlDieZLevelID.AddString(m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetAt(i));
		}
		m_ctrlDieZLevelID.SetCurSel(bKeyDown ? nRow : nRowChange);
	}
}

void CStationMapping::RemoveFromDieMapPosId(int nTrack, int nSel)
{
	CString str, strTemp, strVal;

	m_ctrlZLevelMap.GetText(nSel, str);
	strTemp = str.Mid(0, str.Find(':'));

	//for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
		for(int a=0; a<m_nDieSelectionCount; a++) {
			for(int b=0; b<m_strDiePosIDContents[nTrack][a].GetCount(); b++) {
				strVal = m_strDiePosIDContents[nTrack][a].GetAt(b);
				if(strTemp.CompareNoCase(strVal)==0) {
					m_strDiePosIDContents[nTrack][a].RemoveAt(b);
					OutputDebugLogTo(1,TRUE,"[Camera%d][FOV%d] Die Map Position Id %s REMOVED", nTrack+1, a+1, strTemp);
				}
			}
		}
	//}
	
	ResetListBoxContents(m_nCurSelectedFovPosID);
}

void CStationMapping::MoveToFOVPosAndSetLight()
{
	CString strTemp, str, strPulse;
	int nTrack, nSel, nStep, nErr;
	int DiePosCount, nDocCount, nTempCount;

	nTrack = m_ctrlCameraID.GetCurSel();
	if (!pApp->m_pTracks[nTrack].m_bCamAvail) {
		return;
	}

	pApp->m_pTracks[nTrack].m_pDoc[0]->DrawView(FALSE);
	UpdateStatusBar(&CString("Updating Positions And Light..."));
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
		int nFov = 0;
//		int nError = pApp->motionParameters.getDieMappingFovNo(m_nCurSelectedDieMapPosID, nFov);
		pApp->m_pTracks[nTrack].m_nFOVIndex = m_nCurSelectedFovPosID;//0;
	}

	nDocCount = pApp->m_nTotalViewDocs[nTrack];
	DiePosCount = m_strDiePosIDContents[nTrack][m_nCurSelectedFovPosID].GetCount();
	nTempCount = (nDocCount <= DiePosCount) ? nDocCount : DiePosCount;

	int nView;
	theApp.motionParameters.GetViewID(nTrack, 0, nView);

	pApp->m_pFrames[nView]->MDIActivate();

	/*for (int i = 0; i<pApp->m_nTotalTracks; i++) {
		for (int j = 0; j<pApp->m_nTotalViewDocs[i]; j++) {
			if (pApp->m_pTracks[i].m_bLiveImage)
				pApp->m_pTracks[i].m_pDoc[j]->CameraLive();
		}
	}*/

	MoveXYToPosition(nTrack, m_nCurSelectedDieMapPosID, FALSE);
	nErr = 0;
	pApp->m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure(TRUE, FALSE, FALSE, TRUE);


	//for(int nDoc=0; nDoc<nDocCount; nDoc++) {
	//			nSel = 0;
	//			str = "";
	//			strPulse = "";
	//	if(nDoc < nTempCount) {
	//		strTemp = m_strDiePosIDContents[nTrack][m_nCurSelectedDieMapPosID].GetAt(nDoc);
	//		if(atoi(strTemp)) {
	//			nSel = atoi(strTemp);
	//			//m_ctrlZLevelMap.GetText(nSel-1, str);
	//			str = m_strDieMapContents[nTrack].GetAt(nSel-1);
	//			strPulse = m_strPulseValue[nTrack].GetAt(nSel-1);
	//		}
	//		else {
	//			nSel = 0;
	//			str = "";
	//			strPulse = "";
	//		}
	//	}
	//		nStep = (MAX_TRACKS*nTrack)+nDoc;
	//		pApp->m_LightHandler.UpdateLightStep(0, nStep, m_nCurSelectedDieMapPosID);	//// Set
	//		SetLightValues(nTrack, pApp->m_nTotalViewDocs[nTrack], m_nCurSelectedDieMapPosID, str, strPulse);
	//		pApp->m_LightHandler.UpdateLightStep(0, -2, m_nCurSelectedDieMapPosID); //// Reset
	//		SetZPositionValues(nTrack, m_nCurSelectedDieMapPosID, nDoc, str);

	//		pApp->m_pTracks[nTrack].SetZPositionForSequence(nDoc);
	//		nErr = 0;//pApp->WaitForZPositionEnd(/*nTrack*/);
	//		if(!nErr)
	//			OutputDebugLogTo(9,TRUE,"CStationMapping::MoveToFOVPosAndSetLight WaitForPositionEnd Error");
	//}

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	UpdateStatusBar(&CString(""));
}

void CStationMapping::StationMappingFile(BOOL bRead)
{
	CIniFile iniFile;
	CFileException e;

	CString strKey, strFile, strTemp, strGetVal="";
	int nCamId, nTempCount=-1;
	BOOL bCheck=FALSE;

	UpdateData(TRUE);
	//strFile.Format("%s\\ScanningSequenceMapping.dat", pApp->m_strConfigDir);
	strFile.Format("%s\\%s\\%s\\ScanningSequenceMapping.dat", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
	iniFile.SetPath(strFile);
	
	strKey.Format("StationMap");
	if(bRead && iniFile.ReadFile()) {
		ClearAll();

		nCamId = iniFile.GetInteger(strKey, "Camera ID", 0);
		for(int nCam=0; nCam<pApp->m_nTotalTracks; nCam++) {
			strTemp.Format("ServoMotorValue[%d]", nCam);
			m_dZMotorRelativeMove[nCam] = iniFile.GetDouble(strKey, strTemp, m_dZMotorRelativeMove[nCam]);
			strTemp.Format("MotorScanMax[%d]", nCam);			
			m_dZMotorScanMax[nCam] = iniFile.GetDouble(strKey, strTemp, m_dZMotorScanMax[nCam]);
			strTemp.Format("Relative Move Check[%d]", nCam);			
			m_bRelativeCheck[nCam] = iniFile.GetBoolean(strKey, strTemp, m_bRelativeCheck[nCam]);		
		}

		for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
			for(int a=0; a<m_nDieSelectionCount; a++) {
				nTempCount = -1;// = m_strDiePosIDContents[a].GetCount();
				strTemp.Format("Die Pos IDs[%d][%d] Count:", nTrack+1, a+1);
				nTempCount = iniFile.GetInteger(strKey, strTemp, nTempCount);
				for(int b=0; b<nTempCount; b++) {
					strTemp.Format("Die Pos IDs[%d][%d][%d]:", nTrack+1, a+1, b+1);
					strGetVal = iniFile.GetCString(strKey, strTemp, strGetVal);
					m_strDiePosIDContents[nTrack][a].Add(strGetVal);
					int nIntensity = atoi(strGetVal);
					pApp->motionParameters.EnableGrab(nTrack, nIntensity -1, a, true);
				}
			}

			nTempCount = -1;
			strTemp.Format("Die Pos Count[T%d]:", nTrack+1);
			nTempCount = iniFile.GetInteger(strKey, strTemp, nTempCount);
			for(int c=0; c<nTempCount; c++) {
				strTemp.Format("Die Pos Contents[T%d][%d]:", nTrack+1, c+1);
				strGetVal = iniFile.GetCString(strKey, strTemp, strGetVal);
				//m_ctrlZLevelMap.AddString(strGetVal);
				m_strDieMapContents[nTrack].Add(strGetVal);
			}

			nTempCount = -1;
			strTemp.Format("Die Pos Pulse Count[T%d]:", nTrack+1);
			nTempCount = iniFile.GetInteger(strKey, strTemp, nTempCount);
			for(int d=0; d<nTempCount; d++) {
				strTemp.Format("Die Pos Pulse Contents[T%d][%d]:", nTrack+1, d+1);
				strGetVal = iniFile.GetCString(strKey, strTemp, strGetVal);
				m_strPulseValue[nTrack].Add(strGetVal);
			}
		}

		//strTemp.Format("Die Pos Count:");
		//nTempCount = iniFile.GetInteger(strKey, strTemp, nTempCount);
		//for(int c=0; c<nTempCount; c++) {
		//	strTemp.Format("Die Pos Contents[%d]:", c+1);
		//	strGetVal = iniFile.GetCString(strKey, strTemp, strGetVal);
		//	m_ctrlZLevelMap.AddString(strGetVal);
		//}
		
		/*strTemp.Format("Die Pos Pulse Count:");
		nTempCount = iniFile.GetInteger(strKey, strTemp, nTempCount);
		for(int d=0; d<nTempCount; d++) {
			strTemp.Format("Die Pos Pulse Contents[%d]:", d+1);
			strGetVal = iniFile.GetCString(strKey, strTemp, strGetVal);
			m_strPulseValue.Add(strGetVal);
		}*/

		int nBlockX, nBlockY;
		int nDeviceNoPerBlockX, nDeviceNoPerBlockY;
		theApp.motionParameters.GetMappingParameters(nBlockX, nBlockY, nDeviceNoPerBlockX, nDeviceNoPerBlockY);

	//	int nDieCol, DieRow;
	//	theApp.motionParameters.GetDieGrabNumber(nDieCol, DieRow);
		nTempCount = nBlockX * nBlockY;

		if(nTempCount > MAX_DIE_FOVS)
			nTempCount = MAX_DIE_FOVS;
		strTemp.Format("Pos Selection Count:");
		nTempCount = iniFile.GetInteger(strKey, strTemp, nTempCount);
		for(int e=0; e<nTempCount; e++) {
			strTemp.Format("Pos Selection[%d]", e+1);
			m_bPosSelection[e] = iniFile.GetBoolean(strKey, strTemp, m_bPosSelection[e]);
			theApp.motionParameters.setDeviceStatus(e, m_bPosSelection[e]);
		}

		m_ctrlCameraID.SetCurSel(nCamId);
		//m_dZMotorRelativeMoveDisp = m_dZMotorRelativeMove[nCamId];
		m_strZMotorRelativeMoveDisp.Format("%.2f", m_dZMotorRelativeMove[nCamId]);
		m_dZMotorScanMaxDisp = m_dZMotorScanMax[nCamId];
	//	((CButton*) GetDlgItem(IDC_SM_Z_MTR_SERVO_CHECK))->SetCheck(m_bRelativeCheck[nCamId]);
	}
	else {
		nCamId = m_ctrlCameraID.GetCurSel();
		iniFile.SetInteger(strKey, "Camera ID", nCamId);
		for(int nCam=0; nCam<pApp->m_nTotalTracks; nCam++) {
			strTemp.Format("ServoMotorValue[%d]", nCam);
			iniFile.SetDouble(strKey, strTemp, m_dZMotorRelativeMove[nCam]);			
			strTemp.Format("MotorScanMax[%d]", nCam);			
			iniFile.SetDouble(strKey, strTemp, m_dZMotorScanMax[nCam]);
			strTemp.Format("Relative Move Check[%d]", nCam);			
			iniFile.SetBoolean(strKey, strTemp, m_bRelativeCheck[nCam]);
		}

		for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
			for(int a=0; a<m_nDieSelectionCount; a++) {
				int nTempCount = m_strDiePosIDContents[nTrack][a].GetCount();
				strTemp.Format("Die Pos IDs[%d][%d] Count:", nTrack+1, a+1);
				iniFile.SetInteger(strKey, strTemp, nTempCount);
				for(int b=0; b<nTempCount; b++) {
					strTemp.Format("Die Pos IDs[%d][%d][%d]:", nTrack+1, a+1, b+1);
					iniFile.SetCString(strKey, strTemp, m_strDiePosIDContents[nTrack][a].GetAt(b));
				}
			}

			nTempCount = m_strDieMapContents[nTrack].GetCount();//m_ctrlZLevelMap.GetCount();
			strTemp.Format("Die Pos Count[T%d]:", nTrack+1);
			iniFile.SetInteger(strKey, strTemp, nTempCount);
			for(int c=0; c<nTempCount; c++) {
				strTemp.Format("Die Pos Contents[T%d][%d]:", nTrack+1, c+1);
				//m_ctrlZLevelMap.GetText(c, strGetVal);
				strGetVal = m_strDieMapContents[nTrack].GetAt(c);
				iniFile.SetCString(strKey, strTemp, strGetVal);
			}

			nTempCount = m_strPulseValue[nTrack].GetSize();
			strTemp.Format("Die Pos Pulse Count[T%d]:", nTrack+1);
			iniFile.SetInteger(strKey, strTemp, nTempCount);
			for(int d=0; d<nTempCount; d++) {
				strTemp.Format("Die Pos Pulse Contents[T%d][%d]:", nTrack+1, d+1);
				strGetVal = m_strPulseValue[nTrack].GetAt(d);
				iniFile.SetCString(strKey, strTemp, strGetVal);
			}
		}

		//nTempCount = m_ctrlZLevelMap.GetCount();
		//strTemp.Format("Die Pos Count:");
		//iniFile.SetInteger(strKey, strTemp, nTempCount);
		//for(int c=0; c<nTempCount; c++) {
		//	strTemp.Format("Die Pos Contents[%d]:", c+1);
		//	m_ctrlZLevelMap.GetText(c, strGetVal);
		//	iniFile.SetCString(strKey, strTemp, strGetVal);
		//}

		/*nTempCount = m_strPulseValue.GetSize();
		strTemp.Format("Die Pos Pulse Count:");
		iniFile.SetInteger(strKey, strTemp, nTempCount);
		for(int d=0; d<nTempCount; d++) {
			strTemp.Format("Die Pos Pulse Contents[%d]:", d+1);
			strGetVal = m_strPulseValue.GetAt(d);
			iniFile.SetCString(strKey, strTemp, strGetVal);
		}*/

		strTemp.Format("Pos Selection Count:");
		iniFile.SetInteger(strKey, strTemp, m_nDeviceSelectionCount);
		for(int e=0; e<m_nDeviceSelectionCount; e++) {
			strTemp.Format("Pos Selection[%d]", e+1);
			iniFile.SetBoolean(strKey, strTemp, m_bPosSelection[e]);
		}
		iniFile.WriteFile();
	}
//	UpAndDownZMotorServoButton((nCamId==pApp->m_nTotalTracks-1)?FALSE:TRUE);
	
	UpdateData(FALSE);
}

void CStationMapping::MoveXYToPosition(int nTrack, int nFOV, BOOL bHome)
{
	int nDieRow;
	int nDieCol;
	int nError;
	int nColumnNo;
	int nRowNo;

	BOOL nTempDieMap = TRUE;

	theApp.motionParameters.GetDieGrabNumber(nColumnNo, nRowNo);


	nTempDieMap = FALSE;
	nDieCol = nFOV % nColumnNo;
	nDieRow = nFOV / nColumnNo;

	if (!pApp->motionParameters.IsMotorHomed(Z_SERVO_TOP_MOTOR)) {

		nError = pApp->motionParameters.HomeMove(Z_SERVO_TOP_MOTOR);
		if (nError) return;

		nError = pApp->motionParameters.WaitForMotionDone(Z_SERVO_TOP_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) return;

		nError = pApp->motionParameters.HomeDone(Z_SERVO_TOP_MOTOR);
		if (nError) return;

	}

	if (!pApp->motionParameters.IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {

		nError = pApp->motionParameters.HomeMove(Z_SERVO_BOTTOM_MOTOR);
		if (nError) return;

		nError = pApp->motionParameters.WaitForMotionDone(Z_SERVO_BOTTOM_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) return;

		nError = pApp->motionParameters.HomeDone(Z_SERVO_BOTTOM_MOTOR);
		if (nError) return;

	}

	// Do homing first if it's necessary
	if (!pApp->motionParameters.IsMotorHomed(X_SERVO_MOTOR)) {

		nError = pApp->motionParameters.HomeMove(X_SERVO_MOTOR, false);
		if (nError) return;
	}

	if (!pApp->motionParameters.IsMotorHomed(Y_SERVO_MOTOR)) {

		nError = pApp->motionParameters.HomeMove(Y_SERVO_MOTOR, false);
		if (nError) return;
	}

	if (!pApp->motionParameters.IsMotorHomed(X_SERVO_MOTOR)) {
		nError = pApp->motionParameters.WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) return;

		nError = pApp->motionParameters.HomeDone(X_SERVO_MOTOR);
		if (nError) return;
	}

	if (!pApp->motionParameters.IsMotorHomed(Y_SERVO_MOTOR)) {
		nError = pApp->motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) return;

		nError = pApp->motionParameters.HomeDone(Y_SERVO_MOTOR);
		if (nError) return;
	}

	// Get X and Y offset
	int nOffsetX = 0;
	int nOffsetY = 0;

	nError = pApp->motionParameters.GetDieXYOffset(nDieCol, nDieRow, nOffsetX, nOffsetY);
	if (nError) return;

	std::vector < int > nMotorScanPos(2);
	nMotorScanPos.resize(2);
	
	pApp->motionParameters.getMotorStartScanPosition(X_SERVO_MOTOR, nMotorScanPos[0]);
	pApp->motionParameters.getMotorStartScanPosition(Y_SERVO_MOTOR, nMotorScanPos[1]);

	// Move X motor
	nError = pApp->motionParameters.AbsoluteMove(X_SERVO_MOTOR, nMotorScanPos[0] + nOffsetX, pApp->motionParameters.getMovingSpeed(X_SERVO_MOTOR), false);
	if (nError) return;

	nError = pApp->motionParameters.WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut());
	if (nError) return;

	pApp->motionParameters.WaitMotorSettlingTime(X_SERVO_MOTOR);

	nError = pApp->motionParameters.AbsoluteMove(Y_SERVO_MOTOR, nMotorScanPos[1] + nOffsetY, pApp->motionParameters.getMovingSpeed(Y_SERVO_MOTOR), false);
	if (nError) return;

	nError = pApp->motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut());
	if (nError) return;

	pApp->motionParameters.WaitMotorSettlingTime(Y_SERVO_MOTOR);
}

int CStationMapping::SetZPositionValues(int nTrack, int nFOV, int nDoc, CString strSel, int nZValue)
{
//	int nZValue = 0;
	int nTempLPos, nTempZPos;
	CString strTemp;

	if(!strSel.IsEmpty() && nZValue == 0) {
		nTempZPos = strSel.Find('Z')+1;
		nTempLPos = strSel.Find('L');
		strTemp = strSel.Mid(nTempZPos, (nTempLPos-nTempZPos));
		nZValue = atoi(strTemp);
	}

	pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV] = nZValue;
	OutputDebugLogTo(9,TRUE,"SetZPositionValues::[T%d][FOV%d][Doc%d]ZValue=%d", nTrack, nFOV, nDoc, pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV]);
	return nZValue;
}

void CStationMapping::SetLightValues(int nTrack, int nDoc, CString strSel, CString strPulse)
{
	int nController = 0;
//// Assign zero value ////

	int nTempChannelCount;
	//if(!pApp->m_bStationMapDlgOpen) {
		nTempChannelCount = pApp->m_LightHandler.m_bShareAllChannel ? pApp->m_LightHandler.m_nSegmentsPerTrack : pApp->m_nTotalTracks * pApp->m_LightHandler.m_nSegmentsPerTrack;
		for(int nTempChannel=0; nTempChannel<nTempChannelCount; nTempChannel++) {
			pApp->m_LightHandler.SetLight(nController, nTempChannel, 0, FALSE);
			pApp->m_LightHandler.SetPulseWidth(nController, nTempChannel, 0, FALSE);
		}
	//}

	int nChannel, nTokenPos; 
	int nTempDoc, nTempCount;
	CString strTemp, strTempVal;

//// --- Light Intensity --- ////

	if(!strSel.IsEmpty()) {
		double dTempVal;
		nTokenPos = 0;
		strTemp = strSel.Mid(strSel.ReverseFind(':')+1, strSel.GetLength());
//		for(int nView=0; nView<nDocCount; nView++) {
			theApp.motionParameters.GetViewID(nTrack, nDoc, nTempDoc);

			nTempCount = pApp->m_LightHandler.m_nLightNoOfSegments[nTempDoc];
			for(int n=0; n<nTempCount; n++) {
				nChannel = pApp->m_pTracks[nTrack].m_nSegmentsOfImage[nDoc].GetAt(n);
				strTempVal = (nTokenPos >= 0) ? strTemp.Tokenize(" ", nTokenPos) : "";
				dTempVal = strTempVal.IsEmpty() ? 0 : atof(strTempVal);
				double dMinCur = pApp->m_LightHandler.m_pMaxCurrents[nChannel]+0.001;
				pApp->m_LightHandler.SetLight(nController, nChannel, (int)(((dTempVal*100)/dMinCur)), FALSE);
			}
//		}
		//pApp->m_LightHandler.SetAllLights(nController, nFOV);
	}

//// --- Pulse Width --- ////

	if(!strPulse.IsEmpty()) {
		int nTempPulseVal;
		nTokenPos = 0;
		strTemp = strPulse.Mid(strPulse.ReverseFind(':')+1, strPulse.GetLength());
//		for(int nView=0; nView<nDocCount; nView++) {
			theApp.motionParameters.GetViewID(nTrack, nDoc, nTempDoc);

			nTempCount = pApp->m_LightHandler.m_nLightNoOfSegments[nTempDoc];
			for(int n=0; n<nTempCount; n++) {
				nChannel = pApp->m_pTracks[nTrack].m_nSegmentsOfImage[nDoc].GetAt(n);
				strTempVal = (nTokenPos >= 0) ? strTemp.Tokenize(" ", nTokenPos) : "";
				nTempPulseVal = strTempVal.IsEmpty() ? 0 : atoi(strTempVal);
				pApp->m_LightHandler.SetPulseWidth(nController, nChannel, nTempPulseVal, FALSE);
			}
//		}
		//pApp->m_LightHandler.SetAllPulseWidth(nController, nFOV);
	}
}


//void CStationMapping::OnBnClickedSmSaveTeachImageAllButton()
//{
//	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//	int nDocCount, DiePosCount, nTempCount, nSel, nStep;
//	CString strTeachImgPath, strTemp, str, strPulse;
//
//	//// only if Motion Ctrl & Cam Avaialble ////
//	if (pApp->m_pTracks[0].m_bCamEnable && pApp->m_pTracks[0].m_pDoc[0]->m_bCameraActive && (pApp->m_strMotionCard.CompareNoCase("none") != 0)) {
//		for (int nFOV = 0; nFOV < pApp->m_nNoOfFov; nFOV++) {
//			MoveXYToPosition(/*nTrack*/0, nFOV, FALSE);
//
//			for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {	
//				nDocCount = pApp->m_nTotalViewDocs[nTrack];
//				DiePosCount = m_strDiePosIDContents[nTrack][nFOV].GetCount();
//				nTempCount = (nDocCount <= DiePosCount) ? nDocCount : DiePosCount;
//
//				for(int nDoc=0; nDoc<nDocCount; nDoc++) {
//					nSel = 0;
//					str = "";
//					strPulse = "";
//					if (nDoc < nTempCount) {
//						strTemp = m_strDiePosIDContents[nTrack][nFOV].GetAt(nDoc);
//						if (atoi(strTemp)) {
//							nSel = atoi(strTemp);
//							str = m_strDieMapContents[nTrack].GetAt(nSel - 1);
//							strPulse = m_strPulseValue[nTrack].GetAt(nSel - 1);
//						}
//						else {
//							nSel = 0;
//							str.Format("%2d: Z 0 Light: 0 0 0 0", nCount + 1);;
//							strPulse = "";
//						}
//					}
//					nStep = (MAX_TRACKS*nTrack) + nDoc;
//					pApp->m_LightHandler.UpdateLightStep(0, nStep, nFOV);	//// Set
//					SetLightValues(nTrack, pApp->m_nTotalViewDocs[nTrack], nFOV, str, strPulse);
//					pApp->m_LightHandler.UpdateLightStep(0, -2, nFOV); //// Reset
//					SetZPositionValues(nTrack, nFOV, nDoc, str);
//
//					if(nTrack < pApp->m_nTotalTracks-1)
//						pApp->m_pTracks[nTrack].SetZPositionForSequence(nDoc);
//				}
//				
//				pApp->m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure();
//
//				CRect rectTeach = CRect(CPoint(0, 0), pApp->m_pTracks[nTrack].m_pDoc[0]->m_Buffer.GetImgSize());
//				for (int nDoc1 = 0; nDoc1 <nDocCount; nDoc1++) {
//					ImgCopy(&pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer, &rectTeach, &pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_TeachBuffer[nFOV], &rectTeach);
//
//					if (pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer.IsAllocated()) {
//						strTeachImgPath.Format("%s\\TeachTemplate_Pos%d.bmp", pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_strConfigDir, nFOV + 1);
//						SaveGrayScaleBMPFile(strTeachImgPath, pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer);
//						////CString strTemp;
//						////strTemp.Format("C:\\Rakshith\\TempDelete\\ImgT%dFOV%dDoc%d.bmp", nTrack + 1, nFOV + 1, nDoc1 + 1);
//						////SaveGrayScaleBMPFile(strTemp, pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_TeachBuffer[nFOV]);
//					}
//				}
//			}
//		}
//	}
//	UpdateData(FALSE);
//}


void CStationMapping::OnBnClickedSmSaveTeachImageAllButton()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nDocCount, DiePosCount, nTempCount, nSel, nStep;
	CString strTeachImgPath, strTemp, str, strPulse;
	int nPrevZValue = 0;
	int nError;

	pApp->motionParameters.ClearEnableGrab();

	int nTotalFov = pApp->motionParameters.getTotalFovPerDevice();
	if (pApp->m_pTracks[0].m_bCamEnable && pApp->m_pTracks[0].m_pDoc[0]->m_bCameraActive && (pApp->m_strMotionCard.CompareNoCase("none") != 0)) {
		for (int nFOV = 0; nFOV < nTotalFov; nFOV++) {

			int nDiePos;
//			int nError = pApp->motionParameters.getDieMappingFovNo(nFOV, RealFOV);
			pApp->motionParameters.getDieMappingPos(nFOV, nDiePos);
			MoveXYToPosition(/*nTrack*/0, nDiePos, FALSE);

			for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
				int nIntensity = 0;
			//	int nFov = 0;
			//	nError = pApp->motionParameters.getDieMappingFovNo(nFOV, nFov);
				pApp->m_pTracks[nTrack].m_nFOVIndex = nFOV;
				nDocCount = pApp->m_nTotalViewDocs[nTrack];
				DiePosCount = m_strDiePosIDContents[nTrack][nFOV].GetCount();
				nTempCount = (nDocCount <= DiePosCount) ? nDocCount : DiePosCount;

				LogMessage(8, "nTrack=%d, nDocCount=%d, DiePosCount=%d, nTempCount=%d", nTrack, nDocCount, DiePosCount, nTempCount);
				
				BOOL bGrab = TRUE;
				for (int nGrabs = 0; nGrabs < m_strDiePosIDContents[nTrack][nFOV].GetCount(); nGrabs++) {
					nSel = 0;
					str = "";
					strPulse = "";
					if (nGrabs < nTempCount) {
						strTemp = m_strDiePosIDContents[nTrack][nFOV].GetAt(nGrabs);
						if (atoi(strTemp)) {
							nSel = atoi(strTemp);
							nIntensity = nSel - 1;
							str = m_strDieMapContents[nTrack].GetAt(nSel - 1);
							strPulse = m_strPulseValue[nTrack].GetAt(nSel - 1);
							pApp->motionParameters.EnableGrab(nTrack, nIntensity, nFOV, true);
						
						}
						else {
							nSel = 0;
							str.Format("%2d: Z %d Light: 0 0 0 0 0 0 0 0", nIntensity + 1, nPrevZValue);
							strPulse.Format("%2d: Pulse: 0 0 0 0 0 0 0 0", nIntensity + 1);
							//bGrab = FALSE;
						}
					}
					else {
						/*if (nTrack < pApp->m_nTotalTracks - 1) {
							nSel = 0;
							str.Format("%2d: Z %d Light: 0 0 0 0 0 0 0 0", nIntensity + 1, nPrevZValue);
							strPulse.Format("%2d: Pulse: 0 0 0 0 0 0 0 0", nIntensity + 1);
						}
						else*/
							bGrab = FALSE;
					}
					/*if (!bGrab)
						break;*/
					theApp.motionParameters.GetViewID(nTrack, nIntensity, nStep);

					pApp->m_LightHandler.UpdateLightStep(0, nStep); //// Set
					LogMessage(8, "(UpdateLightStep) nStep=%d, nFOV=%d", nStep, nFOV);
					SetLightValues(nTrack, nIntensity, str, strPulse);
					LogMessage(8, "(SetLightValues) str=%s, nFOV=%d", str, nFOV);
					pApp->m_LightHandler.UpdateLightStep(0, -2); //// Reset
	//				nPrevZValue = SetZPositionValues(nTrack, nFOV, nIntensity, str);
					LogMessage(8, "(SetZPositionValues) nDoc=%d nPrevZValue = %d", nIntensity, nPrevZValue);

					//if (nTrack < pApp->m_nTotalTracks - 1)
					//	pApp->m_pTracks[nTrack].SetZPositionForSequence(nDoc);
				}

				//LogMessage(8, "bGrab=%d", bGrab);
				if (bGrab)
					pApp->m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure(1,0,TRUE);

				CRect rectTeach = CRect(CPoint(0, 0), pApp->m_pTracks[nTrack].m_pDoc[0]->m_Buffer.GetImgSize());
				for (int nDoc1 = 0; nDoc1 <nDocCount; nDoc1++) {

					if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc1, nFOV))
						continue;

					ImgCopy(&pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer, &rectTeach, &pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_TeachBuffer[nFOV], &rectTeach);

					if (pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer.IsAllocated()) {
						strTeachImgPath.Format("%s\\TeachTemplate_Pos%d.bmp", pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_strConfigDir, nFOV + 1);
						SaveGrayScaleBMPFile(strTeachImgPath, pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer);
						////CString strTemp;
						////strTemp.Format("C:\\Rakshith\\TempDelete\\ImgT%dFOV%dDoc%d.bmp", nTrack + 1, nFOV + 1, nDoc1 + 1);
						////SaveGrayScaleBMPFile(strTemp, pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_TeachBuffer[nFOV]);
					}
				}
			}
		}
	}
	UpdateData(FALSE);
}

void CStationMapping::OnSelchangeMotorIdCombo()
{
	UpdateData(TRUE);

	switch (_motorId) {
	case 0:
		_id = X_SERVO_MOTOR;
		_positiveMovementButton.LoadBitmaps(IDB_ARROWRIGHT, IDB_ARROWRIGHT_CLICKED);
		_negativeMovementButton.LoadBitmaps(IDB_ARROWLEFT, IDB_ARROWLEFT_CLICKED);
		break;

	case 1:
		_id = Y_SERVO_MOTOR;
		_positiveMovementButton.LoadBitmaps(IDB_ARROWUP, IDB_ARROWUP_CLICKED);
		_negativeMovementButton.LoadBitmaps(IDB_ARROWDOWN, IDB_ARROWDOWN_CLICKED);
		break;

	case 2:
		_id = Z_SERVO_TOP_MOTOR;
		_positiveMovementButton.LoadBitmaps(IDB_ARROWUP, IDB_ARROWUP_CLICKED);
		_negativeMovementButton.LoadBitmaps(IDB_ARROWDOWN, IDB_ARROWDOWN_CLICKED);
		break;

	case 3:
		_id = Z_SERVO_BOTTOM_MOTOR;
		_positiveMovementButton.LoadBitmaps(IDB_ARROWUP, IDB_ARROWUP_CLICKED);
		_negativeMovementButton.LoadBitmaps(IDB_ARROWDOWN, IDB_ARROWDOWN_CLICKED);
		break;

	default:
		break;
	}

	UpdateData(FALSE);

}

void CStationMapping::RelativeMove(int axisId, int direction)
{
	// 1. Stop current movement
	// 2. Move
	int nTrack = m_ctrlCameraID.GetCurSel();
	double motorResolution = 0.0;

	theApp.motionParameters.GetMotorResolution(axisId, motorResolution);
	UpdateData(TRUE);

	double _relativeMoveMm = atof(m_strZMotorRelativeMoveDisp);

	int motorMoveRelativePos = Round(_relativeMoveMm * motorResolution);

	UpdateData(FALSE);

	int nError = theApp.motionParameters.StopAxis(axisId);
	if (nError)
		return;

	Sleep(50);

//	_motorMoving = true;

	nError = theApp.motionParameters.RelativeMove(axisId, direction * motorMoveRelativePos, theApp.motionParameters.getMovingSpeed(axisId), true);
	if (nError) {
//		_motorMoving = false;
		return;
	}

//	if (!nError && m_bSnapEnable) {
		theApp.m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure(TRUE, FALSE, FALSE, TRUE);
//		m_bSnapEnable = FALSE;
//	}

//	_motorMoving = false;
}

void CStationMapping::RedrawPositions()
{
	int nMotorPos;
	double dMotorResolution;
	CString strMotorPos, strSensorPos;
	int nDataAvail = 1;
	double dData = 0.0;
	vector <double> zValues;

	theApp.motionParameters.GetCurrentMotorPosition(_motorId, nMotorPos);
	theApp.motionParameters.GetMotorResolution(_motorId, dMotorResolution);

	strMotorPos.Format("%d", nMotorPos);
	ctrlMotorPos.SetWindowText(strMotorPos);
	ctrlMotorPos.UpdateData(FALSE);

}

void CStationMapping::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	SetTimer(STATION_MAPPING_TIMER, 250, NULL);

	// TODO: Add your message handler code here
}


void CStationMapping::OnBnClickedButtonStartPosition()
{
	int nColNo;
	int nRowNo;

	CString str;
	str.Format("Confirm to Save FOV %d as Start Position?", m_nCurSelectedFovPosID);
	if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) == IDYES) {
	
		theApp.motionParameters.GetDieGrabNumber(nColNo, nRowNo);
		m_nStartScanFOV = m_nCurSelectedDieMapPosID;
		theApp.motionParameters.setStartScanFOVPosition(m_nStartScanFOV / nColNo, m_nStartScanFOV % nColNo);
	}
}


void CStationMapping::OnBnClickedButtonMoveToStartPos()
{
	if (theApp.m_pMotionControl != NULL) {
		theApp.motionParameters.MoveToStartScanPos();
	}
}


void CStationMapping::OnBnClickedButtonSetStartPosition()
{
	if (IDYES == AfxMessageBox("Do you want to set current position as START Position?", MB_YESNO)) {
		if (theApp.m_pMotionControl != NULL) {
			theApp.motionParameters.SetStartScanPos();
		}
	}
}
