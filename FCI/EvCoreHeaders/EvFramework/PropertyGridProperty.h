/// PropertyGridProperty.h : interface of the CPropertyGridProperty
/// Udupa; April'2012

#pragma once

#include "AfxPropertyGridCtrl.h"

class CPropertyGridProperty : public CMFCPropertyGridProperty
{
public:
	CPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData=0, BOOL bIsValueList=FALSE);
	CPropertyGridProperty(const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr=NULL,
		DWORD_PTR dwData=0, LPCTSTR lpszEditMask=NULL, LPCTSTR lpszEditTemplate=NULL, LPCTSTR lpszValidChars=NULL);

	CString m_strFormat;

	//virtual BOOL TextToVar(const CString& strText);
	virtual CString FormatProperty();
	virtual BOOL OnEndEdit();

	void SetLimits(int nMin, int nMax);

protected:
	int m_nMin;
	int m_nMax;
};

