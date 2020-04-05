/// PropertyListPropertyGridProperty.h : interface of the CPropertyList
/// Udupa; April'2012

#pragma once

#include "AfxPropertyGridCtrl.h"

class CPropertyList : public CMFCPropertyGridCtrl
{
public:
	CPropertyList();
	~CPropertyList();

	BOOL m_bIsModified;
	BOOL m_bIsExpand;

	void SetLeftColumnWidth(int cx);
};

