/// EVStatsPane.h : interface of the CEVStatsPane class
/// Udupa; November'2012

#pragma once
#include "EvTabPane.h"

class CEvStatsTabPage;

class CEVStatsPane : public CEvTabPane
{
public:
	CEVStatsPane(CEvMainFrame* pMainFrame);

	virtual void OnPressCloseButton() { };

	virtual void UpdateFonts();
	void AdjustLayout();

	CEvImageDoc* m_pImageDoc;
	CEvSdiChildWnd* m_pImageWnd;
	CEvStatsTabPage* m_pImageDialog;

	virtual ~CEVStatsPane();
	virtual void CloseChildren();
	void AddImageView();
	int OnSelection(int nTrack, int nIndex, CFailImageStack* pFailure);
	CImageBuffer m_Buffer;
	void OnRadioStatsType(UINT id);
	void OnRadioStatsCam(UINT id);

	CEvMainFrame* m_pMainFrame;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStatsImageFirstButton();
	afx_msg void OnBnClickedStatsImagePrevButton();
	afx_msg void OnBnClickedStatsImageNextButton();
	afx_msg void OnBnClickedStatsImageLastButton();
	afx_msg void OnBnClickedStatsDatarefreshButton();
	afx_msg void OnLbnSelchangeStatsCharttypeList();
};
