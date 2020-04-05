#pragma once
#include "afxwin.h"
#include "Application.h"

//// Rakshith;
// CCustomListBox derived class for CListBox - drag & move listbox contents

#define LB_SCROLLDOWN  100
#define LB_SCROLLUP    101

class CCustomListBox : public CListBox 
{
	DECLARE_DYNAMIC(CCustomListBox)

public:
	CCustomListBox();
	BOOL bCallNavigate;
	virtual ~CCustomListBox();

protected:
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
   DECLARE_MESSAGE_MAP()

private:
   int m_nMovingIndex;
   int m_nMoveToIndex;
   BOOL m_bCaptured;
   DWORD m_dwInterval;

   void InsertDraggedItem();
   void DoTheScrolling(CPoint Point,CRect ClientRect);
   void DrawTheLines(int Index);
};

//// **** ------------------------------------------------------------------------------ **** ////

// CDefectPrioritySelDlg dialog

class CDefectPrioritySelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectPrioritySelDlg)

public:
	CDefectPrioritySelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDefectPrioritySelDlg();

// Dialog Data
	enum { IDD = IDD_DEFECT_PRIORITY_SEL_DLG };

	CApplication* pApp;

	//CListBox m_ctrlListBox;
	CCustomListBox m_ctrlListBox;
//	CStringArray m_strListBoxContents;

	void LoadDefaultDefectSelection();
	void NavigateListBoxContents(int nRow, BOOL bDown, BOOL bKeyDown=FALSE);
	void DefectPrioritySelectionFile(BOOL bSave);
	void InitializePriorityList();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);

	afx_msg void OnBnClickedDefectPrioritySelUpMfcbutton();
	afx_msg void OnBnClickedDefectPrioritySelDownMfcbutton();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
