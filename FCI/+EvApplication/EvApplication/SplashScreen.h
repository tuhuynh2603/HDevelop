#pragma once

/* 
 * Copyright (c) 2004 Apple Enterprises. This code is
 * proprietary and a trade secret of Apple Enterprises.
 *
 * $Workfile: SplashWindow.h $
 *
 * $Creator: Chris Apple $
 *
 * $Description: Initial splash screen. $
 *
 * $Log: /AppleEnterprises/CodeExamples/SplashMFC/SplashMfcSingleDoc/SplashWindow.h $
 * 
 * 1     4/22/09 4:11p Capple
 * Initial Checkin.
 * 
 * 1     11/05/04 1:27p Capple
 * Initial Checkin.
 * 
 * $NoKeywords: $
 */

// Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CSplashScreen : public CWnd
{
// Construction
protected:
	CSplashScreen();

// Attributes:
public:
	CBitmap m_bitmap;

// Operations
public:
	static void ShowSplashScreen(CWnd* pParentWnd = NULL, LPCTSTR statusMessage = NULL, int millisecondsToDisplay=0 );
	static void HideSplashScreen();
	static BOOL PreTranslateAppMessage(MSG* pMsg);
	static CString GetVersionNumber() { return m_versionNumberString; }
	static void GetVersionStrings();
	static CSize FindFontPointSize( CPaintDC &dc, LPCTSTR fontName, const CStringArray & stringsToCheck, CSize maximumWidth );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashScreen)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSplashScreen();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	static CSplashScreen* m_pSplashWindow;

// Generated message map functions
protected:
	//{{AFX_MSG(CSplashScreen)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static CString m_productNameString;
	static CString m_companyNameString;
	static CString m_versionNumberString;
	static CString m_versionString;
	static CString m_copyrightString;
	static CString m_commentsString;
	static CString m_statusMessage;

	static int m_millisecondsToDisplay;				// 0 ==> until mouse click or keystroke

	// create rectangle that product name has to fit in
	static const int m_productNameVerticalOffset;	// empty space between top border and Product Name
	static const int m_productNameVerticalHeight;	// maximum height of Product Name
	static const int m_productNameLeftMargin;		// distance from left side to place name, company, copyright and version
	static const int m_productNameRightMargin;		// distance from right side to place name, company, copyright and version
	static const CString m_productNameFontName;		// name of font for application name
	static CSize m_productNamePointSize;			// point size used for the application name	
	static COLORREF m_productNameTextColor;			// color used for text

	static const BOOL m_displayCompanyName;			// true if displaying companyName
	static const BOOL m_displayVersion;				// true if displaying version
	static const BOOL m_displayCopyright;			// true if displaying copyright
	static const BOOL m_displayComments;			// true if displaying comments

	// create rectangle that strings in body have to fit in
	static const int m_bodyVerticalOffset;			// empty space between top border and top of body
	static const int m_bodyVerticalHeight;			// maximum height of body
	static const int m_bodyLeftMargin;				// distance from left side to place company name, copyright, version and comment
	static const int m_bodyRightMargin;				// distance from right side to place company name, copyright, version and comment
	static const CString m_bodyFontName;			// name of font for company name, copyright, version and comment	
	static CSize m_bodyPointSize;					// point size used for company name, copyright, version and comment	
	static COLORREF m_bodyTextColor;				// color used for company name, copyright, version and comment

	// create rectangle for status line string
	static const int m_statusVerticalOffset;		// empty space between top border and top of status string
	static const int m_statusVerticalHeight;		// maximum height of status string
	static const int m_statusLeftMargin;			// distance from left side to place status string
	static const int m_statusRightMargin;			// distance from right side to place status string
	static const CString m_statusMessageFontName;	// name of font for status message while starting	
	static CSize m_statusMessagePointSize;			// point size used for status message while starting	
	static COLORREF m_statusMessageTextColor;		// color used for status message while starting
};
