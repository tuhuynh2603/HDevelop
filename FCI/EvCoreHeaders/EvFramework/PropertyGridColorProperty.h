/// PropertyGridProperty.h : interface of the CPropertyGridProperty
/// Udupa; April'2012

#pragma once

#include "AfxPropertyGridCtrl.h"

class CPropertyGridColorProperty : public CMFCPropertyGridColorProperty
{
public:
	CPropertyGridColorProperty(const CString& strName, const COLORREF& color, CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	virtual void ResetOriginalValue();
};

