/// EvTabPane.h : interface of the CEvTabPane class
/// Udupa; August'2012

#pragma once

class CEvTabPane : public CDockablePane
{
public:
	CEvTabPane() {};
	virtual ~CEvTabPane() {};

	virtual void OnPressCloseButton();
	virtual void UpdateFonts() = 0;

	virtual void CloseChildren() = 0;
	virtual void OnAfterDockFromMiniFrame();
	//virtual BOOL OnBeforeFloat(CRect& rectFloat, AFX_DOCK_METHOD dockMethod);
};
