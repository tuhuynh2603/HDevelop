/// EvHistoryTabView.h : interface of the CEvHistoryTabView class
/// Udupa; April'2012

#pragma once
#include "EvTabView.h"

class CEvHistoryTabView : public CEvTabView
{
protected:
	DECLARE_DYNCREATE(CEvHistoryTabView)

public:
	virtual void OnInitialUpdate();
	CImageBuffer* m_pBuffer;
	int UpdateImage(CImageBuffer* pBuffer);
	void SetTitle(int nIndex, int nTrack);

protected:
	//{{AFX_MSG(CEvHistoryTabView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

