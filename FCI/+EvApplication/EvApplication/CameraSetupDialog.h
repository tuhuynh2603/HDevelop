#pragma once
#include "afxcmn.h"

class CTrackHandler;

// CCameraSetupDialog dialog

class CCameraSetupDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraSetupDialog)

public:
	CCameraSetupDialog(CTrackHandler* pTrack);
	virtual ~CCameraSetupDialog();
	
	CString m_strTitle;

	CTrackHandler* m_pTrack;

// Dialog Data
	enum { IDD = IDD_CAMERA_SETUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nExposure;
	int m_nGains[3];
	int m_nGain;
	int m_nBlackLevel;
	CSliderCtrl m_sliderExposure;
	CSliderCtrl m_sliderGain;
	CSliderCtrl m_sliderBlackLevel;
	afx_msg void OnEnChangeExposureEdit();
	afx_msg void OnEnChangeGainEdit();
	afx_msg void OnEnChangeBlackLevelEdit();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCamerasetupGainRadio();
	int m_nGainMode;
};
