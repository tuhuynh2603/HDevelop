#pragma once


// CChildDockablePane
class CChildFormView;
class CChildDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(CChildDockablePane)

public:
	CChildDockablePane();
	virtual ~CChildDockablePane();
	//CChildFormView m_pFormview;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lp);
	afx_msg void OnSize(UINT nType,int cx,int cy);
};


