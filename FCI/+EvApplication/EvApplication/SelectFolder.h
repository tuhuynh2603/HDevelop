#include "resource.h"

#pragma once


// CSelectFolder dialog

class CSelectFolder : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectFolder)

public:
	CSelectFolder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectFolder();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECTFOLDER };
#endif
	BOOL			m_bFindImage;
	BOOL			m_bSave;
	CTreeCtrl		m_Tree;
	CImageList		m_ImageListTree;					//Item in the Tree image list				
	CString			m_sPath;

	CString			GetPath()				{ return m_sPath; }
	void			SetPath(CString sPath)	{ m_sPath = sPath; }
	virtual	BOOL	DestroyWindow();
	CString			GetItemPath(HTREEITEM hItem);
	bool			PopulateTree(CString sPath, HTREEITEM hParent);
	HTREEITEM		InsertItem(HTREEITEM hParent, NETRESOURCE *const pNetResource, CString sText, int iImage, int iImageSelected = -1);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemexpandingTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	CString m_strFileName;
	afx_msg void OnBnClickedApply();
};
