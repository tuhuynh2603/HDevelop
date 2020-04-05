/// EvPropertiesWnd.h : interface of the CEvPropertiesWnd class
/// Udupa; April'2012

#pragma once

#include "PropertyList.h"

#define IDC_GRID_PROPERTIES_INSP	3010


class CEvImageDoc;
class CParameters;
struct CParmInfo;

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvPropertiesWnd : public CDockablePane
{
// Construction
public:
	CEvPropertiesWnd();

	void AdjustLayout();
	
	void InitGroup(IMAGEVIEW_INFO *pView, CParameters* pParms, int nIndex);
	void SaveGroup(CParameters* pParms, int nIndex);
	CEvImageDoc* m_pDoc;

	//CPropertyList* m_wndPropLists;
	CPropertyList* m_wndPropLists[20];  //MAX_FOV
// Attributes
public:
	virtual void SaveProperties() {};
	virtual void SaveAllProperties() {};
	virtual void InitPropList(CEvImageDoc* pDoc, int nFOV) = 0;
	virtual void RefreshPropList(CEvImageDoc* pDoc, int nFOV);
	virtual void SelChangedCamerasCombo() {};
	virtual void SelChangedCamerasComboPos() {};

	virtual void OnPressCloseButton();
	virtual void OnAfterDockFromMiniFrame();


	void RevertProperties();
	virtual void LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue) {};
	virtual void SetCurView(CEvImageDoc* pDoc) {};

	void SetVSDotNetLook(BOOL bSet)
	{
		/*for(int i=0; i<m_nSets; i++) {
			m_wndPropLists[i].SetVSDotNetLook(bSet);
			m_wndPropLists[i].SetGroupNameFullWidth(bSet);
		}*/
		for(int i=0; i<m_nPos; i++) {
			for(int j=0; j<m_nSets; j++) {
				m_wndPropLists[i][j].SetVSDotNetLook(bSet);
				m_wndPropLists[i][j].SetGroupNameFullWidth(bSet);
			}
		}
	}


protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CComboBox m_wndObjectComboPos;
	CPropertiesToolBar m_wndToolBar;
	//CPropertyList* m_wndPropLists;
	int m_nPos;
	int m_nSets;
	int m_nTopSets;
	int m_nBottomSets;
	int m_nSideSets;
	CStringArray strTopName;
	CStringArray strBottomName;
	CStringArray strSideName; 
	int m_nCurSet;
	int m_nCurPos;
	BOOL m_bIsModified;
	double m_dPixelSizeX;
	double m_dPixelSizeY;

	int m_nUnitIndex;

	void SetModified(BOOL bModified);

// Implementation
public:
	virtual ~CEvPropertiesWnd();

protected:
//	DECLARE_DYNCREATE(CEvPropertiesWnd)

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnRevertProperties();
	afx_msg void OnUpdateRevertProperties(CCmdUI* pCmdUI);
	afx_msg void OnSaveProperties();
	afx_msg void OnUpdateSaveProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSelchangeCamerasCombo();
	afx_msg void OnSelchangeCamerasComboPos();

	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

	void SetPropListFont();
public:
	afx_msg void OnClose();
};

