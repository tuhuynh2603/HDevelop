#pragma once
#include "resource.h"

// CCalibInfo dialog

class CCalibInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CCalibInfo)

public:
	CCalibInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCalibInfo();

// Dialog Data
	enum { IDD = IDD_CALIBINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nCalibGreyValue;
	int m_nCalibGreyVal[2];
	int m_nStepXCount;
	int m_nStepYCount;
	double m_dStepXSize;
	double m_dStepYSize;
	int m_nCalibValueY;
	int m_nMinRadiusDotTarget;
	BOOL m_bDotCalib;
	CFont m_fontHead;
	CString strCalibrationName;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStartPos();
};
