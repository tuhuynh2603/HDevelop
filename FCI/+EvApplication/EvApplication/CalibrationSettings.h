#pragma once
#include "resource.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "CalibInfo.h"
#include "PictureCtrl.h"

// CCalibrationSettings dialog

class CCalibrationSettings : public CDialog
{
	DECLARE_DYNAMIC(CCalibrationSettings)

public:
	CCalibrationSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCalibrationSettings();
	CApplication *m_pApp;
	int nTrack;
	//int nDoc;
	int nSelDoc;

	int nCalibGreyValue;
	CPictureCtrl m_ctrlDieImages[4];
	CCalibInfo dlgCalibInfo;
// Dialog Data
	enum { IDD = IDD_CALIBRATIONSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCalibClose();
	afx_msg void OnBnClickedCalibIntensity1();
	afx_msg void OnBnClickedCalibIntensity2();
	afx_msg void OnBnClickedCalibIntensity3();
	afx_msg void OnBnClickedCalibIntensity4();
	afx_msg void OnBnClickedCalibIntensity5();
	//afx_msg void OnBnClickedMfcbutton3();
	void GoToCalibration(int nDoc, BOOL bDotCalibration);
	void Initialize(int nDoc);
	afx_msg void OnBnClickedCalibIntensity6();
	void SaveStartPositionCalib(BOOL bSave);		//// Save Start position 
	int m_nXAxisPosition[MAX_TRACKS][5];
	int m_nYAxisPosition[MAX_TRACKS][5];
	int m_nZAxisPositionTop[MAX_TRACKS][5];
	int m_nZAxisPositionBottom[MAX_TRACKS][5];
	BOOL m_bEnableLightCorrection[5];

	int m_nDotXAxisPosition;
	int m_nDotYAxisPosition;
	int m_nDotZAxisPositionTop;
	int m_nDotZAxisPositionBottom;
	afx_msg void OnBnClickedButton1();
	void SaveMotionParameters(CString strPath,BOOL bIsDotCalibration);
	void LoadMotionParameters(CString strPath, BOOL bIsDotCalibration);
	afx_msg void OnBnClickedButtonApply();
};
