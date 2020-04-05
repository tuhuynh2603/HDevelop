///Udupa; Jan'2013; Custom Edit Control with auto popup/close of on-screen-keyboard

#pragma once

#include "afxwin.h"

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif


class DECLSPEC_WINDOW_IF CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)

public:
	CEditEx();
	~CEditEx();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	HANDLE m_hProcess;
};

