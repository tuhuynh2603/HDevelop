/// EvDebugViewPane.h : interface of the CEvDebugViewPane class
/// Udupa; April'2012

#pragma once

#include "EvTabPane.h"

class CEvDebugViewPane : public CDockablePane
{
public:
	CEvDebugViewPane();
	BOOL bIsClosed;
	virtual void UpdateFonts();
	void AdjustLayout();

	CDialog* m_pMessageDialog;

public:
	virtual ~CEvDebugViewPane();
	virtual void CloseChildren();
	virtual void OnPressCloseButton();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

