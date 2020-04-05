/// EvTabDoc.h : interface of the CEvTabDoc class
/// Udupa; April'2012

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CEvTabDoc document

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvTabDoc : public CDocument
{
protected:
	CEvTabDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEvTabDoc)

// Attributes
public:

// Operations
public:
	void SetNewTitle(LPCTSTR lpszTitle );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvTabDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEvTabDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEvTabDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
