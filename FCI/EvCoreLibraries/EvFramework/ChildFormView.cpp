// ChildFormView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ChildFormView.h"


// CChildFormView

IMPLEMENT_DYNCREATE(CChildFormView, CFormView)

CChildFormView::CChildFormView()
	: CFormView(CChildFormView::IDD)
{

}

CChildFormView::~CChildFormView()
{
}

void CChildFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChildFormView, CFormView)
END_MESSAGE_MAP()


// CChildFormView diagnostics

#ifdef _DEBUG
void CChildFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CChildFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CChildFormView message handlers
void CChildFormView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
 
    // TODO: Add your specialized code here and/or call the base class
    GetParentFrame()->RecalcLayout();
    SetScrollSizes( MM_TEXT, CSize( 205, 157 ) );
 
    ResizeParentToFit(FALSE);
}
 
CChildFormView *CChildFormView::CreateOne( CWnd *pParent )
{
    CChildFormView *pFormView = new CChildFormView;
    //CMyFormView *pFormView = NULL;
    //CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CMyFormView);
    //pFormView = (CMyFormView *)pRuntimeClass->CreateObject();
	CEvImageDoc *pDoc;
    ///CEvImageDoc *pDoc = CEvImageDoc::CreateOne();
    pFormView->m_pDocument = pDoc;
 
    CCreateContext *pContext = NULL;
 
#if 0
    if( !pFormView->CreateEx(0, NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,205,157),
        pParent, -1, pContext ) )
#else
    if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 205, 157), pParent, 0, pContext))
#endif
    //if( !pFormView->CreateEx( 0, AfxRegisterWndClass(0, 0, 0, 0), NULL,
    //  WS_CHILD | WS_VISIBLE, CRect( 0, 0, 205, 157), pParent, -1, pContext) )
    {
        AfxMessageBox( _T("Failed in creating CMyFormView") );
    }
 
    pFormView->OnInitialUpdate();
 
    return pFormView;
}
 
int CChildFormView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    // TODO: Add your message handler code here and/or call default
 
    int nResult = 0;
 
    CFrameWnd* pParentFrame = GetParentFrame();
 
    if( pParentFrame == pDesktopWnd )
    {
        // When this is docked
        nResult = CFormView::OnMouseActivate(pDesktopWnd, nHitTest, message);
    }
    else
    {
        // When this is not docked
 
        BOOL isMiniFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CMiniFrameWnd ) );
        BOOL isPaneFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CPaneFrameWnd ) );
        BOOL isMultiPaneFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CMultiPaneFrameWnd ) );
 
        // pDesktopWnd is the frame window for CDockablePane
 
        nResult = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
 
        //nResult = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
        //if( nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT )
        //  return nResult;
 
        //if (pDesktopWnd != NULL)
        //{
        //  // either re-activate the current view, or set this view to be active
        //  //CView* pView = pDesktopWnd->GetActiveView();
        //  //HWND hWndFocus = ::GetFocus();
        //  //if (pView == this &&
        //  //  m_hWnd != hWndFocus && !::IsChild(m_hWnd, hWndFocus))
        //  //{
        //  //  // re-activate this view
        //  //  OnActivateView(TRUE, this, this);
        //  //}
        //  //else
        //  //{
        //  //  // activate this view
        //  //  pDesktopWnd->SetActiveView(this);
        //  //}
        //}
    }
 
    return nResult;
}