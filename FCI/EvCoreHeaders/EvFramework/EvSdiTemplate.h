/// EvSdiTemplate.h : interface of the CEvSdiTemplate class
/// Udupa; April'2012

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CEvSdiTemplate command target

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvSdiTemplate : public CSingleDocTemplate
{
	DECLARE_DYNAMIC(CEvSdiTemplate)

	CEvSdiTemplate(UINT nIDResource,
		CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass,
		CRuntimeClass* pViewClass);
	virtual ~CEvSdiTemplate();


// Attributes
public:
	CWnd * m_pParent;
	CRect m_Rect;
	DWORD m_Style;
	DWORD m_StyleEx;

	CFrameWnd * m_pFrame;

// Operations
public:
	virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog);
	void _AfxAppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt);
	virtual CDocument* OpenDocumentFile(
		LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvSdiTemplate)
	//}}AFX_VIRTUAL

// Implementation
protected:
//	virtual ~CEvSdiTemplate();

	// Generated message map functions
	//{{AFX_MSG(CEvSdiTemplate)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
