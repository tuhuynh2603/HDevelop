
#include "stdafx.h"

#include "EvTeachPropertiesWnd.h"
#include "EvImageDoc.h"
#include "Resource.h"
#include "PropertyGridProperty.h"
#include "PropertyGridColorProperty.h"
#include "Parameters.h"
#include "MsgCode.h"
#include "EvMainFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CEvTeachPropertiesWnd::CEvTeachPropertiesWnd()
{
	m_nPos = 0;
	m_nSets = 0;
	m_nCurSet = -1;
	m_nCurPos = -1;
	m_bIsModified = FALSE;
}

CEvTeachPropertiesWnd::~CEvTeachPropertiesWnd()
{
	strTopName.RemoveAll();
	strBottomName.RemoveAll();
	strSideName.RemoveAll();
	//delete[] m_wndPropLists;
	for(int n=0; n<m_nPos; n++)
		delete[] m_wndPropLists[n];

	m_fntPropList.DeleteObject();
}

BEGIN_MESSAGE_MAP(CEvTeachPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)//
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)//
	ON_COMMAND(ID_REVERTPROPERTIES, OnRevertProperties)//
	ON_UPDATE_COMMAND_UI(ID_REVERTPROPERTIES, OnUpdateRevertProperties)//
	ON_COMMAND(ID_SAVEPROPERTIES, OnSaveProperties)//
	ON_UPDATE_COMMAND_UI(ID_SAVEPROPERTIES, OnUpdateSaveProperties)//
	ON_WM_SETFOCUS()//
	ON_WM_SETTINGCHANGE()//
	ON_WM_CLOSE()//
	ON_CBN_SELCHANGE(IDC_COMBO_PROPERTIES, OnSelchangeCamerasCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_PROPERTIES_POS, OnSelchangeCamerasComboPos)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CEvTeachPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	//int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	m_wndObjectComboPos.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 300, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top  + cyCmb, rectClient.Width(), 300, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + (cyCmb*2), rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	CRect rect;
	GetWindowRect(&rect);
	/*for(int i=0; i<m_nSets; i++) {
		m_wndPropLists[i].SetWindowPos(NULL, rectClient.left, rectClient.top + (cyCmb*2) + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndPropLists[i].SetLeftColumnWidth(rect.Width()*3/4);
	}*/
	for(int i=0; i<m_nPos; i++) {
		for(int j=0; j<m_nSets; j++) {
			m_wndPropLists[i][j].SetWindowPos(NULL, rectClient.left, rectClient.top + (cyCmb*2) + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
			m_wndPropLists[i][j].SetLeftColumnWidth(rect.Width()*3/4);
		}
	}
}

int CEvTeachPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectComboPos.Create(dwViewStyle, rectDummy, this, IDC_COMBO_PROPERTIES_POS))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}
	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, IDC_COMBO_PROPERTIES))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	//m_wndPropLists = new CPropertyList[m_nSets];
	for(int n=0; n<m_nPos; n++)
		m_wndPropLists[n] = new CPropertyList[m_nSets];

	/*for(int i=0; i<m_nPos; i++) {
		str.Format("Position_%d",i+1);
		m_wndObjectComboPos.AddString(_T(str));
	}*/

	for(int i=0; i<m_nPos; i++) {
		CString str;
		int nBottomNo=0;
		int nSideNo=0;
		for(int j=0; j<m_nSets; j++) {
			int nID = (m_nSets*i)+j;
			//if(!m_wndPropLists[i].Create(WS_CHILD, rectDummy, this, IDC_GRID1_PROPERTIES+i)) {
			if(!m_wndPropLists[i][j].Create(WS_CHILD, rectDummy, this, IDC_GRID_PROPERTIES+nID)) {
				TRACE0("Failed to create Properties Grid \n");
				return -1;
			}
			if(!i) {
				if(j<m_nTopSets)
					str.Format("%s", strTopName.GetAt(j));
				else if(j>=m_nTopSets && j< (m_nTopSets+m_nBottomSets)){
					str.Format("%s", strBottomName.GetAt(nBottomNo));
					nBottomNo++;
				}
				else{
					str.Format("%s", strSideName.GetAt(nSideNo));
					nSideNo++;
				}
				m_wndObjectCombo.AddString(_T(str));
			}
		}
		str.Format("Position_%d",i+1);
		m_wndObjectComboPos.AddString(_T(str));
	}
	m_wndObjectCombo.SetCurSel(0);
	//OnSelchangeCamerasCombo();
	m_wndObjectComboPos.SetCurSel(0);
	//OnSelchangeCamerasComboPos();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);
	m_wndToolBar.LoadBitmap(IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	SetPropListFont();
	for(int i=0; i<m_nPos; i++) {
		for(int j=0; j<m_nSets; j++) {
			m_wndPropLists[i][j].EnableHeaderCtrl(FALSE);
			m_wndPropLists[i][j].EnableDescriptionArea();
			m_wndPropLists[i][j].SetVSDotNetLook();
			m_wndPropLists[i][j].MarkModifiedProperties();
		}
	}

	AdjustLayout();
	return 0;
}

void CEvTeachPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CEvTeachPropertiesWnd::InitGroup(IMAGEVIEW_INFO *pView, CParameters* pParms, int nIndex)
{
	CArray<CParmInfo*, CParmInfo*>* pParm = &pParms->arrParmInfo;
	CString strInspection = pParms->strInspection;
	CPropertyGridProperty *pGroup, *pSection;
	CMFCPropertyGridProperty *pProp;

	CParmInfo* pParmInfo;
	CString strSection, strDesc, strLimit;
	int nSectionId, nPropId;
	_variant_t var;

	BOOL bCreate = (m_wndPropLists[m_nCurPos][m_nCurSet].GetPropertyCount() <= nIndex);

	if(bCreate) {
		pGroup = new CPropertyGridProperty(_T(strInspection));
		m_wndPropLists[m_nCurPos][m_nCurSet].AddProperty(pGroup);
	}
	else
		pGroup = (CPropertyGridProperty*) m_wndPropLists[m_nCurPos][m_nCurSet].GetProperty(nIndex);

	strSection = "###";
	nSectionId = 0;

	int nMeasParm;
	BOOL bIsMeasurement;
	double dConversionFactor;
	m_dPixelSizeX = pView->PixCalInfo.dXPixSize;
	m_dPixelSizeY = pView->PixCalInfo.dYPixSize;

	for(int i=0; i<pParm->GetCount(); i++) {
		pParmInfo = pParm->GetAt(i);

		if(pParmInfo->bVisible) {
			if(pParmInfo->strSection != strSection) {
				strSection = pParmInfo->strSection;
				if(strSection.IsEmpty()) {
					pSection = pGroup;
					if(!bCreate)
						nPropId = nSectionId;
				}
				else {
					if(bCreate) {
						pSection = new CPropertyGridProperty(_T(strSection));
						pGroup->AddSubItem(pSection);
					}
					else {
						pSection = (CPropertyGridProperty*) pGroup->GetSubItem(nSectionId);
						nPropId = 0;
					}
				}
				if(!bCreate)
					nSectionId++;
			}
			else if(strSection.IsEmpty())
				nSectionId++;

			nMeasParm = (pParmInfo->strUnit.CompareNoCase("area")==0) ? 2 :
				((pParmInfo->strUnit.CompareNoCase("length")==0) ? 1 : 0);

			if(bCreate) {
				if(pParmInfo->strUnit.IsEmpty())
					strDesc = pParmInfo->strDescription;
				else {
					if(nMeasParm) {
						strDesc.Format("%s (%s%s)", pParmInfo->strDescription, ((m_nUnitIndex>0) && (nMeasParm==2)) ? "sq." : "", 
						(m_nUnitIndex==0) ? "pixels" : (m_nUnitIndex==1 ? "microns" : "mm"));
					}
					else
						strDesc.Format("%s", pParmInfo->strDescription);
				}
			}
			else {
				pProp = (CPropertyGridProperty*) pSection->GetSubItem(nPropId);
			}

			bIsMeasurement = nMeasParm;
			if(nMeasParm) {
				if(m_nUnitIndex==0)
					nMeasParm = 0;
				else {
					if(m_nUnitIndex==1)
						dConversionFactor = ((nMeasParm==1) ? m_dPixelSizeX*1000 : m_dPixelSizeX*m_dPixelSizeY*1000000);
					if(m_nUnitIndex==2)
						dConversionFactor = ((nMeasParm==1) ? m_dPixelSizeX : m_dPixelSizeX*m_dPixelSizeY);
				}
			}

			var.Clear();
			if(pParmInfo->parmType==TENUM) {
				int nEnumIndex = *((int*) pParmInfo->pParm);
				LPCTSTR strVal = _T(nEnumIndex < pParmInfo->strEnumeration.GetCount() ? pParmInfo->strEnumeration[nEnumIndex] : "");
				var = (_variant_t) strVal;
				if(bCreate) {
					pProp = new CPropertyGridProperty( _T(pParmInfo->strName), var, _T(strDesc));
					for(int j=0; j<pParmInfo->strEnumeration.GetCount(); j++)
						pProp->AddOption(_T(pParmInfo->strEnumeration[j]));
					pProp->AllowEdit(FALSE);
				}
				else
					pProp->SetValue(var);
			}
			else if(pParmInfo->parmType==TBOOL) {
				var = (_variant_t) (*((int*) pParmInfo->pParm) ? true : false);
				if(bCreate)
					pProp = new CPropertyGridProperty( _T(pParmInfo->strName), var, _T(strDesc));
				else
					pProp->SetValue(var);
			}
			else if(pParmInfo->parmType==TINT) {
				long nValue = (long) *((int*) pParmInfo->pParm);
				var = nMeasParm ? ((variant_t) (nValue * dConversionFactor)) : ((variant_t) nValue);

				if(bCreate) {
					strLimit.Format("\r\nRange:  [%d to %d] %s", pParmInfo->nMin, pParmInfo->nMax, pParmInfo->strUnit);
					strDesc.Append(strLimit);
					pProp = new CPropertyGridProperty( _T(pParmInfo->strName), var, _T(strDesc));
					if(!nMeasParm)
						pProp->EnableSpinControl(TRUE, pParmInfo->nMin, pParmInfo->nMax);
					else
						((CPropertyGridProperty*) pProp)->m_strFormat = (m_nUnitIndex==2) ? ((nMeasParm==1) ? "%0.3f" : "%0.6f") : "%.0f";
					((CPropertyGridProperty*) pProp)->SetLimits(pParmInfo->nMin, pParmInfo->nMax);
				}
				else
					pProp->SetValue(var);
			}
			else if(pParmInfo->parmType==TDOUBLE) {
				double dValue = (*((double*) pParmInfo->pParm));
				var = (variant_t) (nMeasParm ? (dValue * dConversionFactor) : dValue);

				if(bCreate) {
					strLimit.Format("\r\n[Range: %d to %d] %s", pParmInfo->nMin, pParmInfo->nMax, pParmInfo->strUnit);
					strDesc.Append(strLimit);
					pProp = new CPropertyGridProperty( _T(pParmInfo->strName), var, _T(strDesc));
					if(!bIsMeasurement)
						((CPropertyGridProperty*) pProp)->m_strFormat = "%.2f";
					else
						((CPropertyGridProperty*) pProp)->m_strFormat = (m_nUnitIndex==2) ? ((nMeasParm==1) ? "%0.3f" : "%0.6f") : "%.0f";
					((CPropertyGridProperty*) pProp)->SetLimits(pParmInfo->nMin, pParmInfo->nMax);
				}
				else
					pProp->SetValue(var);
			}
			else if(pParmInfo->parmType==TCOLOR) {
				var = (variant_t) ((long) (*((int*) pParmInfo->pParm)));
				if(bCreate) {
					pProp = new CPropertyGridColorProperty(_T(pParmInfo->strName), var, NULL, _T(strDesc));
					((CPropertyGridColorProperty*) pProp)->EnableOtherButton("More Colors...");
				}
				else
					((CPropertyGridColorProperty*) pProp)->SetColor(var);
			}
			if(bCreate)
				pSection->AddSubItem(pProp);
			else {
				pProp->SetOriginalValue(var);
				nPropId++;
			}
		}
	}
	pGroup->Expand(m_wndPropLists[m_nCurPos][m_nCurSet].m_bIsExpand=FALSE);
}

void CEvTeachPropertiesWnd::RefreshPropList(CEvImageDoc* pDoc, int nFOV)
{
	OnSelchangeCamerasCombo();
	OnSelchangeCamerasComboPos();
}

void CEvTeachPropertiesWnd::OnExpandAllProperties()
{
	for(int i=0; i<m_nPos; i++) {
		for(int j=0; j<m_nSets; j++) {
			m_wndPropLists[/*m_nCurPos*/i][j].m_bIsExpand = !m_wndPropLists[/*m_nCurPos*/i][j].m_bIsExpand;
			m_wndPropLists[/*m_nCurPos*/i][j].ExpandAll(m_wndPropLists[/*m_nCurPos*/i][j].m_bIsExpand);
		}
	}
}

void CEvTeachPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropLists[m_nCurPos][m_nCurSet].m_bIsExpand);
}

void CEvTeachPropertiesWnd::RevertProperties()
{
	m_wndPropLists[m_nCurPos][m_nCurSet].ResetOriginalValues();
	SetModified(FALSE);
}

void CEvTeachPropertiesWnd::OnRevertProperties()
{
	RevertProperties();
}

void CEvTeachPropertiesWnd::OnUpdateRevertProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndPropLists[m_nCurPos][m_nCurSet].m_bIsModified);
}

void CEvTeachPropertiesWnd::SaveGroup(CParameters* pParms, int nIndex)
{
	CArray<CParmInfo*, CParmInfo*>* pParm = &pParms->arrParmInfo;
	CPropertyGridProperty *pGroup, *pSection;
	CMFCPropertyGridProperty *pProp;

	CParmInfo* pParmInfo;
	CString strSection, strDesc;
	int nSectionId, nPropId;
	_variant_t var;

	pGroup = (CPropertyGridProperty*) m_wndPropLists[m_nCurPos][m_nCurSet].GetProperty(nIndex);

	strSection = "###";
	nSectionId = 0;

	int nMeasParm;
	double dConversionFactor;

	for(int i=0; i<pParm->GetCount(); i++) {
		pParmInfo = pParm->GetAt(i);
		if(pParmInfo->bVisible) {
			if(pParmInfo->strSection != strSection) {
				strSection = pParmInfo->strSection;
				if(strSection.IsEmpty()) {
					pSection = pGroup;
					nPropId = nSectionId;
				}
				else {
					pSection = (CPropertyGridProperty*) pGroup->GetSubItem(nSectionId);
					nPropId = 0;
				}
				nSectionId++;
			}
			else if(strSection.IsEmpty())
				nSectionId++;

			pProp = (CMFCPropertyGridProperty*) pSection->GetSubItem(nPropId);
			if(pProp->IsModified()) {
				nMeasParm = 0;
				if(m_nUnitIndex!=0) {
					nMeasParm = (pParmInfo->strUnit.CompareNoCase("area")==0) ? 2 :
						((pParmInfo->strUnit.CompareNoCase("length")==0) ? 1 : 0);
					if(m_nUnitIndex==1)
						dConversionFactor = ((nMeasParm==1) ? m_dPixelSizeX*1000 : m_dPixelSizeX*m_dPixelSizeY*1000000);
					if(m_nUnitIndex==2)
						dConversionFactor = ((nMeasParm==1) ? m_dPixelSizeX : m_dPixelSizeX*m_dPixelSizeY);
				}

				var.Clear();
				if(pParmInfo->parmType==TENUM) {
					var = pProp->GetValue();
					CString strVal = CString(var.bstrVal);
					for(int j=0; j<pParmInfo->strEnumeration.GetCount(); j++)
						if(pParmInfo->strEnumeration[j]==strVal) {
							int* pVal = (int*) (pParmInfo->pParm);
							if(*pVal != j) {
								LogParmChange(pParms, i, *pVal, j);
								*pVal = j;
							}
							break;
						}
				}
				else if(pParmInfo->parmType==TBOOL) {
					BOOL* pVal = (BOOL*) (pParmInfo->pParm);
					var = pProp->GetValue();
					BOOL bVal = (BOOL) var.boolVal ? TRUE : FALSE;
					if(*pVal != bVal) {
						LogParmChange(pParms, i, *pVal, bVal);
						*pVal = bVal;
					}
				}
				else if(pParmInfo->parmType==TINT) {
					int* pVal = (int*) (pParmInfo->pParm);
					var = pProp->GetValue();

					int nValue = (nMeasParm ? Round(var.dblVal / dConversionFactor) : var.intVal);
					if(nMeasParm)
						var = (variant_t) (nValue * dConversionFactor);

					if(*pVal != nValue) {
						LogParmChange(pParms, i, *pVal, nValue);
						*pVal = nValue;
					}
				}
				else if(pParmInfo->parmType==TDOUBLE) {
					double* pVal = (double*) (pParmInfo->pParm);
					var = pProp->GetValue();

					double dValue = (nMeasParm ? (var.dblVal / dConversionFactor) : var.dblVal);
					if(nMeasParm)
						var = (variant_t) (dValue * dConversionFactor);

					if(*pVal != dValue) {
						LogParmChange(pParms, i, *pVal, dValue);
						*pVal = dValue;
					}
				}
				else if(pParmInfo->parmType==TCOLOR) {
					int* pVal = (int*) (pParmInfo->pParm);
					var = pProp->GetValue();
					int nVal = var.intVal;
					//COLORREF nVal = ((CPropertyGridColorProperty*) pProp)->GetColor();
					if(*pVal != nVal) {
						LogParmChange(pParms, i, *pVal, nVal);
						*pVal = nVal;
						//((CPropertyGridColorProperty*) pProp)->SetColor(*pVal);
					}
				}
				if(var.vt != VT_EMPTY)
					pProp->SetOriginalValue(var);
			}

			nPropId++;
		}
	}
}

void CEvTeachPropertiesWnd::OnSaveProperties()
{
	SaveAllProperties();
}

void CEvTeachPropertiesWnd::OnUpdateSaveProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsModified);
}

void CEvTeachPropertiesWnd::OnSelchangeCamerasCombo()
{
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].ShowWindow(SW_HIDE);
	
	m_nCurSet = m_wndObjectCombo.GetCurSel();
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].ShowWindow(SW_SHOW);
	SelChangedCamerasCombo();
	
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].ExpandAll(m_wndPropLists[m_nCurPos][m_nCurSet].m_bIsExpand=FALSE);
}

void CEvTeachPropertiesWnd::OnSelchangeCamerasComboPos()
{
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].ShowWindow(SW_HIDE);
	
	m_nCurPos = m_wndObjectComboPos.GetCurSel();
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].ShowWindow(SW_SHOW);
	SelChangedCamerasComboPos();
	
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].ExpandAll(m_wndPropLists[m_nCurPos][m_nCurSet].m_bIsExpand=FALSE);
}


void CEvTeachPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	if(m_nCurSet >=0 && m_nCurPos >= 0)
		m_wndPropLists[m_nCurPos][m_nCurSet].SetFocus();
}

void CEvTeachPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CEvTeachPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	for(int i=0; i<m_nPos; i++) {
		for(int j=0; j<m_nSets; j++) {
			m_wndPropLists[i][j].SetFont(&m_fntPropList);
		}
	}
	m_wndObjectCombo.SetFont(&m_fntPropList);
	m_wndObjectComboPos.SetFont(&m_fntPropList);
}


void CEvTeachPropertiesWnd::OnClose()
{

	CDockablePane::OnClose();
}

void CEvTeachPropertiesWnd::SetModified(BOOL bModified)
{
	m_wndPropLists[m_nCurPos][m_nCurSet].m_bIsModified = bModified;
	if(!bModified) {
		m_bIsModified = FALSE;
		for(int i=0; i<m_nPos; i++) {
			for(int j=0; j<m_nSets; j++) {
				if(m_wndPropLists[i][j].m_bIsModified) {
					m_bIsModified = TRUE;
					break;
				}
			}
		}
	}
	else
		m_bIsModified = TRUE;
}

LRESULT CEvTeachPropertiesWnd::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);

	return 0;
}


void CEvTeachPropertiesWnd::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();

	((CMDIFrameWndEx*) AfxGetMainWnd())->SendMessage(WM_VISION_PANE_CLOSED, (WPARAM) this, (LPARAM)1);

	((CMDIFrameWndEx*) AfxGetMainWnd())->RecalcLayout();
}

void CEvTeachPropertiesWnd::OnAfterDockFromMiniFrame()
{
	((CMDIFrameWndEx*) AfxGetMainWnd())->RecalcLayout();
}
