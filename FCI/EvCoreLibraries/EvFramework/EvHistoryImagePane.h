/// EvHistoryImageView.h : interface of the CEvHistoryImageView class
/// Udupa; April'2012

#pragma once

#include "EvTabView.h"
#include "EvTabPane.h"

/////////////////////////////////////////////////////////////////////////////
// CEvHistoryImageView window

class CEvHistoryImageView : public CEvTabView
{
// Construction
public:
	CEvHistoryImageView();

// Implementation
public:
	virtual ~CEvHistoryImageView();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class CEvHistoryImagePane : public CEvTabPane
{
public:
	CEvHistoryImagePane();

	virtual void UpdateFonts();
	CImageBuffer m_Buffers[10];
	int m_nLastIndex;

	int m_nSourceImage;
	int m_nDepth;

	void PushImage(CImageBuffer* pBuffer);
	void AddImageView();
	void SetHistoryDepth(int nDepth);
	void AdjustLayout();
	CArray<CEvTabPage*, CEvTabPage*> m_arrImageDialogs;
	CArray<CEvSdiChildWnd*, CEvSdiChildWnd*> m_arrImageWnds;
	void SetTitles();
	void Clean(int nIndex);

protected:
	//CMFCTabCtrl	m_wndTabs;

	CArray<CEvImageDoc*, CEvImageDoc*> m_arrImageDocs;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

public:
	virtual ~CEvHistoryImagePane();
	virtual void CloseChildren();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//virtual BOOL OnInitDialog();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

