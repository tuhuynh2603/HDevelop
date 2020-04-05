#pragma once
#include "resource.h"

// CPixelRuler dialog

class CPixelRuler : public CDialogEx
{
	DECLARE_DYNAMIC(CPixelRuler)

public:
	CPixelRuler(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPixelRuler();

// Dialog Data
	enum { IDD = IDD_DIALOG_PIXEL_RULER };
	void UpdateDatas(CPoint point1, CPoint point2, double dResolution = 1, double dResolutionX = 1, double dResolutionY = 1, BOOL bImperialUnit = FALSE);


// Dialog Data
protected:
	CBrush m_brush;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
private:
	CString m_strDirectionX;
	CString m_strDirectionY;
	CString m_strDistanceInPixel;
	CString m_strDistanceInMicron;
	CString m_strAngleInDegrees;
	CString m_strAngleInRadians;
public:
	afx_msg void OnClose();
};
