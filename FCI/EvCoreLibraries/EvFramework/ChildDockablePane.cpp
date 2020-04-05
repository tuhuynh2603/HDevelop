// ChildDockablePane.cpp : implementation file
//

#include "stdafx.h"
#include "ChildDockablePane.h"
#include "ChildFormView.h"

// CChildDockablePane

IMPLEMENT_DYNAMIC(CChildDockablePane, CDockablePane)

CChildDockablePane::CChildDockablePane()
{

}

CChildDockablePane::~CChildDockablePane()
{
}


BEGIN_MESSAGE_MAP(CChildDockablePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()




// CChildDockablePane message handlers


int CChildDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;
 
    // TODO:  Add your specialized creation code here
 
 //   m_pFormview = CChildFormView::onNe
 
    return 0;
}
 
void CChildDockablePane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
 
    // TODO: Add your message handler code here
 
   /* if( m_pFormview )
        m_pMyFormView-&gt;*/
	SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}