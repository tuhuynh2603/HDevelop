// SelectFolder.cpp : implementation file
//

#include "stdafx.h"
#include "SelectFolder.h"
#include "afxdialogex.h"


// CSelectFolder dialog

IMPLEMENT_DYNAMIC(CSelectFolder, CDialogEx)

TCHAR*	MakeObjectDynamic(LPTSTR szData);
CString WindowName(CString sName);
bool	ParsePath(CString sPath, int nLevel, CString *const psDirName);


CSelectFolder::CSelectFolder(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SELECTFOLDER, pParent)
	, m_strFileName(_T(""))
{
	m_bFindImage = FALSE;
	m_bSave = FALSE;
}

CSelectFolder::~CSelectFolder()
{
}

void CSelectFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
}


BEGIN_MESSAGE_MAP(CSelectFolder, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSelectFolder::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSelectFolder::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CSelectFolder::OnTvnSelchangedTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, &CSelectFolder::OnItemexpandingTree)
	ON_BN_CLICKED(ID_APPLY, &CSelectFolder::OnBnClickedApply)
END_MESSAGE_MAP()


bool ParsePath(CString sPath, int nLevel, CString *const psDirName)
{
	ASSERT(psDirName);
	//find first match
	int nStart = 0;									//Location in sPath to search from
	while (nLevel)
	{
		nStart = sPath.Find(_T('\\'), nStart);
		if (nStart < 0)
			return false;
		nLevel--;
		nStart++;
	}
	//We now have the start point to find the end.
	int nEnd = sPath.Find(_T('\\'), nStart);
	if (nEnd < 0)
		*psDirName = sPath.Mid(nStart);
	else
		*psDirName = sPath.Mid(nStart, nEnd - nStart);
	return true;
}

// CSelectFolder message handlers


BOOL CSelectFolder::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!m_ImageListTree.Create(IDB_FILE_TYPES, 17, 1, RGB(255, 255, 255)))
		TRACE(_T("Image list creation fault"));
	m_Tree.SetImageList(&m_ImageListTree, TVSIL_NORMAL);

	//
	//List the local drives
	//
	for (TCHAR cLetter = _T('A'); cLetter <= _T('Z'); cLetter++)
	{
		CString sDrive;
		sDrive.Format("%c", cLetter);

		sDrive += _T(":");
		UINT nType = GetDriveType(sDrive + _T("\\"));
		if (DRIVE_REMOVABLE <= nType && nType <= DRIVE_RAMDISK)
			InsertItem(TVI_ROOT, NULL, sDrive, nType);
	}

	//
	//Expand the Tree to the Inital Path.
	//
	int nLevel = 0;
	CString sDirName;
	HTREEITEM hCurrent = TVI_ROOT;					//Current item in the list being expanded.
	while (ParsePath(m_sPath, nLevel, &sDirName))
	{
		//Network or Drive expand
		//if (!nLevel && sDirName.IsEmpty())
		//	sDirName = NETWORK_NEIGHBOUR;
		//Search for the matching tree item and expand
		HTREEITEM hItem = m_Tree.GetChildItem(hCurrent);
		while (hItem)
		{
			if (sDirName.CompareNoCase(m_Tree.GetItemText(hItem)) == 0)
			{
				hCurrent = hItem;
				m_Tree.Expand(hCurrent, TVE_EXPAND);
				break;
			}
			hItem = m_Tree.GetNextSiblingItem(hItem);
		}
		nLevel++;
	}

	if (!m_bSave)
		m_strFileName = m_sPath;
	else
		m_strFileName.Empty();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CSelectFolder::InsertItem(HTREEITEM hParent, NETRESOURCE *const pNetResource, CString sText, int iImage, int iImageSelected /* = -1 */)
{
	//TRACE(_T("CDlgGetPath::InsertItem(%p,%p,%s +++ )\n"), hParent, pNetResource, sText);
	sText = WindowName(sText);
	TVINSERTSTRUCT InsertStruct;
	InsertStruct.hParent = hParent;
	InsertStruct.hInsertAfter = TVI_LAST;
	InsertStruct.itemex.mask = TVIF_IMAGE | TVIF_TEXT | TVIF_CHILDREN |
		TVIF_SELECTEDIMAGE | TVIF_PARAM;
	InsertStruct.itemex.pszText = sText.GetBuffer(sText.GetLength());  sText.ReleaseBuffer();
	InsertStruct.itemex.iImage = iImage;
	InsertStruct.itemex.cChildren = 1;
	InsertStruct.itemex.lParam = (LPARAM)pNetResource;
	if (iImageSelected == -1)
		InsertStruct.itemex.iSelectedImage = iImage;
	else
		InsertStruct.itemex.iSelectedImage = iImageSelected;
	return m_Tree.InsertItem(&InsertStruct);
}

CString WindowName(CString sName)
{
	CString sRet;
	//Scan all charactors to determine if their are any lower case items
	for (int n = 0; n < sName.GetLength(); n++)
	{
		TCHAR ch = sName[n];
		if ((ch >= 'a') && (ch <= 'z'))
			return sName;
	}
	sName.MakeLower();
	if (sName.GetLength() > 0)
	{
		CString sFirstChar;
		sFirstChar.Format("%c", sName[0]);
		sFirstChar.MakeUpper();
		sName = sFirstChar + sName.Mid(1);
	}
	return sName;
}

TCHAR* MakeObjectDynamic(LPTSTR szData)
{
	//TRACE(_T("MakeObjectDynamic( %s )\n"), szData);
	//Assume a NULL = empty string
	TCHAR * szRet = NULL;
	int nLength = 0;
	if (szData)
		nLength = _tcslen(szData) + 1;
	if (nLength > 0)
	{
		szRet = new TCHAR[nLength];
		ASSERT(szRet);
		_tcscpy(szRet, szData);
	}
	return szRet;
}

bool CSelectFolder::PopulateTree(CString sPath, HTREEITEM hParent)
{
	TRACE(_T("CDlgGetPath::PopulateTree( %s )\n"), sPath);
	bool bGotChildren = false;						//True if a child is added.	
													//
													//Populate Network neighbourhood tree (Entire network and Local-Computers)
													//
	//if (hParent == m_hNetworkRoot)
	//{
	//	bGotChildren = EnumNetwork(hParent);
	//}
	//
	//Network item(Search deep into the network)
	//
	//else if (m_Tree.GetItemData(hParent))
	//{
	//	bGotChildren = EnumNetwork(hParent);
	//}

	//
	//Search for files and populate the CTreeCtrl
	//

	CFileFind finder;
	BOOL bWorking = finder.FindFile(sPath + _T("*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strFileName = finder.GetFileName();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
		{
			CString strFileName = finder.GetFileName();
			InsertItem(hParent, NULL, finder.GetFileName(), DRIVE_NO_ROOT_DIR, DRIVE_UNKNOWN);
			bGotChildren = true;
		}
		else if (m_bFindImage)
		{
			if (!strFileName.Right(3).CompareNoCase("bmp"))
				InsertItem(hParent, NULL, finder.GetFileName(), 15, 15);
		}
	}

	//
	//Remove the [+] if no children
	//
	if (!bGotChildren)
	{
		TVITEM item = { 0 };
		item.mask = TVIF_HANDLE | TVIF_CHILDREN;
		item.hItem = hParent;
		item.cChildren = 0;
		m_Tree.SetItem(&item);
	}
	return bGotChildren;
}


CString CSelectFolder::GetItemPath(HTREEITEM hItem)
{
	TRACE(_T("CDlgGetPath::GetItemPath(%p)\n"), hItem);
	CString sRet;
	do
	{
		//End with a share name.
		NETRESOURCE *const pNetResource = (NETRESOURCE *)(m_Tree.GetItemData(hItem));
		if (pNetResource)
		{
			sRet = CString(pNetResource->lpRemoteName) + _T('\\') + sRet;
			break;
		}
		//Add the directory name to the path.
		sRet = m_Tree.GetItemText(hItem) + _T('\\') + sRet;
		hItem = m_Tree.GetParentItem(hItem);
	} while (hItem);
	return sRet;
}


void CSelectFolder::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_sPath;
	if (m_bFindImage)
	{
		CString strTempPath = m_strFileName;
		strTempPath = strTempPath.Left(strTempPath.GetLength() - 1);
		CString str = strTempPath.Right(3);
		if (str.Compare("bmp")!=0) {
			AfxMessageBox("Please Select Image File");
			return;
		}
			
	}
	//if (m_bSave)
	//	m_sPath.Format("%s%s", m_sPath, m_strFileName);
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CSelectFolder::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CSelectFolder::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	ASSERT(pNMTreeView);
	ASSERT(pResult);
	CString sPath;									//Currently selected path or empty if not valid
													//Disable search on Workstation roots
	if (m_Tree.GetItemData(pNMTreeView->itemNew.hItem)/* ||
		pNMTreeView->itemNew.hItem == m_hNetworkRoot*/)
	{
		GetDlgItem(IDOK)->EnableWindow(false);
	}
	else
	{
		
		GetDlgItem(IDOK)->EnableWindow(true);
		sPath = GetItemPath(pNMTreeView->itemNew.hItem);
	}

	if (!m_bSave) {
		m_strFileName = sPath;
		if (m_bFindImage) {
			CString strTempPath = m_strFileName;
			strTempPath = strTempPath.Left(strTempPath.GetLength() - 1);
			CString str = strTempPath.Right(3);
			if (str.Compare("bmp") != 0) {
				GetDlgItem(IDOK)->EnableWindow(false);
			}
			else {
				GetDlgItem(IDOK)->EnableWindow(true);
			}
		}
	}
	

	UpdateData(FALSE);
	*pResult = 0;
}


void CSelectFolder::OnItemexpandingTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CWaitCursor CursorWaiting;						//Show the wait cursor while expanding
	//NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	ASSERT(pNMTreeView);
	ASSERT(pResult);
	//Only action 2 notifications
	if (pNMTreeView->action == 2)
	{
		//Update location display
		CString sPath = GetItemPath(pNMTreeView->itemNew.hItem);
		//Refresh children
		if (!m_Tree.GetChildItem(pNMTreeView->itemNew.hItem))
		{
			PopulateTree(sPath, pNMTreeView->itemNew.hItem);
			if (m_Tree.GetSelectedItem() != pNMTreeView->itemNew.hItem)
				m_Tree.SelectItem(pNMTreeView->itemNew.hItem);
		}
	}
	*pResult = 0;
}

BOOL CSelectFolder::DestroyWindow()
{
	//TRACE(_T("CDlgGetPath::DestroyWindow()\n"));
	//Get the current selection before heading home
	m_sPath = GetItemPath(m_Tree.GetSelectedItem());

	//Start at the root deleting.
	HTREEITEM hItemCur = m_Tree.GetRootItem();		//Current item under inspection

													//
													//While their is still data in the tree
													//
	while (hItemCur)
	{
		HTREEITEM hItem = m_Tree.GetChildItem(hItemCur);
		//Has children then make the child current
		if (hItem)
		{
			hItemCur = hItem;
			continue;
		}

		//Item has not children so we shall destroy it.
		//but first we must decide who is to take its place.
		HTREEITEM hNextSibItem = m_Tree.GetNextSiblingItem(hItemCur);
		HTREEITEM hPrevSibItem = m_Tree.GetPrevSiblingItem(hItemCur);
		HTREEITEM hParentItem = m_Tree.GetParentItem(hItemCur);
		//Get item data to check if lparam is to be destroyed
		NETRESOURCE *const pNetResource = (NETRESOURCE *)m_Tree.GetItemData(hItemCur);//(item.lParam);
		if (pNetResource)
		{
			delete[](pNetResource->lpLocalName);
			delete[](pNetResource->lpRemoteName);
			delete[](pNetResource->lpComment);
			delete[](pNetResource->lpProvider);
			delete pNetResource;
		}
		m_Tree.DeleteItem(hItemCur);
		hItemCur = NULL;
		//Determine which item is next to recieve the focus
		if (hParentItem)
			hItemCur = hParentItem;
		if (hPrevSibItem)
			hItemCur = hPrevSibItem;
		if (hNextSibItem)
			hItemCur = hNextSibItem;
	}
	//All items removed from list now. Lets trash this place and go home.
	return CDialog::DestroyWindow();
}

void CSelectFolder::OnBnClickedApply()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_sPath = m_strFileName;
	//if (!m_ImageListTree.Create(IDB_FILE_TYPES, 17, 1, RGB(255, 255, 255)))
	//	TRACE(_T("Image list creation fault"));
	m_Tree.SetImageList(&m_ImageListTree, TVSIL_NORMAL);
	//for (TCHAR cLetter = _T('A'); cLetter <= _T('Z'); cLetter++)
	//{
	//	CString sDrive;
	//	sDrive.Format("%c", cLetter);

	//	sDrive += _T(":");
	//	UINT nType = GetDriveType(sDrive + _T("\\"));
	//	if (DRIVE_REMOVABLE <= nType && nType <= DRIVE_RAMDISK)
	//		InsertItem(TVI_ROOT, NULL, sDrive, nType);
	//}

	// 
	//Expand the Tree to the Inital Path. 
	// 
	int nLevel = 0;
	CString sDirName;
	HTREEITEM hCurrent = TVI_ROOT;					//Current item in the list being expanded. 
	while (ParsePath(m_sPath, nLevel, &sDirName))
	{
		//Network or Drive expand 
		//if (!nLevel && sDirName.IsEmpty()) 
		//	sDirName = NETWORK_NEIGHBOUR; 
		//Search for the matching tree item and expand 
		HTREEITEM hItem = m_Tree.GetChildItem(hCurrent);
		while (hItem)
		{
			if (sDirName.CompareNoCase(m_Tree.GetItemText(hItem)) == 0)
			{
				hCurrent = hItem;
				m_Tree.Expand(hCurrent, TVE_EXPAND);
				break;
			}
			hItem = m_Tree.GetNextSiblingItem(hItem);
		}
		nLevel++;
	}

	if (!m_bSave)
		m_strFileName = m_sPath;
	else
		m_strFileName.Empty();

	UpdateData(FALSE);
}
