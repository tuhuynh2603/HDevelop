#pragma once
#include "resource.h"
#include "afxwin.h"

// CSaveCalibRationPosition dialog

class CSaveCalibRationPosition : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveCalibRationPosition)

public:
	CSaveCalibRationPosition(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveCalibRationPosition();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALIB_MOTION_SAVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CButton m_bCheckedDotCalibration;
	BOOL m_bCheckedIntensity[5];
	BOOL m_bSelectedDotCalibration;
	int nTrack;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioDotCalibration();
	afx_msg void OnBnClickedRadioLightCorrection();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
