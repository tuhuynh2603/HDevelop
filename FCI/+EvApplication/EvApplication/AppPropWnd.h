/// AppPropWnd.h : header file
/// Udupa; April'2012

#pragma once

#include "EvPropertiesWnd.h"

class CAppDoc;

class CAppPropWnd : public CEvPropertiesWnd
{
// Construction
public:
	CAppPropWnd();
	void Initialize();
	void InitPropList(CEvImageDoc* pDoc, int nFOV);
	void RefreshPropList(CEvImageDoc* pDoc, int nFOV);

// Attributes
public:

// Implementation
	virtual void SaveProperties();
	virtual void SaveAllProperties();
	virtual void SelChangedCamerasCombo();
	virtual void SelChangedCamerasComboPos();
	virtual void LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue);
	virtual void SetCurView(CEvImageDoc* pDoc);

protected:
//	DECLARE_DYNCREATE(CAppPropWnd)

	DECLARE_MESSAGE_MAP()
};

