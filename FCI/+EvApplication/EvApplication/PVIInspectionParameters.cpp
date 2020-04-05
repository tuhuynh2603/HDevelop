// PVIInspectionParameters.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "PVIInspectionParameters.h"
#include "PropertyGridProperty.h"
#include "PropertyGridColorProperty.h"
#include "Parameters.h"
#include "afxdialogex.h"
#include <direct.h>

// CPVIInspectionParameters dialog

IMPLEMENT_DYNAMIC(CPVIInspectionParameters, CDialogEx)

CPVIInspectionParameters::CPVIInspectionParameters(CWnd* pParent /*=NULL*/)
	: CDialog(CPVIInspectionParameters::IDD, pParent)
{
	m_pApp = (CApplication*)AfxGetApp();




	m_strPVIDefect = _T("");
	m_nIndex = 0;
	//  m_strNoOfPviArea = _T("");
	m_nPviArea = 1;
	nFOV = 0;
	bChangePviArea = FALSE;
	strArrayDefectSel.RemoveAll();
}

CPVIInspectionParameters::~CPVIInspectionParameters()
{
	//m_Parameters.RemoveAll();


	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	if (m_bPVIDefectImgEnable != NULL) {
		for (int nFOV = 0; nFOV< m_nNoOfFov; nFOV++)
			delete m_bPVIDefectImgEnable[nFOV];
		delete[] m_bPVIDefectImgEnable;
	}
	strArrayDefectSel.RemoveAll();
	strArrayAreaType.RemoveAll();
}

void CPVIInspectionParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PVI_DEFECT, m_ComboPVIDefect);
	DDX_Text(pDX, IDC_EDIT_PVI_DEFECT, m_strPVIDefect);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_propPVIDefect);
	//  DDX_Control(pDX, IDC_COMBO_INSPECTION_AREA, m_PviInspectionArea);
	DDX_Control(pDX, IDC_COMBO_PVI_AREA, m_ComboPviArea);
	DDX_Control(pDX, IDC_COMBO_INSPECTION_AREA, m_ComboInspectionArea);
	//  DDX_Text(pDX, IDC_EDIT_PVI_AREA, m_strNoOfPviArea);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID_INSPECTION_SETUP, m_propInspectionSetup);
	DDX_Text(pDX, IDC_EDIT_PVI_AREA, m_nPviArea);
	DDV_MinMaxInt(pDX, m_nPviArea, 1, 10);
	DDX_Control(pDX, IDC_COMBO_PVI_FOV, m_ComboFov);
	DDX_Control(pDX, IDC_COMBO_INTENSITY, m_ComboIntensity);
	//DDX_Check(pDX, IDC_CHECK_PVI_DEFECT_IMG_ENABLE, m_bPVIDefectImgEnable);
}


BEGIN_MESSAGE_MAP(CPVIInspectionParameters, CDialog)
	ON_BN_CLICKED(IDC_PVI_ADD, &CPVIInspectionParameters::OnBnClickedPviAdd)
	ON_CBN_SELCHANGE(IDC_COMBO_PVI_DEFECT, &CPVIInspectionParameters::OnSelchangeComboPviDefect)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_BN_CLICKED(IDC_PVI_DELETE, &CPVIInspectionParameters::OnBnClickedPviDelete)
	ON_BN_CLICKED(ID_PVI_APPLY, &CPVIInspectionParameters::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_PVI_AREA, &CPVIInspectionParameters::OnEnChangeEditPviArea)
	ON_CBN_SELCHANGE(IDC_COMBO_PVI_AREA, &CPVIInspectionParameters::OnCbnSelchangeComboPviArea)
	ON_CBN_SELCHANGE(IDC_COMBO_PVI_FOV, &CPVIInspectionParameters::OnCbnSelchangeComboPviFov)
	ON_BN_CLICKED(IDCANCEL, &CPVIInspectionParameters::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_INTENSITY, &CPVIInspectionParameters::OnCbnSelchangeComboIntensity)
	ON_BN_CLICKED(IDC_CHECK_PVI_DEFECT_IMG_ENABLE, &CPVIInspectionParameters::OnBnClickedCheckPviDefectImgEnable)
END_MESSAGE_MAP()


// CPVIInspectionParameters message handlers


void CPVIInspectionParameters::OnBnClickedPviAdd()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_strPVIDefect.IsEmpty())
	{
		AfxMessageBox("Defect Name Missing");
		return;
	}
	int nPos = 0;
	CString resToken = m_strPVIDefect.Tokenize(" ", nPos);
	if (resToken.IsEmpty())
	{
		AfxMessageBox("Defect Name Missing");
		m_strPVIDefect = "";
		return;
	}
	BOOL bCreate = FALSE;

	
	for (int i = 0; i<pPviInspectionSetup->nDefectCount; i++)
	{
		CString strTempDefect;
		m_ComboPVIDefect.GetLBText(i, strTempDefect);
		if (m_strPVIDefect.CompareNoCase(strTempDefect) == 0)
		{
			CString strMsg;
			strMsg.Format("The %s Defect is Already Present \n Please Give Other name", m_strPVIDefect);
			AfxMessageBox(strMsg);
			m_strPVIDefect = "";
			return;
		}

	}
	int nCount = m_PviParametersDefect.GetCount();
	for (int i = 0; i < nCount; i++) {
		pParms = m_PviParametersDefect.GetAt(i);
		int nParmCount = pParms->arrParmInfo.GetCount();
		for (int j = 0; j<pParms->arrParmInfo.GetCount(); j++) {
			delete pParms->arrParmInfo[j];
		}
		pParms->Clean();
		delete pParms;
	}
	m_PviParametersDefect.RemoveAll();
	int nAreaIndex = m_ComboPviArea.GetCurSel();
	pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].strDefectName = m_strPVIDefect;
	CString strDefectId;
	strDefectId.Format("%d%d%d%d%d", nFOV+1, m_nTrackId + 1, m_nDocId + 1, nAreaIndex + 1, pPviInspectionSetup->nDefectCount+1);
	pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].m_nID = atoi(strDefectId);
	pPviParms = new CParameters();
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].bEnable, "Enable", TBOOL, "", 0, 1, "", ""));
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].nDefectCharacteristics, "DefectCharacteristics", TENUM, "", 0, 1, "", "0 - Black; 1 - White", &strArrayDefectSel));
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].m_nSize, "Size", TINT, "", 0, 50000, "Microns2", "Minimum Area of Defect"));
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].nContrast, "Contrast", TINT, "", 0, 250, "levels", "Contrast on Sub Image"));
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].nMinLength, "MinLength", TINT, "", 0, 5000, "Microns", "Min Length of Defect"));
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].nMinSquareSize, "MinSquareSize", TINT, "", 0, 5000, "Microns", "Minimum Width & Height of Defect"));
	pPviParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].nMinCount, "MinCount", TINT, "", 0, 250, "", "Minimum Number of Defect Count"));

	if (pPviInspectionSetup->nDefectCount == 0)
		bCreate = TRUE;
	int nAddedIndex;
	
	m_ComboPVIDefect.AddString(pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].strDefectName);
	nAddedIndex = m_ComboPVIDefect.FindString(-1, pPviInspectionSetup->m_pPviDefect[pPviInspectionSetup->nDefectCount].strDefectName);
	m_PviParametersDefect.Add(pPviParms);
	InitGroup(m_PviParametersDefect.GetAt(0), 0, &m_propPVIDefect, TRUE);
	pPviInspectionSetup->nDefectCount++;
	

	if (pPviInspectionSetup->nDefectCount == PVI_DEFECT_COUNT - 1)
		GetDlgItem(IDC_PVI_ADD)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_PVI_ADD)->EnableWindow(TRUE);

	m_strPVIDefect = "";

	m_propPVIDefect.ExpandAll();
	m_propPVIDefect.ShowWindow(TRUE);
	m_ComboPVIDefect.SetCurSel(nAddedIndex);
	UpdateData(FALSE);
}





CPropertyGridProperty::CPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData, BOOL bIsValueList) : CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
	m_strFormat = "";
	m_nMin = 0;
	m_nMax = -1;
}

CPropertyGridProperty::CPropertyGridProperty(const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr,
	DWORD_PTR dwData, LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	m_strFormat = "";
}



CString CPropertyGridProperty::FormatProperty()
{
	COleVariant& var = m_varValue;
	if (m_strFormat.IsEmpty() || (var.vt != VT_R8))
		return CMFCPropertyGridProperty::FormatProperty();

	else {
		ASSERT_VALID(this);
		ASSERT_VALID(m_pWndList);

		CString strVal;
		strVal.Format(m_strFormat, (double)var.dblVal);
		return strVal;
	}
}

void CPropertyGridProperty::SetLimits(int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;
}

BOOL CPropertyGridProperty::OnEndEdit()
{
	CMFCPropertyGridProperty::OnEndEdit();

	if (m_nMin > m_nMax)
		return TRUE;

	if (m_varValue.vt == VT_I4) {
		if (m_varValue.intVal < m_nMin) {
			m_varValue.intVal = m_nMin;
			Redraw();
		}
		else if (m_varValue.intVal > m_nMax) {
			m_varValue.intVal = m_nMax;
			Redraw();
		}
	}
	else if (m_varValue.vt == VT_R8) {
		if (m_varValue.dblVal < m_nMin) {
			m_varValue.dblVal = m_nMin;
			Redraw();
		}
		else if (m_varValue.dblVal > m_nMax) {
			m_varValue.dblVal = m_nMax;
			Redraw();
		}
	}

	return TRUE;
}


CPropertyGridColorProperty::CPropertyGridColorProperty(const CString& strName, const COLORREF& color, CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
{
}

void CPropertyGridColorProperty::ResetOriginalValue()
{
	CMFCPropertyGridProperty::ResetOriginalValue();
}


BOOL CPVIInspectionParameters::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	m_nNoOfFov = nFovPerX * nFovPerY;

	int nBottomNo = 0;
	int nSideNo = 0;
	strArrayAreaType.Add("Rectangle");
	strArrayAreaType.Add("Polygon");
	strArrayAreaType.Add("Ellipse");

	strArrayDefectSel.RemoveAll();
	strArrayDefectSel.Add("Black");
	strArrayDefectSel.Add("White");

	CApplication *pApp = (CApplication*)AfxGetApp();

	CRect rect;
	m_propPVIDefect.GetWindowRect(&rect);

	HDITEM hdItem;
	hdItem.mask = HDI_WIDTH; // indicating cxy is width
	hdItem.cxy = rect.Width() / 2; // whatever you want the property name column width to be
	m_propPVIDefect.GetHeaderCtrl().SetItem(0, &hdItem);
	// TODO:  Add extra initialization here
	m_propPVIDefect.SetVSDotNetLook(TRUE);
	m_ComboInspectionArea.AddString("Device");
	m_ComboInspectionArea.SetCurSel(0);
	//OnCbnSelchangeComboInspectionArea();
	m_nPviArea = 1;
	for (int i = 0; i<m_nPviArea; i++)
	{
		CString strArea;
		strArea.Format("%d", i + 1);
		m_ComboPviArea.AddString(strArea);

	}
	m_propInspectionSetup.GetHeaderCtrl().SetItem(0, &hdItem);
	// TODO:  Add extra initialization here
	m_propInspectionSetup.SetVSDotNetLook(TRUE);
	CString strTemp;
	for (int i = 0; i< m_nNoOfFov; i++)
	{
		strTemp.Format("Position_%d", i + 1);
		m_ComboFov.AddString(strTemp);
	}
	CString str;
	for (int i = 0; i<pApp->m_nTotalViews; i++) {
		/*str.Format("Camera %d", i+1);*/
		if (i<pApp->m_nTop)
			str.Format("%s", pApp->m_strTopCameraName.GetAt(i));
		else if (i >= pApp->m_nTop && i< (pApp->m_nTop + pApp->m_nBottom)) {
			str.Format("%s", pApp->m_strBottomCameraName.GetAt(nBottomNo));
			nBottomNo++;
		}
		else {
			str.Format("%s", pApp->m_strSideCameraName.GetAt(nSideNo));
			nSideNo++;
		}
		m_ComboIntensity.AddString(_T(str));

	}

	m_bPVIDefectImgEnable = new BOOL*[m_nNoOfFov];
	for (int nFOV = 0; nFOV< m_nNoOfFov; nFOV++) {
		m_bPVIDefectImgEnable[nFOV] = new BOOL[pApp->m_nTotalViews];
	}

	m_ComboFov.SetCurSel(0);
	GetDlgItem(IDC_PVI_DELETE)->EnableWindow(FALSE);
	//UpdateData(TRUE);
	return TRUE;
}

void CPVIInspectionParameters::Initialize(int nFov)
{
	UpdateData();

	//Check if have changed of number of fov
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int nNoOfFovNew = nFovPerX * nFovPerY;
	if (m_nNoOfFov != nNoOfFovNew) {	
		CString strTemp;
		m_ComboFov.ResetContent();
		for (int i = 0; i < nNoOfFovNew; i++)
		{
			strTemp.Format("Position_%d", i + 1);
			m_ComboFov.AddString(strTemp);
		}
		if (nFov >= nNoOfFovNew)
			nFov = 0;
	}

	nFOV = nFov;
	int nTemp;
	nTemp = !m_nTrackId ? m_nDocId : (m_nTrackId == 2 ? (m_nDocId + m_pApp->m_nTop + m_pApp->m_nBottom) : (m_nDocId + m_pApp->m_nTop));
	m_ComboIntensity.SetCurSel(nTemp);
	m_ComboInspectionArea.SetCurSel(0);
	m_ComboFov.SetCurSel(nFov);
	pInspHandler = &m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_InspectionHandler[nFOV];
	pInspHandler->GetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);
	OnCbnSelchangeComboPviFov();
	CString strArea;
	strArea.Format("%d", pInspHandler->m_Inspection.m_TrainingData.nNoOfPviArea);
	m_nPviArea = pInspHandler->m_Inspection.m_TrainingData.nNoOfPviArea;
	UpdateEditPVIArea();
	((CButton*)GetDlgItem(IDC_CHECK_PVI_DEFECT_IMG_ENABLE))->SetCheck(0);
	ClearAllPVIDefectImg(TRUE);

	UpdateData(FALSE);
}

void CPVIInspectionParameters::OnSelchangeComboPviDefect()
{
	// TODO: Add your control notification handler code here
	if (m_ComboPVIDefect.GetCount() == 0) {
		GetDlgItem(IDC_PVI_DELETE)->EnableWindow(FALSE);
		m_propPVIDefect.RemoveAll();
		m_propPVIDefect.ShowWindow(FALSE);
	}
	m_propPVIDefect.RemoveAll();
	int nIndex = m_ComboPVIDefect.GetCurSel();
	if (nIndex != -1) {
		int nCount = m_PviParametersDefect.GetCount();
		for (int i = 0; i < nCount; i++) {
			pParms = m_PviParametersDefect.GetAt(i);
			int nParmCount = pParms->arrParmInfo.GetCount();
			for (int j = 0; j<pParms->arrParmInfo.GetCount(); j++) {
				delete pParms->arrParmInfo[j];
			}
			pParms->Clean();
			delete pParms;
		}
		m_PviParametersDefect.RemoveAll();
		pParms = new CParameters();
		pParms->strInspection = pPviInspectionSetup->m_pPviDefect[nIndex].strDefectName;//"Defect Parameters";
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].bEnable, "Enable", TBOOL, "", 0, 1, "", ""));
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].nDefectCharacteristics, "DefectCharacteristics", TENUM, "", 0, 1, "", "0 - Black; 1 - White", &strArrayDefectSel));
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].m_nSize, "Size", TINT, "", 0, 50000, "Microns2", "Minimum Area of Defect"));
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].nContrast, "Contrast", TINT, "", 0, 250, "levels", "Contrast on Sub Image"));
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].nMinLength, "MinLength", TINT, "", 0, 5000, "Microns", "Min Length of Defect"));
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].nMinSquareSize, "MinSquareSize", TINT, "", 0, 5000, "Microns", "Minimum Width & Height of Defect"));
		pParms->Add(new CParmInfo(&pPviInspectionSetup->m_pPviDefect[nIndex].nMinCount, "MinCount", TINT, "", 0, 250, "", "Minimum Number of Defect Count"));

		m_PviParametersDefect.Add(pParms);
		InitGroup(m_PviParametersDefect.GetAt(0), 0, &m_propPVIDefect, TRUE);
		GetDlgItem(IDC_PVI_DELETE)->EnableWindow(TRUE);
	
		m_propPVIDefect.ShowWindow(TRUE);
		m_propPVIDefect.ExpandAll();
	}
}


LRESULT CPVIInspectionParameters::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CPVIInspectionParameters::OnBnClickedPviDelete()
{

	int nIndex = m_ComboPVIDefect.GetCurSel();
	if (nIndex != -1) {
		for (int i = nIndex; i<pPviInspectionSetup->nDefectCount; i++)
		{
			pPviInspectionSetup->m_pPviDefect[i+1].m_nID = pPviInspectionSetup->m_pPviDefect[i].m_nID;
			pPviInspectionSetup->m_pPviDefect[i] = pPviInspectionSetup->m_pPviDefect[i + 1];

		}
		int nCount = m_PviParametersDefect.GetCount();
		for (int i = 0; i < nCount; i++) {
			pParms = m_PviParametersDefect.GetAt(i);
			for (int j = 0; j<pParms[i].arrParmInfo.GetCount(); j++) {
				delete pParms[i].arrParmInfo[j];
			}
			pParms->Clean();
			delete pParms;
		}
		m_PviParametersDefect.RemoveAll();
		m_ComboPVIDefect.DeleteString(nIndex);
		pPviInspectionSetup->nDefectCount--;

		if (pPviInspectionSetup->nDefectCount == PVI_DEFECT_COUNT - 1)
			GetDlgItem(IDC_PVI_ADD)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_PVI_ADD)->EnableWindow(TRUE);

		m_ComboPVIDefect.SetCurSel(0);
		OnSelchangeComboPviDefect();
	}
	if (m_ComboPVIDefect.GetCount() == 0) {
		GetDlgItem(IDC_PVI_DELETE)->EnableWindow(FALSE);
		m_propPVIDefect.RemoveAll();
		m_propPVIDefect.ShowWindow(FALSE);
	}

	UpdateData(TRUE);
}




void CPVIInspectionParameters::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	CApplication *pApp = (CApplication*)AfxGetApp();
	CString strDataFileName;
	int nIndex = m_ComboPVIDefect.GetCurSel();

	int nAreaIndex = m_ComboPviArea.GetCurSel();
	if (nAreaIndex != -1) {
		int nCount = m_PviParametersInspSetUP.GetCount();
		for (int i = 0; i < nCount; i++) {
			SaveGroup(m_PviParametersInspSetUP[i], i, &m_propInspectionSetup);
		}
	}

	int nCount = m_PviParametersInspSetUP.GetCount();
	for (int i = 0; i < nCount; i++) {
		pParms = m_PviParametersInspSetUP.GetAt(i);
		int nParmCount = pParms->arrParmInfo.GetCount();
		for (int j = 0; j<pParms->arrParmInfo.GetCount(); j++) {
			delete pParms->arrParmInfo[j];
		}
		pParms->Clean();
		delete pParms;
	}
	m_propInspectionSetup.RemoveAll();
	m_PviParametersInspSetUP.RemoveAll();
	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "PVI Inspection SetUP";
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bEnable, "Enable", TBOOL, "", 0, 100, "", "Enable PVI Inspection Area SetUp"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nDeviceMaskNumber, "Device Mask Number", TINT, "", 0, 10, "", "Number of Area to be Masked"));
	/*pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dDeviceEdgeWidth, "Device Edge Width", TDOUBLE, "", 0, 2000, "", ""));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dDeviceEdgeOffset, "Device Edge Offset", TDOUBLE, "", 0, 2000, "", ""));
	*/
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nDeviceAreaType, "Device Area Type", TENUM, "", 0, 2, "", "0-Rectangle and 1 - Eclipse", &strArrayAreaType));
	
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nPolygonPointNumber, "Polygon Point Count", TINT, "", 3, 20, "", "Number Of Points For Polygon"));

	for (int nMask = 0; nMask<pPviInspectionSetup->nDeviceMaskNumber; nMask++) {
		CString strTemp;
		strTemp.Format("Device Mask Type_%d", nMask + 1);
		pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nDeviceMaskArea[nMask], strTemp, TENUM, "", 0, 2, "", "0-Rectangle and 1 - Eclipse", &strArrayAreaType));
	}
	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(0), 0, &m_propInspectionSetup);
	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "Edge PVI Inspection SetUP";
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bEdgeEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Edge PVI Inspection Area SetUp"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nMinEdgeContrast, "Min. Intensity", TINT, "", 0, 255, "levels", "Minimum Intensity of Encap"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nMaxEdgeContrast, "Max. Intensity", TINT, "", 0, 255, "levels", "Maximum Intensity of Encap"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dEdgeRegionOpen, "Edge Region Opening", TDOUBLE, "", 0, 600, "Mircons", ""));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dEdgeOffset, "Edge Offset", TDOUBLE, "", 0, 6000, "Mircons", "Offset from Die Edge"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dEdgeWidth, "Edge Width", TDOUBLE, "", 0, 6000, "Microns", "Width of Inspection Area"));
	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(1), 0, &m_propInspectionSetup);
	
	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "PVI Inspection Recheck SetUP";

	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVIRecheckEnable, "Recheck Enable", TBOOL, "", 0, 1, "", "Enable PVI Inspection Recheck"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nMinGVDiff, "Min. Mean GV Diff", TINT, "", 0, 255, "levels", "Minimum Gray Mean Value Difference between defect region and dilated boundary region"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nIntensity, "Max Contrast", TINT, "", 0, 255, "levels", "Contrast difference between inspected defect region and Teach region."));
	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(2), 0, &m_propInspectionSetup);

	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "PVI Slot Connection Check SetUP";

	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckEnable, "Slot Connection Check", TBOOL, "", 0, 1, "", "Enable PVI Slot Connection Check"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nNumOfSlot, "Number of Slot", TINT, "", 0, 10, "", "Numer of Slot"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckTopDie, "Top Die Connection Check", TBOOL, "", 0, 1, "", "Enable To Checking PVI Defect Connected From The Top Side Of PVI Area"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckBottomDie, "Bottom Die Connection Check", TBOOL, "", 0, 1, "", " To Checking PVI Defect Connected From The Bottom Side Of PVI Area"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckLeftDie, "Left Die Connection Check", TBOOL, "", 0, 1, "", " To Checking PVI Defect Connected From The Left Side Of PVI Area"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckRightDie, "Right Die Connection Check", TBOOL, "", 0, 1, "", " To Checking PVI Defect Connected From The Right Side Of PVI Area"));

	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(3), 0, &m_propInspectionSetup);

	nCount = m_PviParametersInspSetUP.GetCount();
	for (int i = 0; i < nCount; i++) {
		SaveGroup(m_PviParametersInspSetUP.GetAt(i), i, &m_propInspectionSetup);
	}

	if (nIndex != -1)
		SaveGroup(m_PviParametersDefect[0], 0, &m_propPVIDefect);

	pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);
	m_propInspectionSetup.ExpandAll();

	
	UpdateData(TRUE);
}



void CPVIInspectionParameters::SaveGroup(CParameters* pParms, int nIndex, CMFCPropertyGridCtrl *m_propWindow)
{
	CArray<CParmInfo*, CParmInfo*>* pParm = &pParms->arrParmInfo;
	CPropertyGridProperty *pGroup, *pSection;
	CMFCPropertyGridProperty *pProp;

	CParmInfo* pParmInfo;
	CString strSection, strDesc;
	int nSectionId, nPropId;
	_variant_t var;

	pGroup = (CPropertyGridProperty*)m_propWindow->GetProperty(nIndex);

	strSection = "###";
	nSectionId = 0;

	int nMeasParm;
	double dConversionFactor;

	for (int i = 0; i<pParm->GetCount(); i++) {
		pParmInfo = pParm->GetAt(i);
		if (pParmInfo->bVisible) {
			if (pParmInfo->strSection != strSection) {
				strSection = pParmInfo->strSection;
				if (strSection.IsEmpty()) {
					pSection = pGroup;
					nPropId = nSectionId;
				}
				else {
					pSection = (CPropertyGridProperty*)pGroup->GetSubItem(nSectionId);
					nPropId = 0;
				}
				nSectionId++;
			}
			else if (strSection.IsEmpty())
				nSectionId++;

			pProp = (CMFCPropertyGridProperty*)pSection->GetSubItem(nPropId);
			if (pProp->IsModified()) {
				nMeasParm = 0;
				if (m_nUnitIndex != 0) {
					nMeasParm = (pParmInfo->strUnit.CompareNoCase("area") == 0) ? 2 :
						((pParmInfo->strUnit.CompareNoCase("length") == 0) ? 1 : 0);
					if (m_nUnitIndex == 1)
						dConversionFactor = ((nMeasParm == 1) ? m_dPixelSizeX * 1000 : m_dPixelSizeX*m_dPixelSizeY * 1000000);
					if (m_nUnitIndex == 2)
						dConversionFactor = ((nMeasParm == 1) ? m_dPixelSizeX : m_dPixelSizeX*m_dPixelSizeY);
				}

				var.Clear();
				if (pParmInfo->parmType == TENUM) {
					var = pProp->GetValue();
					CString strVal = CString(var.bstrVal);
					for (int j = 0; j<pParmInfo->strEnumeration.GetCount(); j++)
						if (pParmInfo->strEnumeration[j] == strVal) {
							int* pVal = (int*)(pParmInfo->pParm);
							if (*pVal != j) {
								LogParmChange(pParms, i, *pVal, j);
								*pVal = j;
							}
							break;
						}
				}
				else if (pParmInfo->parmType == TBOOL) {
					BOOL* pVal = (BOOL*)(pParmInfo->pParm);
					var = pProp->GetValue();
					BOOL bVal = (BOOL)var.boolVal ? TRUE : FALSE;
					if (*pVal != bVal) {
						LogParmChange(pParms, i, *pVal, bVal);
						*pVal = bVal;
					}
				}
				else if (pParmInfo->parmType == TINT) {
					int* pVal = (int*)(pParmInfo->pParm);
					var = pProp->GetValue();

					int nValue = (nMeasParm ? Round(var.dblVal / dConversionFactor) : var.intVal);
					if (nMeasParm)
						var = (variant_t)(nValue * dConversionFactor);

					if (*pVal != nValue) {
						LogParmChange(pParms, i, *pVal, nValue);
						*pVal = nValue;
					}
				}
				else if (pParmInfo->parmType == TDOUBLE) {
					double* pVal = (double*)(pParmInfo->pParm);
					var = pProp->GetValue();

					double dValue = (nMeasParm ? (var.dblVal / dConversionFactor) : var.dblVal);
					if (nMeasParm)
						var = (variant_t)(dValue * dConversionFactor);

					if (*pVal != dValue) {
						LogParmChange(pParms, i, *pVal, dValue);
						*pVal = dValue;
					}
				}
				else if (pParmInfo->parmType == TCOLOR) {
					int* pVal = (int*)(pParmInfo->pParm);
					var = pProp->GetValue();
					int nVal = var.intVal;
					//COLORREF nVal = ((CPropertyGridColorProperty*) pProp)->GetColor();
					if (*pVal != nVal) {
						LogParmChange(pParms, i, *pVal, nVal);
						*pVal = nVal;
						//((CPropertyGridColorProperty*) pProp)->SetColor(*pVal);
					}
				}
				if (var.vt != VT_EMPTY)
					pProp->SetOriginalValue(var);
			}

			nPropId++;
		}
	}
}



void CPVIInspectionParameters::LogParmChange(CParameters* pParm, int nInfoIndex, double dOldValue, double dNewValue)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	CString strMessage;
	strMessage.Format("Pvi Inspection Parm-Change [Position:%d,  Intensity:%s,    %s.%s ] %3.1f->%3.1f",
		nFOV, m_nTrackId == 0 ? pApp->m_strTopCameraName[m_nDocId] : m_nTrackId == 1 ? pApp->m_strBottomCameraName[m_nDocId] : pApp->m_strSideCameraName[m_nDocId],
		pParm->strInspection,
		pParm->arrParmInfo[nInfoIndex]->strName,
		dOldValue,
		dNewValue);
	pApp->AddLogEvent(strMessage);
}


void CPVIInspectionParameters::InitGroup(CParameters* pParms, int nIndex, CMFCPropertyGridCtrl *m_propWindow, BOOL bDefect)
{
	CArray<CParmInfo*, CParmInfo*>* pParm = &pParms->arrParmInfo;
	CString strInspection = pParms->strInspection;
	CPropertyGridProperty *pGroup, *pSection;
	CMFCPropertyGridProperty *pProp;

	if (bDefect)
		pParms->strInspection = "Defect Parameters";
	CParmInfo* pParmInfo;
	CString strSection, strDesc, strLimit;
	int nSectionId, nPropId;
	_variant_t var;

	BOOL bCreate = TRUE;
	if (nIndex > 0)
		bCreate = FALSE;
	if (bCreate) {
		pGroup = new CPropertyGridProperty(pParms->strInspection);
		m_propWindow->AddProperty(pGroup);
	}
	else
		pGroup = (CPropertyGridProperty*)m_propWindow->GetProperty(0);

	strSection = "###";
	nSectionId = 0;

	int nMeasParm;
	BOOL bIsMeasurement;
	double dConversionFactor;
	m_dPixelSizeX = m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_ImageViewInfo.PixCalInfo.dXPixSize;
	m_dPixelSizeY = m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_ImageViewInfo.PixCalInfo.dYPixSize;

	for (int i = 0; i<pParm->GetCount(); i++) {
		pParmInfo = pParm->GetAt(i);

		if (pParmInfo->bVisible) {
			if (pParmInfo->strSection != strSection) {
				strSection = pParmInfo->strSection;
				if (strSection.IsEmpty()) {
					pSection = pGroup;
					if (!bCreate)
						nPropId = nSectionId;
				}
				else {
					if (bCreate) {
						pSection = new CPropertyGridProperty(_T(strSection));
						pGroup->AddSubItem(pSection);
					}
					else {
						pSection = (CPropertyGridProperty*)pGroup->GetSubItem(nSectionId);
						nPropId = 0;
					}
				}
				if (!bCreate)
					nSectionId++;
			}
			else if (strSection.IsEmpty())
				nSectionId++;

			nMeasParm = (pParmInfo->strUnit.CompareNoCase("area") == 0) ? 2 :
				((pParmInfo->strUnit.CompareNoCase("length") == 0) ? 1 : 0);

			if (bCreate) {
				if (pParmInfo->strUnit.IsEmpty())
					strDesc = pParmInfo->strDescription;
				else {
					if (nMeasParm) {
						strDesc.Format("%s (%s%s)", pParmInfo->strDescription, ((m_nUnitIndex>0) && (nMeasParm == 2)) ? "sq." : "",
							(m_nUnitIndex == 0) ? "pixels" : (m_nUnitIndex == 1 ? "microns" : "mm"));
					}
					else
						strDesc.Format("%s", pParmInfo->strDescription);
				}
			}
			else {
				pProp = (CPropertyGridProperty*)pSection->GetSubItem(nPropId);
			}

			bIsMeasurement = nMeasParm;
			if (nMeasParm) {
				if (m_nUnitIndex == 0)
					nMeasParm = 0;
				else {
					if (m_nUnitIndex == 1)
						dConversionFactor = ((nMeasParm == 1) ? m_dPixelSizeX * 1000 : m_dPixelSizeX*m_dPixelSizeY * 1000000);
					if (m_nUnitIndex == 2)
						dConversionFactor = ((nMeasParm == 1) ? m_dPixelSizeX : m_dPixelSizeX*m_dPixelSizeY);
				}
			}

			var.Clear();
			if (pParmInfo->parmType == TENUM) {
				int nEnumIndex = *((int*)pParmInfo->pParm);
				LPCTSTR strVal = _T(nEnumIndex < pParmInfo->strEnumeration.GetCount() ? pParmInfo->strEnumeration[nEnumIndex] : "");
				var = (_variant_t)strVal;
				if (bCreate) {
					pProp = new CPropertyGridProperty(_T(pParmInfo->strName), var, _T(strDesc));
					for (int j = 0; j<pParmInfo->strEnumeration.GetCount(); j++)
						pProp->AddOption(_T(pParmInfo->strEnumeration[j]));
					pProp->AllowEdit(FALSE);
				}
				else
					pProp->SetValue(var);
			}
			else if (pParmInfo->parmType == TBOOL) {
				var = (_variant_t)(*((int*)pParmInfo->pParm) ? true : false);
				if (bCreate)
					pProp = new CPropertyGridProperty(_T(pParmInfo->strName), var, _T(strDesc));
				else
					pProp->SetValue(var);
			}
			else if (pParmInfo->parmType == TINT) {
				long nValue = (long) *((int*)pParmInfo->pParm);
				var = nMeasParm ? ((variant_t)(nValue * dConversionFactor)) : ((variant_t)nValue);

				if (bCreate) {
					strLimit.Format("\r\nRange:  [%d to %d] %s", pParmInfo->nMin, pParmInfo->nMax, pParmInfo->strUnit);
					strDesc.Append(strLimit);
					pProp = new CPropertyGridProperty(_T(pParmInfo->strName), var, _T(strDesc));
					if (!nMeasParm)
						pProp->EnableSpinControl(TRUE, pParmInfo->nMin, pParmInfo->nMax);
					else
						((CPropertyGridProperty*)pProp)->m_strFormat = (m_nUnitIndex == 2) ? ((nMeasParm == 1) ? "%0.3f" : "%0.6f") : "%.0f";
					((CPropertyGridProperty*)pProp)->SetLimits(pParmInfo->nMin, pParmInfo->nMax);
				}
				else
					pProp->SetValue(var);
			}
			else if (pParmInfo->parmType == TDOUBLE) {
				double dValue = (*((double*)pParmInfo->pParm));
				var = (variant_t)(nMeasParm ? (dValue * dConversionFactor) : dValue);

				if (bCreate) {
					strLimit.Format("\r\n[Range: %d to %d] %s", pParmInfo->nMin, pParmInfo->nMax, pParmInfo->strUnit);
					strDesc.Append(strLimit);
					pProp = new CPropertyGridProperty(_T(pParmInfo->strName), var, _T(strDesc));
					if (!bIsMeasurement)
						((CPropertyGridProperty*)pProp)->m_strFormat = "%.2f";
					else
						((CPropertyGridProperty*)pProp)->m_strFormat = (m_nUnitIndex == 2) ? ((nMeasParm == 1) ? "%0.3f" : "%0.6f") : "%.0f";
					((CPropertyGridProperty*)pProp)->SetLimits(pParmInfo->nMin, pParmInfo->nMax);
				}
				else
					pProp->SetValue(var);
			}
			else if (pParmInfo->parmType == TCOLOR) {
				var = (variant_t)((long)(*((int*)pParmInfo->pParm)));
				if (bCreate) {
					pProp = new CPropertyGridColorProperty(_T(pParmInfo->strName), var, NULL, _T(strDesc));
					((CPropertyGridColorProperty*)pProp)->EnableOtherButton("More Colors...");
				}
				else
					((CPropertyGridColorProperty*)pProp)->SetColor(var);
			}
			if (bCreate)
				pSection->AddSubItem(pProp);
			else {
				pProp->SetOriginalValue(var);
				nPropId++;
			}
		}
	}
	pGroup->Expand(TRUE);
}



void CPVIInspectionParameters::OnEnChangeEditPviArea()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	bChangePviArea = TRUE;
	UpdateEditPVIArea();
}

void CPVIInspectionParameters::UpdateEditPVIArea()
{
	if (m_nPviArea > 0 && m_nPviArea <= 10) {
		m_ComboPviArea.ResetContent();
		pInspHandler->m_Inspection.m_TrainingData.nNoOfPviArea = m_nPviArea;
		for (int i = 0; i<m_nPviArea; i++) {
			CString str;
			str.Format("%d", i + 1);
			m_ComboPviArea.AddString(str);
		}
		GetDlgItem(IDC_PVI_ADD)->EnableWindow(TRUE);
		m_ComboPviArea.SetCurSel(0);
		OnCbnSelchangeComboPviArea();
	}
	else if (m_nPviArea <= 0 || m_nPviArea > 10) {
		GetDlgItem(IDC_PVI_ADD)->EnableWindow(FALSE);
		m_ComboPviArea.ResetContent();
	}

}

void CPVIInspectionParameters::OnCbnSelchangeComboPviArea()
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	if (!bChangePviArea) {
		pInspHandler->GetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);
	}
	

	int nAreaIndex = m_ComboPviArea.GetCurSel();
	
	int nCount = m_PviParametersInspSetUP.GetCount();
	for (int i = 0; i < nCount; i++) {
		pPviParmsInspSetup = m_PviParametersInspSetUP.GetAt(i);
		int nParmCount = pPviParmsInspSetup->arrParmInfo.GetCount();
		for (int j = 0; j<pPviParmsInspSetup->arrParmInfo.GetCount(); j++) {
			delete pPviParmsInspSetup->arrParmInfo[j];
		}
		pPviParmsInspSetup->Clean();
		delete pPviParmsInspSetup;
	}
	m_propInspectionSetup.RemoveAll();
	m_PviParametersInspSetUP.RemoveAll();
	pPviInspectionSetup = &pInspHandler->m_Inspection.m_pPviInspectionSetup[nAreaIndex];
	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "PVI Inspection SetUP";
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable PVI Inspection Area SetUp"));
	//pParmsInspSetup->Add(new CParmInfo(&pInspHandler->m_Inspection.m_pPviInspectionSetup[i].bNonUniformSurface,"Non Uniform Resource", TBOOL, "", 0, 1, "", ""));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nDeviceMaskNumber, "Device Mask Number", TINT, "", 0, 10, "", "Number of Area to be Masked"));
	//pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dDeviceEdgeWidth, "Device Edge Width", TDOUBLE, "", 0, 2000, "", ""));
	//pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dDeviceEdgeOffset, "Device Edge Offset", TDOUBLE, "", 0, 2000, "", ""));

	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nDeviceAreaType, "Device Area Type", TENUM, "", 0, 2, "", "0-Rectangle and 1 - Eclipse", &strArrayAreaType));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nPolygonPointNumber, "Polygon Point Count", TINT, "", 3, 20, "", "Number Of Points For Polygon Area"));

	for (int nMask = 0; nMask<pPviInspectionSetup->nDeviceMaskNumber; nMask++) {
		CString strTemp;
		strTemp.Format("Device Mask Type_%d", nMask + 1);
		pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nDeviceMaskArea[nMask], strTemp, TENUM, "", 0, 2, "", "0-Rectangle and 1 - Eclipse", &strArrayAreaType));
	}
	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);

	InitGroup(m_PviParametersInspSetUP.GetAt(0), 0, &m_propInspectionSetup);

	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "Edge PVI Inspection SetUP";
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bEdgeEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Edge PVI Inspection Area SetUp"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nMinEdgeContrast, "Min. Intensity", TINT, "", 0, 255, "levels", "Minimum Intensity of Encap"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nMaxEdgeContrast, "Max. Intensity", TINT, "", 0, 255, "levels", "Maximum Intensity of Encap"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dEdgeRegionOpen, "Edge Region Opening", TDOUBLE, "", 0, 600, "Mircons", ""));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dEdgeOffset, "Edge Offset", TDOUBLE, "", 0, 6000, "Mircons", "Offset from Die Edge"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->dEdgeWidth, "Edge Width", TDOUBLE, "", 0, 6000, "Microns", "Width of Inspection Area"));
	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(1), 0, &m_propInspectionSetup);

	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "PVI Inspection Recheck SetUP";
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVIRecheckEnable, "Recheck Enable", TBOOL, "", 0, 1, "", "Enable PVI Inspection Recheck"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nMinGVDiff, "Min. Mean GV Diff", TINT, "", 0, 255, "levels", "Minimum Gray Mean Value Difference between defect region and dilated boundary region"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nIntensity, "Max Contrast", TINT, "", 0, 255, "levels", "Contrast difference between inspected defect region and Teach region."));
	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(2), 0, &m_propInspectionSetup);

	pPviParmsInspSetup = new CParameters();
	pPviParmsInspSetup->strInspection = "PVI Slot Connection Check SetUP";
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckEnable, "Slot Connection Check", TBOOL, "", 0, 1, "", "Enable PVI Slot Connection Check"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->nNumOfSlot, "Number of Slot", TINT, "", 0, 10, "", "Number of Slot"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckTopDie, "Top Die Connection Check", TBOOL, "", 0, 1, "", "Enable To Checking PVI Defect Connected From The Top Side Of PVI Area"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckBottomDie, "Bottom Die Connection Check", TBOOL, "", 0, 1, "", "Enable To Checking PVI Defect Connected From The Bottom Side Of PVI Area"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckLeftDie, "Left Die Connection Check", TBOOL, "", 0, 1, "", "Enable To Checking PVI Defect Connected From The Left Side Of PVI Area"));
	pPviParmsInspSetup->Add(new CParmInfo(&pPviInspectionSetup->bPVISlotConnectionCheckRightDie, "Right Die Connection Check", TBOOL, "", 0, 1, "", "Enable To Checking PVI Defect Connected From The Right Side Of PVI Area"));

	m_PviParametersInspSetUP.Add(pPviParmsInspSetup);
	InitGroup(m_PviParametersInspSetUP.GetAt(3), 0, &m_propInspectionSetup);

	m_propInspectionSetup.ExpandAll();
	m_propPVIDefect.RemoveAll();
	m_ComboPVIDefect.ResetContent();
	if (pPviInspectionSetup->nDefectCount>0)
		m_propPVIDefect.ShowWindow(TRUE);
	else
		m_propPVIDefect.ShowWindow(FALSE);

	if (pPviInspectionSetup->nDefectCount<PVI_DEFECT_COUNT - 1)
		GetDlgItem(IDC_PVI_ADD)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_PVI_ADD)->EnableWindow(FALSE);

	for (int i = 0; i<pPviInspectionSetup->nDefectCount; i++)
	{
		m_ComboPVIDefect.AddString(pPviInspectionSetup->m_pPviDefect[i].strDefectName);
	}
	if (pPviInspectionSetup->nDefectCount > 0) {
		m_ComboPVIDefect.SetCurSel(0);
		OnSelchangeComboPviDefect();
		m_propPVIDefect.ExpandAll();
	}
	bChangePviArea = FALSE;
	UpdateData(TRUE);
}


void CPVIInspectionParameters::OnCbnSelchangeComboPviFov()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	pInspHandler->GetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);

	nFOV = m_ComboFov.GetCurSel();
	pInspHandler = &m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_InspectionHandler[nFOV];
	pInspHandler->GetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);
	m_nPviArea = pInspHandler->m_Inspection.m_TrainingData.nNoOfPviArea;
	UpdateData(FALSE);
	UpdateEditPVIArea();
}


void CPVIInspectionParameters::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CApplication *pApp = (CApplication*)AfxGetApp();

	CleanPviParms();

	/*BeginWaitCursor();
	pApp->LoadRecipes();
	EndWaitCursor();*/
	pApp->InitializeDefectCode();
	CDialog::OnCancel();
}


void CPVIInspectionParameters::OnCbnSelchangeComboIntensity()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	pInspHandler->GetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);

	nFOV = m_ComboFov.GetCurSel();
	int nIndex = m_ComboIntensity.GetCurSel();
	m_nTrackId = nIndex<m_pApp->m_nTop ? 0 : (nIndex >= (m_pApp->m_nTop + m_pApp->m_nBottom) ? 2 : 1);
	m_nDocId = !m_nTrackId ? nIndex%m_pApp->m_nTop : (m_nTrackId == 2 ? (nIndex - (m_pApp->m_nTop + m_pApp->m_nBottom)) % m_pApp->m_nSide : (nIndex - m_pApp->m_nTop) % m_pApp->m_nBottom);

	pInspHandler = &m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_InspectionHandler[nFOV];
	pInspHandler->GetPVIDefectParameters(m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->m_strConfigDir, m_nTrackId, m_nDocId, nFOV);
	m_nPviArea = pInspHandler->m_Inspection.m_TrainingData.nNoOfPviArea;

	((CButton*)GetDlgItem(IDC_CHECK_PVI_DEFECT_IMG_ENABLE))->SetCheck(0);
	ClearAllPVIDefectImg();
	GetDlgItem(IDC_CHECK_PVI_DEFECT_IMG_ENABLE)->EnableWindow((m_nTrackId == m_pApp->m_nTotalTracks - 1) ? FALSE : TRUE);
	UpdateEditPVIArea();
}

void CPVIInspectionParameters::ClearAllPVIDefectImg(BOOL bInitialize)
{
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int nNoOfFovNew = nFovPerX * nFovPerY;

	if (m_nNoOfFov != nNoOfFovNew) {	
		if (m_nNoOfFov < nNoOfFovNew) {
			if (m_bPVIDefectImgEnable != NULL) {
				for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++)
					delete m_bPVIDefectImgEnable[nFOV];
				delete[] m_bPVIDefectImgEnable;
			}
			m_nNoOfFov = nNoOfFovNew;
			m_bPVIDefectImgEnable = new BOOL*[m_nNoOfFov];
			for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
				m_bPVIDefectImgEnable[nFOV] = new BOOL[m_pApp->m_nTotalViews];
			}
		}
		else
			m_nNoOfFov = nNoOfFovNew;
	}

	for (int nTempFOV = 0; nTempFOV< m_nNoOfFov; nTempFOV++) {
		for (int nView = 0; nView<m_pApp->m_nTotalViews; nView++) {
			if (m_bPVIDefectImgEnable[nFOV][nView] && !bInitialize)
				m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->UpdateImageToOriginal();

			m_bPVIDefectImgEnable[nTempFOV][nView] = FALSE;
		}
	}
}

void CPVIInspectionParameters::OnBnClickedCheckPviDefectImgEnable()
{
	// TODO: Add your control notification handler code here
	ClearAllPVIDefectImg();

	UpdateData(TRUE);
	int nIndex = m_ComboIntensity.GetCurSel();
	m_bPVIDefectImgEnable[nFOV][nIndex] = ((CButton*)GetDlgItem(IDC_CHECK_PVI_DEFECT_IMG_ENABLE))->GetCheck();
	if (!m_bPVIDefectImgEnable[nFOV][nIndex]) {
		m_pApp->m_pTracks[m_nTrackId].m_pDoc[m_nDocId]->UpdateImageToOriginal();
	}
	UpdateData(FALSE);
}

void CPVIInspectionParameters::CleanPviParms()
{
	int nCount = m_PviParametersInspSetUP.GetCount();
	for (int i = 0; i < nCount; i++) {
		pParms = m_PviParametersInspSetUP.GetAt(i);
		int nParmCount = pParms->arrParmInfo.GetCount();
		for (int j = 0; j<pParms->arrParmInfo.GetCount(); j++) {
			delete pParms->arrParmInfo[j];
		}
		pParms->Clean();
		delete pParms;
	}
	nCount = m_PviParametersDefect.GetCount();
	for (int i = 0; i < nCount; i++) {
		pParms = m_PviParametersDefect.GetAt(i);
		for (int j = 0; j<pParms->arrParmInfo.GetCount(); j++) {
			delete pParms->arrParmInfo[j];
		}
		pParms->Clean();
		delete pParms;
	}
	m_PviParametersInspSetUP.RemoveAll();
	m_PviParametersDefect.RemoveAll();
}