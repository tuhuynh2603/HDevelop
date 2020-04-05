/// EvPropertiesWnd.h : interface of the CEvPropertiesWnd class
/// Udupa; April'2012

#pragma once

#include "PropertyList.h"

class CEvImageDoc;
class CParameters;
struct CParmInfo;

class CDefectPropertiesToolBar : public CMFCToolBar
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

class DECLSPEC_WINDOW_IF CEvDefectPropertiesWnd : public CDockablePane
{
// Construction
public:
	CEvDefectPropertiesWnd();

	void AdjustLayout();
	
	void InitGroup(CParameters* pParms, int nIndex);
	void SaveGroup(CParameters* pParms, int nIndex);
	
// Attributes
public:
	virtual void SaveProperties() {};
	virtual void SaveAllProperties() {};
	virtual void InitPropList() = 0;
	virtual void RefreshPropList();

	virtual void OnPressCloseButton();
	virtual void OnAfterDockFromMiniFrame();


	void RevertProperties();
	virtual void LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue) {};

	void SetVSDotNetLook(BOOL bSet)
	{
		for(int i=0; i<m_nSets; i++) {
			m_wndPropLists[i].SetVSDotNetLook(bSet);
			m_wndPropLists[i].SetGroupNameFullWidth(bSet);
		}
	}


protected:
	CFont m_fntPropList;
	CDefectPropertiesToolBar m_wndToolBar;
	CPropertyList* m_wndPropLists;

	int m_nSets;
	int m_nCurSet;
	BOOL m_bIsModified;
	double m_dPixelSizeX;
	double m_dPixelSizeY;

	int m_nUnitIndex;

	void SetModified(BOOL bModified);

// Implementation
public:
	virtual ~CEvDefectPropertiesWnd();

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

	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

	void SetPropListFont();
public:
	afx_msg void OnClose();
};

