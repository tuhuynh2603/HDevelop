/// PropertyGridProperty.cpp : implementation of the CCPropertyGridProperty class
/// Udupa; April'2012
/// Udupa; Added limit validation; April'2013

#include "StdAfx.h"
#include "PropertyGridProperty.h"


CPropertyGridProperty::CPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData, BOOL bIsValueList) : CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
	m_strFormat = "";
	m_nMin = 0;
	m_nMax = -1;
}

CPropertyGridProperty::CPropertyGridProperty(const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr,
		DWORD_PTR dwData, LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	m_strFormat = "";
}

//BOOL CPropertyGridProperty::TextToVar(const CString& strText)
//{
//	CString strVal = strText;
//	if(m_varValue.vt==VT_R8) {
//		double dblVal = 0.;
//
//		strVal.TrimLeft();
//		strVal.TrimRight();
//
//		if (!strVal.IsEmpty())
//		{
//			_stscanf_s(strVal, "%lf", &dblVal);
//		}
//
//		m_varValue = dblVal;
//
//		return TRUE;
//	}
//		
//	return CMFCPropertyGridProperty::TextToVar(strText);
//}
//

CString CPropertyGridProperty::FormatProperty()
{
	COleVariant& var = m_varValue;
	if(m_strFormat.IsEmpty() || (var.vt != VT_R8))
		return CMFCPropertyGridProperty::FormatProperty();

	else {
		ASSERT_VALID(this);
		ASSERT_VALID(m_pWndList);

		CString strVal;
		strVal.Format(m_strFormat, (double)var.dblVal);
		return strVal;
	}
}

void CPropertyGridProperty::SetLimits(int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;
}

BOOL CPropertyGridProperty::OnEndEdit()
{
	CMFCPropertyGridProperty::OnEndEdit();

	if(m_nMin > m_nMax)
		return TRUE;

	if(m_varValue.vt == VT_I4) {
		if(m_varValue.intVal < m_nMin) {
			m_varValue.intVal = m_nMin;
			Redraw();
		}
		else if(m_varValue.intVal > m_nMax) {
			m_varValue.intVal = m_nMax;
			Redraw();
		}
	}
	else if(m_varValue.vt == VT_R8) {
		if(m_varValue.dblVal < m_nMin) {
			m_varValue.dblVal = m_nMin;
			Redraw();
		}
		else if(m_varValue.dblVal > m_nMax) {
			m_varValue.dblVal = m_nMax;
			Redraw();
		}
	}
	
	return TRUE;
}
