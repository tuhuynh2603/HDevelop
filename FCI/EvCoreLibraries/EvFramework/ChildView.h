/// ChildView.h : interface of the CChildView class
/// Udupa; April'2012

#pragma once

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

#include "afxwin.h"

class DECLSPEC_WINDOW_IF CChildView : public CScrollView
{
public:
	CChildView(void);
	virtual ~CChildView(void);

	virtual void FitToWindow() {};
};

