/// Light Control Dialog: New interface for controlling all light heads in a single interface
/// Udupa; Sept'2012


#pragma once

class CLightControl;
class CLightHandler;

//// Rakshith; derived class for slider control; event handling for LButtonDown/Up;
class CCustomSliderCtrl : public CSliderCtrl
{
public:
	CCustomSliderCtrl();
	virtual ~CCustomSliderCtrl();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

	CApplication* pApp;
};

class CLightControlDlg : public CDialog
{
// Construction
public:
	CLightControlDlg(CWnd* pParent = NULL);
	~CLightControlDlg();

	CString m_strTitle;

	CApplication* m_pApp;
	CTrackHandler* m_pTrack;
	CLightControl* m_pLight;
	BOOL* m_bSelection;

	int m_nNoOfSegments;
	int m_nStartSegment;
	int m_nEndSegment;
	int nCalibGreyValue;
	int nCalibVariation;
	BOOL bDocChannels[8];
	int m_nLightValueChannels[8];
	int m_nLightPulseChannels[8];
	double m_nLightValuesDisp[8];
	int nCalibLightValue;
	CString m_strLightName;
	CString m_strLightTrackName;

	CRgn* m_prgnLightSegment;

	BOOL* m_bSkipSelection;
	BOOL* m_bActiveChannel;

	int m_nOldLightValueChannels[8];
	int m_nOldLightPulseChannels[8];

// Dialog Data
	//{{AFX_DATA(CLightControlDlg)
	enum { IDD = IDD_LIGHT_CONTROL_DLG };
	//CSliderCtrl	m_ctrlPulseWidthSlider;
	CCustomSliderCtrl m_ctrlPulseWidthSlider;
	BOOL m_bLButtonDownOnSlider;
	CScrollBar	m_ctrlLightScrollbar;
	CRect m_rectLightHead;
	int m_nLightValue;
	int m_nPulseWidth;
	//}}AFX_DATA
	//BOOL	m_bRecalcFactor;
	//int		m_nIndividualSeg;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CLightHandler* m_pLightHandler;
	CBrush m_brushActive;
	CBrush m_brushInActive;
	CBrush m_brushGrayed;
	CBrush m_brushBorder;

	void SelectSegments(int nStartSegment, int nEndSegment, BOOL bSelect);
	void SetCamera(BOOL bRefresh);

	// Generated message map functions
	//{{AFX_MSG(CLightControlDlg)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAllLightsOn();
	afx_msg void OnAllLightsOff();
	
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strLightValue;
	virtual void OnOK();
	virtual void OnCancel();
	void UpdateLightValue();
	void ApplyPulseWidth();
	//int SelectLightController(int nSelRgn);
	CString m_strCurrentRange;
	afx_msg void OnEnChangePulsewidthUs();
	afx_msg void OnRadioCam(UINT id);
	int m_nTrackId;
	int m_nDocId;
	int m_nFOV;
	afx_msg void OnBnClickedCheckLightselectall();
	int SetAutoLightIntensity(int nIsRange,int dGreyValueOut, BOOL bInitialize=FALSE);
	void SkipSegments(int nDoc);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
