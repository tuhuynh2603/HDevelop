///Udupa; Jan'2013; Custom Edit Control with auto popup/close of on-screen-keyboard

#include "StdAfx.h"
#include "EditEx.h"


IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{
	m_hProcess = NULL;
}


CEditEx::~CEditEx()
{
	CWnd* pWnd = FindWindow(NULL, "On-Screen Keyboard");
	if(pWnd && ::IsWindow(pWnd->m_hWnd))
		::SendMessage(pWnd->m_hWnd, WM_CLOSE, 0, 0);
	//	pWnd->DestroyWindow();

	//if(m_hProcess) {
	//	UINT nExitCode = 1;
	//	if(TerminateProcess(m_hProcess, nExitCode)) {
	//		WaitForSingleObject(m_hProcess, 2000);
	//		AfxMessageBox("Terminated");
	//	}
	//	else {
	//		CString str;
	//		str.Format("%d", GetLastError());
	//		AfxMessageBox(str);
	//	}
	//}
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


void CEditEx::OnSetFocus(CWnd* pOldWnd)
{
	CWnd* pWnd = FindWindow(NULL, "On-Screen Keyboard");
	if(!pWnd || !::IsWindow(pWnd->m_hWnd)) {
		//if(!LaunchProcess("osk.exe")) {
			OutputDebugLogTo(0, TRUE, "Failed to launch On-Screen Keyboard");
			return;
		//}
	}

	CEdit::OnSetFocus(pOldWnd);
		//ShellExecute(NULL, "open", "osk.exe", NULL, NULL, SW_SHOWNORMAL);
}
