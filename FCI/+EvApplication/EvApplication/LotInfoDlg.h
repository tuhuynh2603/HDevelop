#pragma once
#include "Application.h"
#include "resource.h"
// CLotInfoDlg dialog
class CApplication;

class CLotInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CLotInfoDlg)

public:
	CLotInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLotInfoDlg();

	CApplication* m_pApp;
	
	CString m_strConfigFile;		// Current Loaded FileName
	CString m_strDeviceSpecDir;		// Device Spec Storage Directory
	CString m_strWildCard;			// Wild Card	
// Dialog Data
	enum { IDD = IDD_LOT_INFO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void FillComboBoxList();
	virtual void OnOK();
	CString m_strInspCfgDir;		// Current Working Directory
	afx_msg void OnCbnSelchangeLotInfoRecipe();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strLotID;
	CString m_strOperator;
	CComboBox m_ctrlRecipe;
	CString m_strComments;
	
};
