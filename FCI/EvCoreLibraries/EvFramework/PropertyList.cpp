/// PropertyList.cpp : implementation of the CPropertyList class
/// Udupa; April'2012

#include "StdAfx.h"
#include "PropertyList.h"


CPropertyList::CPropertyList()
{
	m_bIsModified = FALSE;
	m_bIsExpand = FALSE;
}


CPropertyList::~CPropertyList()
{
}


void CPropertyList::SetLeftColumnWidth(int cx)
{
    m_nLeftColumnWidth = cx;
	AdjustLayout();
}