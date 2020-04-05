/// MultipleRecipeDialog.h : header file
/// Udupa; Dec'2012

#pragma once


class CMultipleRecipeDialog : public CDialogEx
{
// Construction
public:
	CApplication* m_pApp;

	CMultipleRecipeDialog(CWnd* pParent = NULL);   // standard constructor
	~CMultipleRecipeDialog();

// Dialog Data
	//{{AFX_DATA(CMultipleRecipeDialog)
	enum { IDD = IDD_PARM_LOAD_MULTIRECIPE_DIALOG };
	CComboBox m_ComboConfigFileSelect[3];
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultipleRecipeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CBrush m_brush;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMultipleRecipeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonLoad();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeConfigFileSelect();
	afx_msg void OnBnClickedButtonCopyrecipe();
	afx_msg void OnBnClickedButtonDeleterecipe();
	afx_msg void OnSelchangeConfigFileSelect3();
	afx_msg void OnBnClickedCopyProdMode();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
