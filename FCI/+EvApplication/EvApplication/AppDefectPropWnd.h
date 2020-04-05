/// AppPropWnd.h : header file
/// Udupa; April'2012

#pragma once

#include "EvDefectPropertiesWnd.h"

class CAppDoc;

class CAppDefectPropWnd : public CEvDefectPropertiesWnd
{
// Construction
public:
	CAppDefectPropWnd();
	void Initialize();
	void InitPropList();
	void RefreshPropList();

// Attributes
public:

// Implementation
	virtual void SaveProperties();
	virtual void SaveAllProperties();
	virtual void LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue);

protected:
//	DECLARE_DYNCREATE(CAppPropWnd)

	DECLARE_MESSAGE_MAP()
};

