/// ChildFrame.h : interface of the CChildFrame class
/// Udupa; April'2012

#pragma once

#include "EvChildFrame.h"

class CChildFrame : public CEvChildFrame
{
	DECLARE_DYNCREATE(CChildFrame)

protected:
	virtual int CreateFrame();
	DECLARE_MESSAGE_MAP()
};
