
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList window

class COutputList : public CListBox
{
	// Construction
public:
	COutputList();

	// Implementation
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();
	afx_msg void OnSaveToFile();
	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
	// Construction
public:

	COutputWnd();
	COutputList m_wndOutputEvent;
	COutputList m_wndCommEvent;
	COutputList m_wndDataLog;
	BOOL bIsClosed;
	int m_nMaxLines;
	void UpdateFonts();
	int AddToEventLogPane(CString strMsgLog, int nEventLog);
	// Attributes
protected:
	CMFCTabCtrl	m_wndTabs;
protected:


	/*int AddToMotionLog(CString strDescription);
	int AddToLightLog(CString strDescription);
	int AddToParmLog(CString strDescription);*/
	void AdjustHorzScroll(CListBox& wndListBox);

	// Implementation
public:
	virtual ~COutputWnd();
	virtual void OnPressCloseButton();
	afx_msg void OnClose();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

