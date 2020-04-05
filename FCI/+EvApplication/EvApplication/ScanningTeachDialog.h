#pragma once

class MotionParameters;
// ScanningTeachDialog dialog

class ScanningTeachDialog : public CDialog
{
	DECLARE_DYNAMIC(ScanningTeachDialog)

public:
	ScanningTeachDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ScanningTeachDialog();
	//	CApplication* m_pApp;

	MotionParameters * pMotionParms;
	MotionParameters * pMotionParmsInitial;


	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCANNING_TEACH_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetStartPos();
	afx_msg void OnBnClickedMoveToStartPos();

	void SetMotionParameters(MotionParameters * pMotionParmsIn, MotionParameters * pMotionParmsInitialIn);
	afx_msg void OnBnClickedSetZreadyPos();
	afx_msg void OnBnClickedMoveToZreadyPos();
	afx_msg void OnBnClickedSetZreadyPos2();
	afx_msg void OnBnClickedMoveToZreadyPos2();
	afx_msg void OnBnClickedOk();
};
