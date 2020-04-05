#pragma once
#include "resource.h"

// CInspectionSettings dialog

class CInspectionSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CInspectionSettings)

public:
	CInspectionSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInspectionSettings();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	BOOL bCheckedForAllTracks;
//	CString m_strInspectionDelay;
	BOOL m_bCheckForAllTracks;
	BOOL m_bCheckForSingleFov;
	int m_nInspectionDelay;
};
