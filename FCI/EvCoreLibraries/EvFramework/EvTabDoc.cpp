/// EvTabDoc.cpp : implementation of the CEvTabDoc class
/// Udupa; April'2012

#include "stdafx.h"
#include "EvTabDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvTabDoc

IMPLEMENT_DYNCREATE(CEvTabDoc, CDocument)

CEvTabDoc::CEvTabDoc()
{
}

BOOL CEvTabDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CEvTabDoc::~CEvTabDoc()
{
}


BEGIN_MESSAGE_MAP(CEvTabDoc, CDocument)
	//{{AFX_MSG_MAP(CEvTabDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvTabDoc diagnostics

#ifdef _DEBUG
void CEvTabDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEvTabDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEvTabDoc serialization

void CEvTabDoc::Serialize(CArchive& ar)
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

void CEvTabDoc::SetNewTitle( LPCTSTR lpszTitle)
{
	this->SetTitle(lpszTitle);
	
}
/////////////////////////////////////////////////////////////////////////////
// CEvTabDoc commands
