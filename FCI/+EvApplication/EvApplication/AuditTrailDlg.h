#pragma once
#include "atltime.h"

// CAuditTrailDlg dialog


class CAuditTrailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAuditTrailDlg)

public:
	CAuditTrailDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAuditTrailDlg();
	CBrush m_brush;

// Dialog Data
	enum { IDD = IDD_AUDITTRAIL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBexpttopdf();
	afx_msg void OnBnClickedBrowse();
	afx_msg LRESULT OnUpdateAuditTrail(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void DisplayLogInList();
	void WriteToPdf(CString strFile, CString strTitle);
	void WriteToCsv(CString strFile);
	void AddItem(CListCtrl* pCtrl, CString strItem, int nIndex);
	void WriteToList();

	CFont m_fontList;
	COLORREF m_colorInfo[2];
	CFont m_font;
	CTime m_StartDate;
	CTime m_EndDate;
	BOOL m_bPdf;
	BOOL m_bCsv;	
	CListCtrl m_ctrlAuditTrailList;
	CString m_strFolder;
};