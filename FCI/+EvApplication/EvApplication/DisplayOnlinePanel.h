#pragma once


// CDisplayOnlinePanel dialog

class CDisplayOnlinePanel : public CDialogEx
{
	DECLARE_DYNAMIC(CDisplayOnlinePanel)

public:
	CDisplayOnlinePanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDisplayOnlinePanel();

	CString m_strMachineId;
	CString m_strLotId;
	CString m_strRecipe;
	CBrush* m_pBrush;
	CBrush m_pBrushTemp[4];
	//void InitializeDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDC_ONLINE_DISPLAY_PANEL};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
