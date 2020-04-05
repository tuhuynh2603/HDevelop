#pragma once
#include "resource.h"
#include "afxcmn.h"

// CDefectErrorCodeSelection dialog

class CDefectErrorCodeSelection : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectErrorCodeSelection)

public:
	CDefectErrorCodeSelection(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDefectErrorCodeSelection();
	CFont m_fontList;
	int m_nSelectedErrorCode;
	BOOL m_bIsConfirm;
	BOOL m_bIsApplyAllDevice;
	void  AddItem(CListCtrl* pCtrl, CString strItem, int nIndex);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEFECT_ERRORCODE_SELECTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listErrorCodeSelection;
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawList1(NMHDR*, LRESULT*);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonApplyAll();
};
