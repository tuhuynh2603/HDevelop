/// MainFrm.h : interface of the CMainFrame class
/// Udupa; April'2012

#pragma once
#include "AppPropWnd.h"
#include "AppTeachPropWnd.h"
#include "AppDefectPropWnd.h"
#include "Resource.h"
#include "EvMainFrame.h"
#include "OpticsDlg.h"

class CDialogPaneStatsview;
class CMainFrame : public CEvMainFrame
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	~CMainFrame();
	void RecalculateLayout();
	void DisplayOpticsPanel(BOOL bVisible, int nHeight = 100);
	
	COpticsDlg m_wndOpticsPanel;
	void OnMaximizeDoc(BOOL bTrue);
	CRect rectStatsPane;
	CDialogPane* m_dlgPane;
	CDialogPaneStatsview* m_dlgPaneStats;
	
	afx_msg void ShowPreviousImages();
	afx_msg void ShowStatsPane();

	afx_msg void OnUpdateShowPreviousImages(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowStatsPane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowOutputLog(CCmdUI *pCmdUI);
// Generated message map functions
protected:
 	afx_msg void OnClose();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	virtual void InitializeApp();
	virtual int CreateRibbon();
	virtual void ResizeChildren();
	virtual void ShowMainView(BOOL bShow);
	virtual CDockablePane* GetDebugPane();
	virtual void ToggleMulticast();
	virtual BOOL IsNewTabAvailable();
	virtual void ShowHideStatPane(BOOL bShow);
	virtual void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual void OnUpdateRibbonTab();

	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnLoadServerRecipe(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadRecipe(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadSingleFovRecipe(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHidePanes(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSendLightValue(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowMotorInitializing(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetOnlineStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCurPosParamDisplay(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewFullScreen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddEventLogPane(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateRecipeName(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnResizeDieMapping(WPARAM wParam, LPARAM lParam);

};

