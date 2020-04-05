#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxpropertygridctrl.h"
#include "ImageSelectionDlg.h"
#include <map>

// CDefectCodesDlg dialog

class CCheckBoxProp : public CMFCPropertyGridProperty
{
public:
	CCheckBoxProp(const CString& strName, BOOL bCheck, LPCTSTR lpszDescr = NULL, DWORD dwData = 0);

protected:
	virtual BOOL OnEdit(LPPOINT /*lptClick*/) { return FALSE; }
	virtual void OnDrawButton(CDC* /*pDC*/, CRect /*rectButton*/) {}
	virtual void OnDrawValue(CDC* /*pDC*/, CRect /*rect*/) {}
	virtual BOOL HasButton() const { return FALSE; }

	virtual BOOL PushChar(UINT nChar);
	virtual void OnDrawCheckBox(CDC * pDC, CRect rectCheck, BOOL bChecked);
	virtual void OnDrawName(CDC* pDC, CRect rect);
	virtual void OnClickName(CPoint point);
	virtual BOOL OnDblClk(CPoint point);

protected:
	CRect m_rectCheck;
};

class CBoundedNumberSubProp : public CMFCPropertyGridProperty
{
public:
	CBoundedNumberSubProp(const CString& strName, const COleVariant& varValue, int nMinValue, int nMaxValue, LPCTSTR lpszDescr = NULL);

	virtual BOOL OnUpdateValue();
private:
	int m_nMinValue;
	int m_nMaxValue;
};


class CDefectCodesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectCodesDlg)

public:
	CDefectCodesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDefectCodesDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEFECTCODESDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();
	CMFCPropertyGridCtrl m_ctrlDefetCodes;

	void DefectCodesPage();
	void RetrieveDefectCodesPage();

	CMFCPropertyGridProperty* AddPropertyGroup(CString strPropertyName);


	void AddPropertyItem(CString strPropertyName,
		COleVariant cValue,
		int nMin,
		int nMax,
		CString strTips,
		int nType,
		CMFCPropertyGridProperty* pGroup,
		BOOL bSubProperty = false,
		CMFCPropertyGridProperty* pSubGroup = NULL);

	COleVariant RetrivePropertyItem(int nPropertyItem,
		int nSubItem,
		BOOL bSubPorperty = FALSE,
		int nSubItemIndex = 0);

	COLORREF SetDefectColor(int nDefectID);
	COleVariant RetrivePropertyColor(int nPropertyItem, int nSubItem, BOOL bSubPorperty = FALSE, int nSubItemIndex = 0);

	int getDocIndexFromDefectName(std::string strDefectName, int nTrack, int nTotalDocs, int nErrorCodeId);
private:
	std::vector < ItemInfo > _defect;
	std::vector < GroupInfo > _group;
	std::vector < int > _defectGroupPosition;
	std::vector < CMFCPropertyGridColorProperty* > _property;
	std::map<CString, std::vector<int>> mapDefectName[MAX_TRACKS];
public:
	afx_msg void OnBnClickedOk();
};
