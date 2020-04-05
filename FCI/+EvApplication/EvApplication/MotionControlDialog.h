/// CMotionControlDialog dialog
/// Udupa; Dec'2016

#pragma once
#include "ScanningTeachDialog.h"
#include "resource.h"
#include "position_info.h"
#include "vector"


class MotionParameters;
#define NO_OF_AXIS 4

class CMotionControlDialog : public CDialog
{
	DECLARE_DYNAMIC(CMotionControlDialog)

public:
	BOOL * bChecked;

	CMotionControlDialog(MotionParameters * pMotionParmsInit = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMotionControlDialog();

	// Dialog Data
	enum { IDD = IDD_MOTOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP();

public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	void DisplayPos();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedHome();
	afx_msg void OnBnClickedMoveToMotorPos();
	afx_msg void OnBnClickedPosMovement();
	afx_msg void OnBnClickedNegMovement();
	afx_msg void OnCbnSelchangeMotorIdCombo();
	afx_msg void OnBnClickedMotorOff();
	afx_msg void OnBnClickedStopMotors();
	afx_msg void OnBnClickedHomeAll();
	afx_msg void OnBnClickedApplyParameters();
	afx_msg void OnBnClickedSetupScan();
	afx_msg void OnBnClickedRelativeMove();
	afx_msg void OnBnClickedRelativeMoveNegative();
	afx_msg void OnEnChangeHomeStartVel();
	afx_msg void OnEnChangeHomeMaxVel();
	afx_msg void OnEnChangeMoveStartVel();
	afx_msg void OnEnChangeScanStartVel();
	//afx_msg void OnEnChangeScanStartVelTHR();
	afx_msg void OnEnChangeMoveMaxVel();
	afx_msg void OnEnChangeScanMaxVel();
	//afx_msg void OnEnChangeScanMaxVelTHR();

	afx_msg void OnEnChangeHomeStartVelMm();
	afx_msg void OnEnChangeHomeMaxVelMm();
	afx_msg void OnEnChangeMoveStartVelMm();
	afx_msg void OnEnChangeScanStartVelMm();
	afx_msg void OnEnChangeScanStartVelMmTHR();
	afx_msg void OnEnChangeMoveMaxVelMm();
	afx_msg void OnEnChangeScanMaxVelMm();
	afx_msg void OnEnChangeScanMaxVelMmTHR();
	//afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEnChangeSettlingTime();
	afx_msg void OnBnClickedStopMotor();
	afx_msg void OnBnClickedReloadSettings();
	afx_msg void OnBnClickedResetAlarm();
	afx_msg void OnEnChangeMotorMovePosition();
	afx_msg void OnEnChangeMotorRelativeMovePosition();
	afx_msg void OnEnChangeMotorMovePositionMm();
	afx_msg void OnEnChangeMotorRelativeMovePositionMm();

	afx_msg void OnCancel();
	afx_msg void OnClose();


	//	afx_msg void OnBnClickedLinearEncoderPositionVerification();

	//	afx_msg void OnBnClickedLinearEncoderVariationVerification();

	virtual BOOL OnInitDialog();

	void EnableWindows(const BOOL bEnable);

	void ResetValues();

	//	afx_msg void OnBnClickedVerifyMultiplePositions();

private:
	CListCtrl m_ctrlMotorPos;
	//	CListCtrl m_ctrlLinearEncoderPos;
	CComboBox m_cmbMotorID;

	std::vector <int> motorPos;
	//	std::vector <int> scalePos;
	std::vector <int> previousMotorPos;
	//	std::vector <int> previousScalePos;
	std::vector <int> previousMotorHomed;

	bool bCreateHeader;

	bool bRunThread;

	CFont m_FontList;

	//-----
	Speed homeSpeed;
	Speed moveSpeed;
	Speed scanSpeed;
	Speed scanSpeedTHR;
	int m_nHomeORGOffset;

	CString strMotorPos;
	int m_nMotorMovePos;
	int m_nMotorMoveRelativePos;

	CEdit ctrlMotorPos;
	double m_dMotorMovePosMM;
	double m_dMotorMoveRelativePosMM;

	BOOL m_bMotorOff;
	int m_nMotorID;
	int m_nMotorIDPrev;
	int m_nMotorSettlingTime;

	BOOL m_bForwardMove;
	BOOL m_bForwardHome;

	BOOL m_bContinuousMove;
	//	BOOL m_bHomeCheckByPass;

	MotionParameters * pMotionParms;
	MotionParameters * pMotionParmsInitial;

	int nMotionCommand;

	ScanningTeachDialog * pScanningTeachDialog;

	CString m_strMotorLimitAlarm;
	BOOL bMotorLimitAlarm;

	double m_dHomeStartVelMM;
	double m_dHomeMaxVelMM;
	double m_dMoveStartVelMM;
	double m_dMoveMaxVelMM;
	double m_dScanStartVelMM;
	double m_dScanMaxVelMM;
	double m_dScanStartVelMMTHR;
	double m_dScanMaxVelMMTHR;

	double m_dHomeStartVelKM;
	double m_dMoveStartVelKM;
	double m_dScanStartVelKM;
	double m_dScanStartVelKMTHR;
	double m_dHomeMaxVelKM;
	double m_dMoveMaxVelKM;
	double m_dScanMaxVelKM;
	double m_dScanMaxVelKMTHR;

	BOOL bMotorServoStatus;

	CBitmapButton m_NegMovement;
	CBitmapButton m_PosMovement;

	CBitmapButton m_NegRelativeMovement;
	CBitmapButton m_PosRelativeMovement;

	CStatic m_ctrlMotorResolution;

	bool bActiveTimer;

	// Use to GetMotorParameters
	int nTotalMotorNo;
	//	int nTotalScaleNo;

	// Use to check edit box update
	BOOL bEditBoxUpdate[2];	// 0 - value without MM, 1 - value with MM (do not allow update both at same time, only allows update per time)

							//	std::vector < std::vector < int > > linearEncoderPosition;
	size_t linearEncoderPositionCounter;
	//	bool verifyLinearEncoderPosition;


	//	bool verifyEncoderVariation;
	//	bool displayEncoderVariation;
	std::vector < int > linearEncoderVariation;
	size_t linearEncoderVariationId;

	bool moveMotorForVerification;
	size_t currentVerificationMotorPositionId;
	bool moveMotorToNextVerificationPosition;
	bool moveXMotorForVerification;

	int InitializeListControlHeaders();
	void RedrawMotorPosStatus();

	void moveVerificationMotor();

	//*****************
	void Move(int nAxis, int nDirection);
	void UpdateParameters();
	void StopEveryMotor();
	void MotorStopped();

	void evaluatePositionNoise(size_t encoderId, double & mean, double & sigma, int & minValue, int & maxValue);

	void OnTimer(UINT_PTR nIDEvent);

	void _resetValidation();
	double _verificationMotorStepMm;

	bool motionIOGood[9];
	COLORREF clListControlWhite;
	COLORREF clListControlGreen;
	COLORREF clListControlRed;

	std::vector <std::vector <bool >  > motionIoStatus;
	std::vector <long> prevIoStatus;
	std::vector <long> currentIoStatus;

	void RedrawIOStatus();

public:
	afx_msg void OnCustomdrawMotorPosList(NMHDR *pNMHDR, LRESULT *pResult);
};
