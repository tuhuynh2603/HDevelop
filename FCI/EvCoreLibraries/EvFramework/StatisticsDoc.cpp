// StatisticsDoc.cpp : implementation file
//

#include "stdafx.h"
#include "StatisticsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDoc

IMPLEMENT_DYNCREATE(CStatisticsDoc, CDocument)

CStatisticsDoc::CStatisticsDoc()
{
}

CStatisticsDoc::~CStatisticsDoc()
{
}


BEGIN_MESSAGE_MAP(CStatisticsDoc, CDocument)
	//{{AFX_MSG_MAP(CStatisticsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDoc diagnostics

#ifdef _DEBUG
void CStatisticsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStatisticsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
//AddView
//RemoveView
//OnChangedViewList don't close doc after all views are closed
/*
	GetActiveView()
	GetActiveDocument()
	Search for current view; if not found,..  create:

	CCreateContext context;
	context.m_pCurrentCoc = pDoc;
	pNewView=(cView*)pNewViewClass->CreateObject();
	pNewView->Create(NULL, NULL, 0, CFrameWnd::rectDefault, this, nID, &context);
	pNewView->OnInitialUpdate();
	SetActiveView (pNewView);
	pNewView->ShowWindow(SW_SHOW);
	pOldView->ShowWindow(SW_HIDE):
	pOldView->SetDlgCtrlID(pNewView->GetDlgCtrlID());
	pNewView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

	RecalcLayout();


*/

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDoc serialization

void CStatisticsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL CStatisticsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle("Statistics");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDoc commands
