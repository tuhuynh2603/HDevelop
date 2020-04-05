#pragma once

// StatisticsDoc.h : header file
//

class CStatisticsView;

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDoc document

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CStatisticsDoc : public CDocument
{
protected:
	CStatisticsDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStatisticsDoc)

	CStatisticsView* m_pView;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticsDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatisticsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatisticsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
