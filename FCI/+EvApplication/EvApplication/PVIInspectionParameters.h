#pragma once
#include "resource.h"

// CPVIInspectionParameters dialog
class CPropertyGridProperty;
class CParameters;
class CPropertyGridColorProperty;

class CPVIInspectionParameters : public CDialog
{
	DECLARE_DYNAMIC(CPVIInspectionParameters)

public:
	CPVIInspectionParameters(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPVIInspectionParameters();

	CApplication* m_pApp;
	CTrackHandler* m_pTrack;
	CInspectionHandler* pInspHandler;
	CPVIInspectionSetup *pPviInspectionSetup;
	BOOL bChangePviArea;
	int m_nTrackId;
	int m_nDocId;
	int m_nNoOfFov;
	int nFOV;
	int m_nUnitIndex;
	int m_nIndex;
	double m_dPixelSizeX;
	double m_dPixelSizeY;

	
	CArray<CParameters*> m_PviParametersDefect;
	CArray<CParameters*> m_PviParametersInspSetUP;
	

	CParameters* pParms;

	CParameters* pPviParms;
	CParameters* pPviParmsInspSetup;
	
	
	int nSize;
	BOOL** m_bPVIDefectImgEnable;
	void ClearAllPVIDefectImg(BOOL bInitialize=FALSE);

// Dialog Data
	enum { IDD = IDD_PVI_INSPECTION_PARM };
	int nMinSize;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ComboPVIDefect;
	CString m_strPVIDefect;
	CMFCPropertyGridCtrl m_propPVIDefect;
	afx_msg void OnBnClickedPviAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPviDefect();
	void Initialize(int nFov);
	CStringArray strArrayAreaType;
	CStringArray strArrayDefectSel;
	afx_msg void OnBnClickedPviDelete();
	afx_msg void OnBnClickedOk();
	void SaveGroup(CParameters* pParms, int nIndex,CMFCPropertyGridCtrl *m_propWindow);
	void InitGroup(CParameters* pParms, int nIndex,CMFCPropertyGridCtrl *m_propWindow,BOOL bDefect = FALSE);
	void LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue);
	void UpdateEditPVIArea();
//	CComboBox m_PviInspectionArea;
	CComboBox m_ComboPviArea;
	CComboBox m_ComboInspectionArea;
//	CString m_strNoOfPviArea;
	CMFCPropertyGridCtrl m_propInspectionSetup;
	int m_nPviArea;
	afx_msg void OnEnChangeEditPviArea();
	afx_msg void OnCbnSelchangeComboPviArea();
	CComboBox m_ComboFov;
	afx_msg void OnCbnSelchangeComboPviFov();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboIntensity;
	afx_msg void OnCbnSelchangeComboIntensity();
	afx_msg void OnBnClickedCheckPviDefectImgEnable();
	
	void CleanPviParms();
};


