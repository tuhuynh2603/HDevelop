/// PropertyGridProperty.cpp : implementation of the CCPropertyGridProperty class
/// Udupa; April'2012

#include "StdAfx.h"
#include "PropertyGridColorProperty.h"


CPropertyGridColorProperty::CPropertyGridColorProperty(const CString& strName, const COLORREF& color, CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData)
	 : CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
{
}

void CPropertyGridColorProperty::ResetOriginalValue()
{
	CMFCPropertyGridProperty::ResetOriginalValue();
}
