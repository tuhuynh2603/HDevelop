/// EvSdiChildWnd.cpp : implementation of the CEvSdiChildWnd class
/// Udupa; April'2012

#include "stdafx.h"
#include "EvSdiChildWnd.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CEvSdiChildWnd

CEvSdiChildWnd::CEvSdiChildWnd()
{
	m_pDocTemplate = NULL;
	m_pParent = NULL;
}

CEvSdiChildWnd::~CEvSdiChildWnd()
{
	if(m_pDocTemplate) {
		delete m_pDocTemplate;
		m_pDocTemplate=NULL;
	}
}


BEGIN_MESSAGE_MAP(CEvSdiChildWnd, CWnd)
	//{{AFX_MSG_MAP(CEvSdiChildWnd)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEvSdiChildWnd message handlers

BOOL CEvSdiChildWnd::Create(CWnd *pParent, DWORD ctrlId,
		UINT nIDResource,
		CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass,
		CRuntimeClass* pViewClass,
		DWORD Style, DWORD StyleEx)
{
	m_pParent = pParent;

    SubclassWindow(m_pParent->GetDlgItem(ctrlId)->GetSafeHwnd());

	m_pDocTemplate = new CEvSdiTemplate(
		nIDResource,
		pDocClass,
		pFrameClass,       // main SDI frame window
		pViewClass);

	m_pDocTemplate->m_pParent = pParent;
	
	CRect clientRect;
	GetWindowRect(clientRect);
    m_pParent->ScreenToClient(&clientRect);
	m_pDocTemplate->m_Rect = clientRect;
	
	m_pDocTemplate->m_Style = Style;
	m_pDocTemplate->m_StyleEx = StyleEx;

	m_pDocTemplate->OpenDocumentFile(NULL);
	return TRUE;
}

BOOL CEvSdiChildWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;

	//return CWnd::OnEraseBkgnd(pDC);
}
