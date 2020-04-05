#pragma once
#include "resource.h"
#include "afxwin.h"
#include "Application.h"
#include "LightHandler.h"
#include "LightControl.h"
#include "PictureCtrl.h"
#define MAX_DIE_FOVS		25//40
#define MAX_DIE_MAP_FOVS	8

// CStationMapping dialog

class CStationMapping : public CDialogEx
{
	DECLARE_DYNAMIC(CStationMapping)

public:
	CStationMapping(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStationMapping();

// Dialog Data
	enum { IDD = IDD_STATION_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CApplication* pApp;

	CRect *m_diePicCtrl;
	CRect* m_prgnPositionSelection;
	

	CRect rcClientWnd;
	int nClientHeight;
	int nClientWidth;

	int nScrollBarHMax;
	int nScrollBarVMax;

	int nScrollBarHMaxFov;
	int nScrollbarVMaxFov;

	int nHorizontalScrollBar;
	int nVerticalScrollBar;

	CScrollBar scrollBarDeviceH;
	CScrollBar scrollBarDeviceV;

	CRect scrollBarDeviceHRect;
	CRect scrollBarDeviceVRect;

	CScrollBar scrollBarDieH;
	CScrollBar scrollBarDieV;

	CRect scrollBarDieHRect;
	CRect scrollBarDieVRect;

	CBrush brushCell[3];

	CBrush brushBackground;

	CBrush brushSelectedCell;

	BOOL bRedrawing;

//	CRect * prcDie;
//	int * pnDieState;



	BOOL* m_bPosSelection;
	BOOL* m_bDieMapPosSel;

	CBrush m_brushCheck;
	CBrush m_brushUncheck;
	CBrush m_brushBorder;

	CComboBox m_ctrlCameraID;
	CListBox m_ctrlZLevelMap;
	CListBox m_ctrlDieZLevelID;

	CRect rcDeviceMappingRelative;
	CRect rcDieRelative;
	int m_nDeviceSelectionCount;
	int m_nDieSelectionCount;
	int m_nCurSelectedDieMapPosID;
	int m_nCurSelectedFovPosID;
	int nDieRectSize;
	int nZoom;
	int nPrevZoom;
	int nMouseSelectedDie;
	int m_nStartScanFOV;

	//double m_dZMotorRelativeMoveDisp;
	CString m_strZMotorRelativeMoveDisp;
	int m_nZMotorPosDisp;
	CEdit ctrlMotorPos;
	double m_dZMotorScanMaxDisp;

	double m_dZMotorRelativeMove[3];
	int m_nZMotorPos[3];
	double m_dZMotorScanMax[3];
	BOOL m_bRelativeCheck[3];

	BOOL m_bLightDlgOpen;
	CStringArray m_strDieMapContents[MAX_TRACKS];//// max. 3 tracks;
	CStringArray m_strDiePosIDContents[MAX_TRACKS][MAX_FOV];//// max. 3 tracks; max. 8 FOVs 
	CStringArray m_strPulseValue[MAX_TRACKS];//// max. 3 tracks;

	void Initialize();
	void StationMappingFile(BOOL bRead=TRUE);
	void UpAndDownZMotorServoButton(BOOL bEnable);
	void ResetListBoxContents(int nFovPos = 0);
	void ScrollBarEnable(BOOL bHorizontal);
	void NavigateListBoxContents(int nRow, BOOL bDown, BOOL bKeyDown=FALSE);
	void MoveToFOVPosAndSetLight();
	void RemoveFromDieMapPosId(int nTrack, int nSel);
	void ClearAll();

	void MoveXYToPosition(int nTrack, int nFOV, BOOL bHome);
	int SetZPositionValues(int nTrack, int nFOV, int nDoc, CString strSel, int nZValue);
	void SetLightValues(int nTrack, int nDocCount, CString strSel, CString strPulse);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnSelchangeSmCamidCombo();
	afx_msg void OnBnClickedSmSelectallButton();

	afx_msg void OnBnClickedSmDieMapAddMfcbutton();
	afx_msg void OnBnClickedSmDieMapRemoveMfcbutton();
	afx_msg void OnBnClickedSmDieMapRetakeMfcbutton();
	//afx_msg void OnDblclkSmDieMapList();
	afx_msg void OnSelchangeSmDieMapList();
	afx_msg void OnBnClickedSmDieMapSwapMfcbutton();
	afx_msg void OnBnClickedSmDieMapPosmoveSetlightButton();
	afx_msg void OnBnClickedSmDieMapPosidRemoveMfcbutton();
	afx_msg void OnBnClickedSmDieMapLivecamLightButton();

	afx_msg void OnBnClickedSmSaveAllTeachButton();
	afx_msg void OnBnClickedSmSaveOneTeachButton();
	afx_msg void OnBnClickedSmLoadTeachButton();
	afx_msg void OnBnClickedSmSetImgIdButton();
	afx_msg void OnUpdateSmStartEdit();
	afx_msg void OnUpdateSmMaxEdit();

	afx_msg void OnUpdateSmZMtrServoEdit();
	//afx_msg void OnClickedSmZMtrServoCheck();
	afx_msg void OnBnClickedSmZMtrServoUpMfcbutton();
	afx_msg void OnBnClickedSmZMtrServoDownMfcbutton();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();

	afx_msg void OnBnClickedSmNeglectMfcbutton();
	afx_msg void OnBnClickedSmSaveTeachImageAllButton();
	CComboBox _motorIdCombobox;
	int _motorId;
	int _id;
	CBitmapButton _negativeMovementButton;
	CBitmapButton _positiveMovementButton;
	afx_msg void OnSelchangeMotorIdCombo();

	void RelativeMove(int axisId, int direction);
	void RedrawPositions();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonStartPosition();
	afx_msg void OnBnClickedButtonMoveToStartPos();
	afx_msg void OnBnClickedButtonSetStartPosition();
};
