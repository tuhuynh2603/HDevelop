/// CInspectionHandler: Vision Inspection manager class
/// Udupa; Jan'2015

#include "stdAfx.h"
#include "InspectionHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif






CInspectionHandler::CInspectionHandler()
{
	for(int i=0; i<6; i++)
		rcTeachLocation[i] = CRect( 100, 100, 300, 300);

	m_strErrorMsg.Empty();

	strArrayReferenceType.RemoveAll();
	strArrayReferenceType.Add("Top Left");
	strArrayReferenceType.Add("Top Right");
	strArrayReferenceType.Add("Bottom Left");
	strArrayReferenceType.Add("Bottom Right");
	strArrayReferenceType.Add("Encap Edge Left");
	strArrayReferenceType.Add("Encap Edge Right");
	strArrayReferenceType.Add("Center");

	strArrayDirectionType.RemoveAll();
	strArrayDirectionType.Add("Left To Right");
	strArrayDirectionType.Add("Top Top Bottom");
	strArrayDirectionType.Add("Right To Left");
	strArrayDirectionType.Add("Bottom To Top");
	strArrayDirectionType.Add("None");

	strArrayLineType.RemoveAll();
	strArrayLineType.Add("Vertical");
	strArrayLineType.Add("Horizontal");

	strArrayShapeType.RemoveAll();
	strArrayShapeType.Add("Rectangle");
	strArrayShapeType.Add("Polygon");
	strArrayShapeType.Add("Ellipse");

}

CInspectionHandler::~CInspectionHandler()
{
	CleanUp();
	CleanUpTeach();
	CleanUpPVI();

	strArrayReferenceType.RemoveAll();
	strArrayDirectionType.RemoveAll();
	strArrayLineType.RemoveAll();
	strArrayShapeType.RemoveAll();
}

int CInspectionHandler::LoadRecipe(CSharedData* pData,//IMAGEVIEW_INFO *pView,
									  CImageBuffer *pImgBufSrc,
									  CString strRecipeFolder,
									  CString strRecipeFile,
									  int nTrack,
									  int nDocCount,
									  int nFOV,
									  int nDoc)
{
	LoadInspectParameters(strRecipeFolder, strRecipeFile, nFOV);
	LoadTeachParameters(strRecipeFolder, strRecipeFile, nFOV);
	GetPVIDefectParameters(strRecipeFolder, nTrack, nDoc, nFOV);

	return 0;
}

int CInspectionHandler::Teach(CSharedData* pData,//IMAGEVIEW_INFO *pView,
							   CImageBuffer *pImgBufSrc,
							   CString strRecipeFolder,
							   CString strRecipeFile,
							   BOOL bAutoTeach,
							   long lDebugFlag)

{
	int nError;
	try {
		nError = 0;
	}
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::Teach] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -1;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::Teach]");
		return -1;
	}

	return nError;
}

int CInspectionHandler::Teach(CSharedData* pData,//IMAGEVIEW_INFO *pView,
							   CImageBuffer *pImgBufSrc,
							   CString strRecipeFolder,
							   CString strRecipeFile,
							   int nTeachStep,
							   BOOL *bEnable,
							   BOOL bAutoTeach,
							   int nCurTrack,
							   int nDoc,
							   int nFOV,
							   int nTotalFOV,
							   CTrainingData* pTrainingData,
							   CCalibrationData* pCalibData,
							   BOOL bEncapLoc)
{
	try {
		int nError;
		CString strTeachImageFile;

		nError = m_Inspection.Teach(pData, pImgBufSrc, strRecipeFolder, 
								nTeachStep, bEnable, bAutoTeach,
								nCurTrack, nDoc, nFOV, nTotalFOV,
								pTrainingData, pCalibData, bEncapLoc);
		return nError;
	}
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::Teach] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -1;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::Teach]");
		return -1;
	}

	return 0;
}

int CInspectionHandler::AutoTeach(CSharedData* pData,//IMAGEVIEW_INFO *pView,
							   CImageBuffer *pImgBufSrc,
							   CString strRecipeFolder,
							   CString strRecipeFile,
							   BOOL bRegionTeach,
							   int nCurTrack,
							   int nDoc,
							   int nFOV,
							   int nTotalFOV,
							   CCalibrationData* pCalibData)
{
	int nError;
	try {
		nError = m_Inspection.AutoTeach(pData, pImgBufSrc, strRecipeFolder, bRegionTeach, nCurTrack, nDoc, nFOV, nTotalFOV, pCalibData);
	}
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::AutoTeach] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -1;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::AutoTeach]");
		return -1;
	}

	return nError;
}

int CInspectionHandler::ClearTeachParam(CTrainingData* pTrainingData)
{
	int nError;
	try {
		nError = m_Inspection.ClearTeachParam(pTrainingData);
	}	
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::ClearTeachParam] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -PROCESS_ERROR;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::ClearTeachParam]");
		return -PROCESS_ERROR;
	}

	return nError;
}

int CInspectionHandler::ClearInspParam(CInspectinonData* pInspData, BOOL bInspClearAll)
{
	int nError;
	try {
		nError = m_Inspection.ClearInspParam(pInspData, bInspClearAll);
	}	
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::ClearInspParam] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -PROCESS_ERROR;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::ClearInspParam]");
		return -PROCESS_ERROR;
	}

	return nError;
}

int CInspectionHandler::ClearCalibParam(CCalibrationData* pCalibData, BOOL bClearAvg, BOOL bClearInsp)
{
	int nError;
	try {
		nError = m_Inspection.ClearCalibParam(pCalibData, bClearAvg, bClearInsp);
	}
	catch (HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::ClearCalibParam] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -PROCESS_ERROR;
	}
	catch (...) {
		LogMessage(0, "[InspectionHandler::ClearCalibParam]");
		return -PROCESS_ERROR;
	}

	return nError;
}

int CInspectionHandler::GetCalibDataForInsp(CCalibrationData* pCalibData)
{
	int nError;
	try {
		nError = m_Inspection.GetCalibDataForInsp(pCalibData);
	}
	catch (HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::GetCalibDataForInsp] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -PROCESS_ERROR;
	}
	catch (...) {
		LogMessage(0, "[InspectionHandler::GetCalibDataForInsp]");
		return -PROCESS_ERROR;
	}

	return nError;
}

int CInspectionHandler::SetTrainingData()
{
	CTrainingData* pData = &m_Inspection.m_TrainingData;

	//pData->nCuvetteRadiusRa = rcTeachLocation[0].Width()/2;
	//pData->nCuvetteRadiusRb = rcTeachLocation[0].Width()/2;
	//
	//pData->ptCuvetteTopLeftRect = rcTeachLocation[0].TopLeft();//
	//pData->ptCuvetteBotRightRect = rcTeachLocation[0].BottomRight();//

	//int nTeachSteps = m_Inspection.m_LensLocationParam.nNumberOfLayer;
	//for(int i=0; i<nTeachSteps+1; i++)
	//{
	//	pData->nPatternCols[i] = rcTeachLocation[i+1].CenterPoint().x;
	//	pData->nPatternRows[i] = rcTeachLocation[i+1].CenterPoint().y;
	//	pData->nPatternRads[i] = (rcTeachLocation[i+1].Width()/2 + rcTeachLocation[i+1].Height()/2)/2;
	//}

	//pData->nLensRadius      = rcTeachLocation[1].Width()/2;
	//pData->nPatternOuterRadius      = rcTeachLocation[2].Width()/2;
	//pData->nPatternInnerRadius      = rcTeachLocation[3].Width()/2;
	return 0;
}

int CInspectionHandler::Inspect(CSharedData* pData,
         						 long lDebugFlag,BOOL *m_bStepSelection)
{
	int nError;
	try {
		nError = 0;
	}	
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::Inspect] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -PROCESS_ERROR;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::Inspect]");
		return -PROCESS_ERROR;
	}

	return nError;
}

int CInspectionHandler::Inspect(CSharedData* pData,
								 CImageBuffer* pImgBufSrc,
								 CImageBuffer* pTeachImgBufSrc,
         						 long lDebugFlag, 
								 BOOL bRegionInsp,
								 int nCurTrack,
								 int nCurDoc,
								 int nTotalDoc,
								 int nCurFOV,
								 int nTotalFOV,
								 CTrainingData* pTrainingData,
								 CInspectinonData* pInspData,
								 CCalibrationData* pCalibData,
								 BOOL bDirForTiltInsp,
								CDeepLearningInterface * pDeepLearningModule,
								CStringArray& strArrayInspValues)
{
	int nError;
	try {
		nError = m_Inspection.Inspect(pData, pImgBufSrc, pTeachImgBufSrc,
									lDebugFlag, bRegionInsp, 
									nCurTrack, nCurDoc, nTotalDoc, nCurFOV, nTotalFOV,
									pTrainingData, pInspData, pCalibData,
									bDirForTiltInsp,
									pDeepLearningModule,
									strArrayInspValues);
	}
	catch(HOperatorException ex) {
		CString str;
		str.Format("[InspectionHandler::Inspect] %s", ex.ErrorMessage().Text());
		LogMessage(0, str);
		return -PROCESS_ERROR;
	}
	catch(...) {
		LogMessage(0, "[InspectionHandler::Inspect]");
		return -PROCESS_ERROR;
	}

	return nError;
}

void CInspectionHandler::CreateParameterList(int nTrack, int nDoc, int nFOV)
{
	//CleanUp();
	m_Parameters.SetSize(7);
	int nParamIndex = 0;
	CParameters* pParm;

	pParm = new CParameters();
	pParm->strInspection = "Cover Layer";
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Cover Layer Inspection"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nImageIndex, "Die Position ID", TINT, "", 1, 5, "value", "Specifies image intensity for cover layer inspection"));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nContrast[0], "Max. Intensity", TINT, "Cover Layer 1", 0, 255, "levels", "-Maximum Intensity of Cover Layer.\n-If the gray value of Cover Layer is less than Max.Intensity value, the Cover Layer will be found.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerLength[0], "Min. Length", TINT, "Cover Layer 1", 0, 50000, "Microns", "-Minimum Length of Cover Layer.\n-If the Length of Cover Layer is less than Min.Length value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerHeight[0], "Min. Height", TINT, "Cover Layer 1", 0, 50000, "Microns", "-Minimum Height of Cover Layer.\n-If the Height of Cover Layer is less than Min.Height value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Length value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dMaskSize[0], "Mask Size", TDOUBLE, "Cover Layer 1", 0, 2100, "Microns", "-Size of Filter Mask\n-If the size of any regions are less than Mask Size value, the regions will be removed.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Height value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapWidthTolerance[0], "Gap Width Tolerance", TDOUBLE, "Cover Layer 1", 0, 2100, "Microns", "-Width Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Width Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapHeightTolerance[0], "Gap Height Tolerance", TDOUBLE, "Cover Layer 1", 0, 2100, "Microns", "-Height Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Height Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nContrast[1], "Max. Intensity", TINT, "Cover Layer 2", 0, 255, "levels", "-Maximum Intensity of Cover Layer.\n-If the gray value of Cover Layer is less than Max.Intensity value, the Cover Layer will be found.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerLength[1], "Min. Length", TINT, "Cover Layer 2", 0, 50000, "Microns", "-Minimum Length of Cover Layer.\n-If the Length of Cover Layer is less than Min.Length value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerHeight[1], "Min. Height", TINT, "Cover Layer 2", 0, 50000, "Microns", "-Minimum Height of Cover Layer.\n-If the Height of Cover Layer is less than Min.Height value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Length value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dMaskSize[1], "Mask Size", TDOUBLE, "Cover Layer 2", 0, 2100, "Microns", "-Size of Filter Mask\n-If the size of any regions are less than Mask Size value, the regions will be removed.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Height value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapWidthTolerance[1], "Gap Width Tolerance", TDOUBLE, "Cover Layer 2", 0, 2100, "Microns", "-Width Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Width Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapHeightTolerance[1], "Gap Height Tolerance", TDOUBLE, "Cover Layer 2", 0, 2100, "Microns", "-Height Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Height Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nContrast[2], "Max. Intensity", TINT, "Cover Layer 3", 0, 255, "levels", "-Maximum Intensity of Cover Layer.\n-If the gray value of Cover Layer is less than Max.Intensity value, the Cover Layer will be found.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerLength[2], "Min. Length", TINT, "Cover Layer 3", 0, 50000, "Microns", "-Minimum Length of Cover Layer.\n-If the Length of Cover Layer is less than Min.Length value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerHeight[2], "Min. Height", TINT, "Cover Layer 3", 0, 50000, "Microns", "-Minimum Height of Cover Layer.\n-If the Height of Cover Layer is less than Min.Height value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Length value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dMaskSize[2], "Mask Size", TDOUBLE, "Cover Layer 3", 0, 2100, "Microns", "-Size of Filter Mask\n-If the size of any regions are less than Mask Size value, the regions will be removed.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Height value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapWidthTolerance[2], "Gap Width Tolerance", TDOUBLE, "Cover Layer 3", 0, 2100, "Microns", "-Width Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Width Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapHeightTolerance[2], "Gap Height Tolerance", TDOUBLE, "Cover Layer 3", 0, 2100, "Microns", "-Height Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Height Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nContrast[3], "Max. Intensity", TINT, "Cover Layer 4", 0, 255, "levels", "-Maximum Intensity of Cover Layer.\n-If the gray value of Cover Layer is less than Max.Intensity value, the Cover Layer will be found.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerLength[3], "Min. Length", TINT, "Cover Layer 4", 0, 50000, "Microns", "-Minimum Length of Cover Layer.\n-If the Length of Cover Layer is less than Min.Length value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerHeight[3], "Min. Height", TINT, "Cover Layer 4", 0, 50000, "Microns", "-Minimum Height of Cover Layer.\n-If the Height of Cover Layer is less than Min.Height value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Length value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dMaskSize[3], "Mask Size", TDOUBLE, "Cover Layer 4", 0, 2100, "Microns", "-Size of Filter Mask\n-If the size of any regions are less than Mask Size value, the regions will be removed.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Height value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapWidthTolerance[3], "Gap Width Tolerance", TDOUBLE, "Cover Layer 4", 0, 2100, "Microns", "-Width Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Width Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapHeightTolerance[3], "Gap Height Tolerance", TDOUBLE, "Cover Layer 4", 0, 2100, "Microns", "-Height Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Height Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nContrast[4], "Max. Intensity", TINT, "Cover Layer 5", 0, 255, "levels", "-Maximum Intensity of Cover Layer.\n-If the gray value of Cover Layer is less than Max.Intensity value, the Cover Layer will be found.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerLength[4], "Min. Length", TINT, "Cover Layer 5", 0, 50000, "Microns", "-Minimum Length of Cover Layer.\n-If the Length of Cover Layer is less than Min.Length value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.nCoverLayerHeight[4], "Min. Height", TINT, "Cover Layer 5", 0, 50000, "Microns", "-Minimum Height of Cover Layer.\n-If the Height of Cover Layer is less than Min.Height value, the Cover Layer will be absent.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Length value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dMaskSize[4], "Mask Size", TDOUBLE, "Cover Layer 5", 0, 2100, "Microns", "-Size of Filter Mask\n-If the size of any regions are less than Mask Size value, the regions will be removed.\n-If this value is increased too high, the Cover Layer will be absent. Otherwise, the Cover Layer will be found in mistake.\n-Should setup this value lower than Min.Height value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapWidthTolerance[4], "Gap Width Tolerance", TDOUBLE, "Cover Layer 5", 0, 2100, "Microns", "-Width Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Width Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerParm.dGapHeightTolerance[4], "Gap Height Tolerance", TDOUBLE, "Cover Layer 5", 0, 2100, "Microns", "-Height Tolerance of Gap.\n-If the Cover Layers are broken and the distance between of those less than Gap Height Tolerance value, the Cover Layers will be connected.\n-If this value is increased too high, the Cover Layer will be found in mistake. Otherwise, the Cover Layer will be absent."));

	//m_Parameters.Add(pParm);
	m_Parameters.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "Cover Layer Alignment";
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.bEnablePointer, "Enable", TBOOL, "", 0, 1, "", "Enable Cover Layer Alignment Inspection using Pointer"));
	
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.nCoverLayerDirection[0], "Cover Layer Direction", TENUM, "Pointer 1", 0, 1, "", "Specify Direction Of The Cover Layer", &strArrayLineType));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.dToleranceDist[0], "Tolerance", TDOUBLE, "Pointer 1", 0, 1000, "Microns", "-Tolerance Distance of Pointer.\n-If Distance between Pointer and Cover Layer less than Tolerance, Cover Layer Alignment will be pass."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.nCoverLayerDirection[1], "Cover Layer Direction", TENUM, "Pointer 2", 0, 1, "", "Specify Direction Of The Cover Layer", &strArrayLineType));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.dToleranceDist[1], "Tolerance", TDOUBLE, "Pointer 2", 0, 1000, "Microns", "Tolerance Distance of Pointer.\n-If Distance between Pointer and Cover Layer less than Tolerance, Cover Layer Alignment will be pass."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.nCoverLayerDirection[2], "Cover Layer Direction", TENUM, "Pointer 3", 0, 1, "", "Specify Direction Of The Cover Layer", &strArrayLineType));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.dToleranceDist[2], "Tolerance", TDOUBLE, "Pointer 3", 0, 1000, "Microns", "Tolerance Distance of Pointer.\n-If Distance between Pointer and Cover Layer less than Tolerance, Cover Layer Alignment will be pass."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.nCoverLayerDirection[3], "Cover Layer Direction", TENUM, "Pointer 4", 0, 1, "", "Specify Direction Of The Cover Layer", &strArrayLineType));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.dToleranceDist[3], "Tolerance", TDOUBLE, "Pointer 4", 0, 1000, "Microns", "Tolerance Distance of Pointer.\n-If Distance between Pointer and Cover Layer less than Tolerance, Cover Layer Alignment will be pass."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.nCoverLayerDirection[4], "Cover Layer Direction", TENUM, "Pointer 5", 0, 1, "", "Specify Direction Of The Cover Layer", &strArrayLineType));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.dToleranceDist[4], "Tolerance", TDOUBLE, "Pointer 5", 0, 1000, "Microns", "Tolerance Distance of Pointer.\n-If Distance between Pointer and Cover Layer less than Tolerance, Cover Layer Alignment will be pass."));

	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.nCoverLayerDirection[5], "Cover Layer Direction", TENUM, "Pointer 6", 0, 1, "", "Specify Direction Of The Cover Layer", &strArrayLineType));
	pParm->Add(new CParmInfo(&m_Inspection.m_CoverLayerAlignmentParm.dToleranceDist[5], "Tolerance", TDOUBLE, "Pointer 6", 0, 1000, "Microns", "Tolerance Distance of Pointer.\n-If Distance between Pointer and Cover Layer less than Tolerance, Cover Layer Alignment will be pass."));

	//m_Parameters.Add(pParm);
	m_Parameters.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "Encap";
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Excess / Insufficient Encap inspection"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.bEnableEncapSurface, "Enable Encap Surface", TBOOL, "", 0, 1, "", "-Enable Encap Surface Inspection\n-If this value is set 'true', Encap Surface will be inspected Defects. Then, all of Defects will be classified in 3 types Crack, Blow Hole and Contamination in turn."));

	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.dErosionOffset, "Edge Offset", TDOUBLE, "", 0, 600, "Microns", "Inspection offset from encap edge"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.dMaxLayOnAllowedPercentage, "Max. Lay On", TDOUBLE, "", 0, 100, "%", "Maximum length of the defect lay on the encap border allowed"));
	
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinContrastCrack, "Crack Contrast", TINT, "Crack", 0, 255, "levels", "Minimum contrast of Crack"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinSizeCrack, "Min. Size", TINT, "Crack", 0, 99999, "Microns2", "-Minimum area of Crack.\n-If the Size of Defect is less than Min. Size value, the Defect will be classified in Crack.\n-If this value is increase too high, any Defects will not be classified in Crack. Otherwise, all of Defect will be classfied in Crack."));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinLengthCrack, "Min. Length", TINT, "Crack", 0, 99999, "Microns", "-Minimum length of Crack.\n-If the Length of Defect is less than Min. Length value, the Defect will be classified in Crack.\n-If this value is increase too high, any Defects will not classified in Crack. Otherwise, all of Defect will be classfied in Crack.\n-Should setup this value higher than Blow Hole and Contamination Min. Length, because Length of Crack is usually longer than Length of Blow Hole and Contamination"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinSquareSizeCrack, "Min. Square Size", TINT, "Crack", 0, 99999, "Microns", "-Minimum width & height of Crack.\n-If the Width and Height of Defect are less than Min. Square Size value, the Defect will be classified in Crack.\n-If this value is increase too high, any Defects will not classified in Crack. Otherwise, all of Defect will be classfied in Crack."));

	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinContrastBH, "Blow Hole Contrast", TINT, "Blow Hole", 0, 255, "levels", "Minimum contrast of Blow Hole"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinSizeBH, "Min. Size", TINT, "Blow Hole", 0, 99999, "", "Minimum area of Blow Hole"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.dMinCircularityBH, "Min. Circularity", TDOUBLE, "Blow Hole", 0, 1, "", "-Minimum Circularity of Blow Hole; 1=Perfect circle.\n-If the Shape of Defect is similar to a circle, the Defect will be classified in Blow Hole.\n-Should setup this value equal to Max. Circularity of Contamination value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinLengthBH, "Min. Length", TINT, "Blow Hole", 0, 99999, "Microns", "-Minimum Length of Blow Hole.\n-If the Length of Defect is less than Min. Length value, the Defect will be classified in Blow Hole.\n-If this value is increase too high, any Defects will not be classified in Blow Hole. Otherwise, all of Defect will be classfied in Blow Hole."));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinSquareSizeBH, "Min. Square Size", TINT, "Blow Hole", 0, 99999, "Microns", "-Minimum Width & Height of Blow Hole.\n-If the Width and Height of Defect are less than Min. Square Size value, the Defect will be classified in Blow Hole.\n-If this value is increase too high, any Defects will not classified in Blow Hole. Otherwise, all of Defect will be classfied in Blow Hole."));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinGrayMeanBH, "Max. Intensity", TINT, "Blow Hole", 0, 255, "levels", "Maximum intensity of Blow Hole. Use this parameter to control detect black/white blow hole."));

	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinContrastCT, "Contamination Contrast", TINT, "Contamination", 0, 255, "", "Minimum contrast of Contamination"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinSizeCT, "Min. Size", TINT, "Contamination", 0, 99999, "", "Minimum area of Contamination"));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.dMaxCircularityCT, "Max. Circularity", TDOUBLE, "Contamination", 0, 1, "", "Maximum circularity of Contamination Is Allowed; 1=Perfect circle.\n-If the Shape of Defect is different from a circle, the Defect will be classified in Contamination.\n-Should setup this value equal to Max. Circularity of Blow Hole value."));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinLengthCT, "Min. Length", TINT, "Contamination", 0, 99999, "Microns", "-Minimum length of Contamination.\n-If the Length of Defect is less than Min. Length value, the Defect will be classified in Contamination.\n-If this value is increase too high, any Defects will not be classified in Contamination. Otherwise, all of Defect will be classfied in Contamination."));
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinSquareSizeCT, "Min. Square Size", TINT, "Contamination", 0, 99999, "Microns", "-Minimum width & height of Contamination\n-If the Width and Height of Defect are less than Min. Square Size value, the Defect will be classified in Contamination.\n-If this value is increase too high, any Defects will not classified in Contamination. Otherwise, all of Defect will be classfied in Contamination."));


	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nImageIndex, "Die Position ID", TINT, "", 1, 5, "value", "Specifies image intensity for encap inspection"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMinContrast, "Min Contrast", TINT, "", 0, 255, "levels", ""));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapParm.nMaxContrast, "Max Contrast", TINT, "", 0, 255, "levels", ""));

	//m_Parameters.Add(pParm);
	m_Parameters.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "Slot";
	pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Slot Inspection"));
	for (int k = 0; k < 5; k++) {
		CString strTemp;
		strTemp.Format("Slot %d", k + 1);
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.bEnableCheckSurface[k], "Enable Check Surface", TBOOL, strTemp, 0, 1, "", "Enable Check Surface"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nEdgeOffset[k], "Edge Offset", TINT, strTemp, 0, 9999999999, "Microns", "Inspection offset from Slot edge"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nMinContrast[k], "Contrast", TINT, strTemp, 0, 255, "Levels", "Contrast of Defect"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nMinMeanGVDiffSlot[k], "Min. Mean GV Diff", TINT, strTemp, 0, 255, "Levels", "Minimum Gray Mean Value Difference between defect region and dilated boundary region"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nMinSize[k], "Min. Size", TINT, strTemp, 0, 9999999999, "Microns2", "Minimum size of Defect"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nMinLength[k], "Min. Length", TINT, strTemp, 0, 9999999999, "Microns", "Minimum length of Defect"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nMinSquareSize[k], "Min. Square Size", TINT, strTemp, 0, 9999999999, "Microns", "Minimum Width & Height of Defect"));
		pParm->Add(new CParmInfo(&m_Inspection.m_SlotParm.nMinCount[k], "Min. Count", TINT, strTemp, 1, 100, "Minimum quantity of Defect"));
	}
	m_Parameters.SetAt(nParamIndex++, pParm);

	//pParm = new CParameters();
	//pParm->strInspection = "Die";
	//pParm->Add(new CParmInfo(&m_Inspection.m_DieInspParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Specifies enable/disable die inspection"));
	//m_Parameters.Add(pParm);

	pParm = new CParameters();
	pParm->strInspection = "Tilt";
	pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Tilt inspection"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nImageIndex, "Die Position ID", TINT, "", 1, 5, "value", "Number of image to extract object of Tilt inspection"));
	pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nContrast, "Object Intensity", TINT, "", 0, 255, "levels", "Intensity to extract object of Tilt inspection. 0=Internal calculation"));
	pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nDistanceForTiltInsp, "Measuring Width", TINT, "", 0, 5500, "Microns", "Width of object for height measurement"));
	
	//pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nRegionChar, "Characterstics", TINT, "", 0, 1, "", "0 - Black; 1 - White"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nRegionHeight, "Hieght", TINT, "", 0, 2100, "", ""));
	//pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nMeanGreyValue, "Mean Grey Value", TINT, "", 0, 255, "", ""));

	if(nTrack == 2) {
		pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nToleranceSide, "Min. Tilted Height", TINT, "", 0, 5000, "Microns", "Minimum height of Die Tilt"));
		pParm->Add(new CParmInfo(&m_Inspection.m_TiltInspParm.nAngleOfSideView, "Angle of View", TINT, "", 0, 360, "degree", "Angle of side view"));
	}
	//m_Parameters.Add(pParm);
	m_Parameters.SetAt(nParamIndex++, pParm);


	pParm = new CParameters();
	pParm->strInspection = "Die Edge";
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Die Edge Inspection"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.nDefectCharacteristics, "Defect Characteristics", TINT, "", 0, 1, "", "0 - Black; 1 - White"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.m_nSize, "Min Size", TINT, "", 0, 50000, "Microns2", "Minimum Area of Defect"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.nContrast, "Contrast", TINT, "", 0, 250, "levels", "Contrast on Sub Image"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.nMinLength, "Min Length", TINT, "", 0, 5000, "Microns", "Min Length of Defect"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.nMinSquareSize, "Min SquareSize", TINT, "", 0, 5000, "Microns", "Minimum Width & Height of Defect"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.nMinCount, "Min Count", TINT, "", 0, 250, "", "Minimum Number of Defect Count"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.dOffset, "Insp Offset", TDOUBLE, "", 0, 2000, "Microns", "Offset from Die Edge"));
	pParm->Add(new CParmInfo(&m_Inspection.m_DieEdgeParm.dWidth, "Insp Width", TDOUBLE, "", 0, 2000, "Microns", "Width of Inspection Area"));

	
	//m_Parameters.Add(pParm);
	m_Parameters.SetAt(nParamIndex++, pParm);

	/// Test bottom encap properties

	pParm = new CParameters();
	pParm->strInspection = "Encap Magnus";
	pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Encap MagnusPam"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Thresh[0], "Min Thresh", TINT, "", 0, 255, "", "Thresh"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Thresh[1], "Max Thresh", TINT, "", 0, 255, "", "Thresh"));

	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Crop_Expand[0], "Crop Expand Width", TINT, "", 0, 500, "", "Crop Expand"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Crop_Expand[1], "Crop Expand Height", TINT, "", 0, 500, "", "Crop Expand"));

	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Dilate_Kernel, "Dilate Kernel Width", TINT, "", 1, 20, "", "Dilate_Kernel"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Dilate_Kernel, "Dilate Kernel Height", TINT, "", 1, 20, "", "Dilate_Kernel"));

	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Opening_Kernel, "Opening Kernel Width", TINT, "", 1, 20, "", "Opening_Kernel"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Opening_Kernel, "Opening Kernel Height", TINT, "", 1, 20, "", "Opening_Kernel"));

	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Crop_Smooth, "Crop Smooth", TINT, "", 10, 100, "", "Crop_Smooth"));
	//pParm->Add(new CParmInfo(&m_Inspection.m_EncapMagnusPam.Area_Object, "Area Object", TINT, "", 0,9999999, "", "Area_Object"));
	

	m_Parameters.SetAt(nParamIndex++, pParm);
	//pParm->Add(new CParmInfo(&m_Inspection.m_MagnusParm.bEnable, "Enable", TBOOL, "", 0, 1, "", "Enable Die Edge Inspection"));

	/// end test

}

void CInspectionHandler::CreateTeachParmList(/*int nTrack, int nDoc, int nFOV*/)
{
	//CleanUpTeach();
	/*CArray<CParameters*> m_ParametersTeachTemp;
	m_ParametersTeachTemp.Copy(m_ParametersTeach);*/

	m_ParametersTeach.SetSize(7);
	int nParamIndex = 0;

	CParameters* pParm;
	CTrainingData* pData = &m_Inspection.m_TrainingData;

	pParm = new CParameters();
	pParm->strInspection = "Device Location";
	pParm->Add(new CParmInfo(&pData->bDeviceLocationEnable,  "Enable", TBOOL, "", 0, 1, "", "Specifies pattern search enable/disable for device location inspection"));
	//pParm->Add(new CParmInfo(&pData->nNoOfLocationTeachRegion, "No. of Device Location", TINT, "", 0, 1, "", "Specifies device width"));
	pParm->Add(new CParmInfo(&pData->dDeviceLocWidth, "Width", TDOUBLE, "", 0, 15000, "Microns", "-Minimum Device Width.\n-If Width of Device less than Width value, Device Location will be found."));
	pParm->Add(new CParmInfo(&pData->dDeviceLocHeight, "Height", TDOUBLE, "", 0, 15000, "Microns", "-Min Device Height.\n-If Height of Device less than Height value, Device Location will be found."));
	pParm->Add(new CParmInfo(&pData->dMaskSize, "Device Gap Mask", TDOUBLE, "", 0, 15000, "Microns", "-Min Gap Allowed on Device.\n-If the Size of Device Gap less than Device Gap Mask value, this gap will be looked like a part of device location."));
	pParm->Add(new CParmInfo(&pData->nMinDeviceContrast, "Intensity", TINT, "", 0, 255, "levels", "-Min Device Intensity / Gray Level.\n-If the Gray Level value of Device is less than Intensity value, the Device Location will be found.\n-If this value is increased too high, Device Location will be not found. Otherwise, Device Location will be found in mistake."));
	pParm->Add(new CParmInfo(&pData->bSobelAmp, "Use Sobel Operator", TBOOL, "", 0, 1, "", ""));

	
	pParm->Add(new CParmInfo(&pData->nDeviceReferencePointType, "Device Reference Type", TENUM, "", 0, REF_POINT_TYPE_COUNT - 1, "", "Specifies reference point type to calculate the shifted information between teach and inspect image", &strArrayReferenceType));

	//Edge Detection Option
	
	pParm->Add(new CParmInfo(&pData->bDeviceEdgeDetectionEnable, "Use Edge Line Detection", TBOOL, "", 0, 1, "", "Enable/disable for edge detection to finding the cross reference point based on the horizontal and vertical line of the device.\nFrom the cross reference point we calculate the shifted information between teach and inspect image"));
	pParm->Add(new CParmInfo(&pData->nDeviceHorizontalContrast, "     Horizontal Line Contrast", TINT, "", 0, 255, "levels", "Min Edge Contrast To Find The Horizontal Line Of The Device / Grey Level"));
	pParm->Add(new CParmInfo(&pData->nDeviceHorizontalDirection, "     Horizontal Line Search Direction", TENUM, "", 0, DIRECTION_TYPE_COUNT - 1, "", "Specifies The Starting Search Direction For The Horizontal Line Of The Device", &strArrayDirectionType));

	pParm->Add(new CParmInfo(&pData->nDeviceVerticalContrast, "     Vertical Line Contrast", TINT, "", 0, 255, "levels", "Min Edge Contrast To Find The Vertical Line Of The Device / Grey Level"));
	pParm->Add(new CParmInfo(&pData->nDeviceVerticalDirection, "     Vertical Line Search Direction", TENUM, "", 0, DIRECTION_TYPE_COUNT - 1, "", "Specifies The Starting Search Direction For The Vertical Line Of The Device", &strArrayDirectionType));

	pParm->Add(new CParmInfo(&pData->bUseSmoothingForEdgeDetection, "Use Smoothing Image", TBOOL, "", 0, 1, "", "Enable/disable to smoothing image for edge detection"));

	//m_ParametersTeach.Add(pParm)
	m_ParametersTeach.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "Datum/Pattern Search";
	pParm->Add(new CParmInfo(&pData->bDattamSearchEnable, "Enable", TBOOL, "", 0, 1, "", "-Specifies pattern search enable/disable for pattern inspection.\n-Should choose unique pattern in the image to teach."));
	pParm->Add(new CParmInfo(&pData->nDattamSearchNoOfDattams, "Number Of Device Pattern", TINT, "", 2, 4, "", "Specifies number of dattam/pattern for device inspection."));
	pParm->Add(new CParmInfo(&pData->dDattamSearchAreaX, "Search Area Along X", TDOUBLE, "", 0, 6000, "Microns", "-Specifies pattern search area in X direction.\n-If this value is setup higher, Datum Search area will be expanded.\n-If this value is setup too high, Datum is searched so slowly. Otherwise, Datum is not found."));
	pParm->Add(new CParmInfo(&pData->dDattamSearchAreaY, "Search Area Along Y", TDOUBLE, "", 0, 6000, "Microns", "-Specifies pattern search area in Y direction.\n-If this value is setup higher, Datum Search area will be expanded.\n-If this value is setup too high, Datum is searched so slowly. Otherwise, Datum is not found."));
	pParm->Add(new CParmInfo(&pData->nDattamSearchMaxRotAngle, "Max. Rotation Angle", TINT, "", 0, 360, "degrees", "-Specifies maximum allowed device rotation angle.\n-If the angle of deviation between teach and inspect Datum less than Max. Rotation Angle, the Datum will be found.\n-If this value is setup too high, Datum is searched so slowly. Otherwise, Datum is not found."));
	pParm->Add(new CParmInfo(&pData->dDattamSearchMinAcceptanceScore, "Min. Acceptance Score", TDOUBLE, "", 0, 100, "%", "-Specifies minimum acceptance for pattern matching.\n-If accuracy of similarity between teach and inspect unique pattern less than Min. Acceptance Score value, the Datum will be found.\n-If this value is setup too high, Datum is not found. Otherwise, Datum is searched slowly."));
	
	//m_ParametersTeach.Add(pParm);
	m_ParametersTeach.SetAt(nParamIndex++, pParm);

	/*pParm = new CParameters();
	pParm->strInspection = "Region Search";
	pParm->Add(new CParmInfo(&pData->bRegionSearchEnable, "Enable", TBOOL, "", 0, 1, "", "Specifies region search enable/disable for device inspection"));
	pParm->Add(new CParmInfo(&pData->nRegionSearchNoOfRegions, "Number of Regions", TINT, "", 0, 10, "quantity", "Specifies number of regions for device inspection"));
	m_ParametersTeach.Add(pParm);*/

	/*pParm = new CParameters();
	pParm->strInspection = "PVI Inspection Setup";
	pParm->Add(new CParmInfo(&pData->bRegionSearchEnable, "Enable", TBOOL, "", 0, 1, "", "Specifies region search enable/disable for device inspection"));
	pParm->Add(new CParmInfo(&pData->nRegionSearchNoOfRegions, "Number of Regions", TINT, "", 0, 10, "quantity", "Specifies number of regions for device inspection"));
	m_ParametersTeach.Add(pParm);*/

	pParm = new CParameters();
	pParm->strInspection = "Encap";
	pParm->Add(new CParmInfo(&pData->bEnableEncap, "Enable", TBOOL, "", 0, 1, "", "Enable Encap Teach"));
	pParm->Add(new CParmInfo(&pData->nEncapMinContrast, "Min. Intensity", TINT, "", 0, 255, "Levels", "-Minimum Intensity of Encap.\n-If the Gray Level value of Encap is between Min. Intensity and Max. Intensity value, the Encap Location will be found.\n-If this value is increased too high, Encap Location will be not found. Otherwise, Encap Location will be found in mistake."));
	pParm->Add(new CParmInfo(&pData->nEncapMaxContrast, "Max. Intensity", TINT, "", 0, 255, "Levels", "-Maximum Intensity of Encap.\n-If the Gray Level value of Encap is between Min. Intensity and Max. Intensity value, the Encap Location will be found.\n-If this value is increased too high, Encap Location will be found in mistake. Otherwise, Encap Location will be not found."));
	pParm->Add(new CParmInfo(&pData->m_nRectWidth[1], "Excess Min Width", TINT, "", 0, 20000, "Microns", "-Box Width for Excess Encap Inspection.\n-No need to setup."));
	pParm->Add(new CParmInfo(&pData->m_nRectHeight[1], "Excess Min Height", TINT, "", 0, 20000, "Microns", "-Box Height for Excess Encap Inspection.\n-No need to setup."));
	pParm->Add(new CParmInfo(&pData->m_nRectWidth[2], "Insufficient Max Width", TINT, "", 0, 20000, "Microns", "Box Width for Insufficient Encap Inspection.\n-No need to setup."));
	pParm->Add(new CParmInfo(&pData->m_nRectHeight[2], "Insufficient Max Height", TINT, "", 0, 20000, "Microns", "Box Height for Insufficient Encap Inspection.\n-No need to setup."));
	pParm->Add(new CParmInfo(&pData->nEncapTracingMaskNumber, "Tracing Mask Number", TINT, "", 0, 10, "", "Number of Area to be Masked For Encap Tracing"));
	for (int nMask = 0; nMask<10; nMask++) {
		CString strTemp, strTemp2;
		strTemp.Format("Tracing Mask Type %d", nMask + 1);
		strTemp2.Format("Polygon Points at Mask %d", nMask + 1);
		pParm->Add(new CParmInfo(&pData->nEncapTracingMaskType[nMask], strTemp, TENUM, strTemp, 0, 2, "", "0-Rectangle  1-Polygon 2-Ellipse",&strArrayShapeType));
		pParm->Add(new CParmInfo(&pData->nEncapTracingPolygonPointNumber[nMask], strTemp2, TINT, strTemp, 0, 20, "Points", "Number of " + strTemp2));
	}
	pParm->Add(new CParmInfo(&pData->nEncapSurfaceMaskNumber, "Surface Mask Number", TINT, "", 0, 10, "", "Number of Area to be Masked For Encap Surface Inspection"));


	//m_ParametersTeach.Add(pParm); 
	m_ParametersTeach.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "CoverLayer";
	pParm->Add(new CParmInfo(&pData->bEnableCoverLayer, "Enable", TBOOL, "", 0, 1, "", "-Enable Cover Layer Teach"));
	pParm->Add(new CParmInfo(&pData->nCLInspRegion, "No. of Cover Layer ", TINT, "", 0, 5, "", "-Number of Region for Cover Layer Inspection"));
	pParm->Add(new CParmInfo(&pData->bEnableCLDatum, "Enable Datum Search", TBOOL, "", 0, 1, "", "-Enable Datum as Reference for Cover Layer Region.\n-If Enable Datum Search is 'true', the Cover Layer Region will be placed more exactly location."));
	pParm->Add(new CParmInfo(&pData->nCLInspRegion, "No. of Datum", TINT, "", 0, 5, "", "-Number of Datum for Cover Layer Region Search"));
	pParm->Add(new CParmInfo(&pData->nCLMaxRotationAngle, "Max. Rotation Angle", TINT, "", 0, 360, "degree", "-Maximum Angle of Rotation.\n-If the angle of deviation between teach and inspect Datum less than Max. Rotation Angle, the Datum will be found.\n-If this value is setup too high, Datum is searched so slowly. Otherwise, Datum is not found."));
	pParm->Add(new CParmInfo(&pData->nCLPatternSearchX, "Search Area in X", TINT, "", 0, 6000, "Microns", "-Datum Search Area Along X Direction.\n-If this value is setup higher, Datum Search area will be expanded.\n-If this value is setup too high, Datum is searched so slowly. Otherwise, Datum is not found."));
	pParm->Add(new CParmInfo(&pData->nCLPatternSearchY, "Search Area in Y", TINT, "", 0, 6000, "Microns", "-Datum Search Area Along Y Direction.\n-If this value is setup higher, Datum Search area will be expanded.\n-If this value is setup too high, Datum is searched so slowly. Otherwise, Datum is not found."));
	pParm->Add(new CParmInfo(&pData->nCLMinAcceptanceScore, "Min. Acceptance Score", TINT, "", 0, 100, "%", "-Minimum Datum Search Score.\n-If accuracy of similarity between teach and inspect unique pattern less than Min. Acceptance Score value, the Datum will be found.\n-If this value is setup too high, Datum is not found. Otherwise, Datum is searched slowly."));
	
	//m_ParametersTeach.Add(pParm);
	m_ParametersTeach.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "CoverLayer Alignment";
	pParm->Add(new CParmInfo(&pData->bEnableCLAlignmentPointer, "Enable", TBOOL, "Pointer", 0, 1, "", "-Enable Cover Layer Alignment Teach Using Pointer"));
	pParm->Add(new CParmInfo(&pData->nPointer, "No. of Pointer ", TINT, "Pointer", 0, 6, "", "-Number of Pointer for Cover Layer Alignment"));
	pParm->Add(new CParmInfo(&pData->nCLAP_MaxRotationAngle, "Max. Rotation Angle", TINT, "Pointer", 0, 360, "degree", "-Maximum Angle of Rotation.\n-If the angle of deviation between teach and inspect Pointer less than Max. Rotation Angle, the Pointer will be found.\n-If this value is setup too high, Pointer is searched so slowly. Otherwise, Pointer is not found."));
	pParm->Add(new CParmInfo(&pData->nCLAP_PatternSearchX, "Search Area in X", TINT, "Pointer", 0, 6000, "Microns", "-Pointer Search Area Along X Direction.\n-If this value is setup higher, Pointer Search area will be expanded.\n-If this value is setup too high, Pointer is searched so slowly. Otherwise, Pointer is not found."));
	pParm->Add(new CParmInfo(&pData->nCLAP_PatternSearchY, "Search Area in Y", TINT, "Pointer", 0, 6000, "Microns", "-Pointer Search Area Along Y Direction.\n-If this value is setup higher, Pointer Search area will be expanded.\n-If this value is setup too high, Pointer is searched so slowly. Otherwise, Pointer is not found."));
	pParm->Add(new CParmInfo(&pData->nCLAP_MinAcceptanceScore, "Min. Acceptance Score", TINT, "Pointer", 0, 100, "%", "-Minimum Pointer Search Score.\n-If accuracy of similarity between teach and inspect Pointer less than Min. Acceptance Score value, the Pointer will be found.\n-If this value is setup too high, Pointer is not found. Otherwise, Pointer is searched slowly."));
	pParm->Add(new CParmInfo(&pData->bEnableManualPointer, "Enable Manual Pointer", TBOOL, "Pointer", 0, 1, "", "-Enable To Use Manual Pointer.\n-If the image does not have unique pattern to choose the pointer, should setup 'true' Enable Manual Pointer.\n-Draw any rectangle on the Cover Layer when Manual Pointer is taught."));
	
	//m_ParametersTeach.Add(pParm);
	m_ParametersTeach.SetAt(nParamIndex++, pParm);

	pParm = new CParameters();
	pParm->strInspection = "Slot";
	pParm->Add(new CParmInfo(&pData->bEnableSlot, "Enable", TBOOL, "", 0, 1, "", "-Enable Teach Slot"));
	pParm->Add(new CParmInfo(&pData->nNumOfSlot, "No. of Slot ", TINT, "", 1, 5, "", "-Number of Slot"));
	pParm->Add(new CParmInfo(&pData->nMinIntensitySlot, "Min. Intensity", TINT, "", 0, 255, "Levels", "-Minimum Intensity of Slot"));
	pParm->Add(new CParmInfo(&pData->nContrastEdgeSlot, "Edge Contrast", TINT, "", 1, 255, "Levels", "Edge Contrast of Slot.\nUse to remove white blob around Slot Region.Set to 255 to ignore this feature"));
	pParm->Add(new CParmInfo(&pData->nSlotAreaType, "Slot Area Type", TENUM, "", 0, 2, "", "0-Rectangle  1-Polygon 2-Ellipse", &strArrayShapeType));
	pParm->Add(new CParmInfo(&pData->nSlotPolygonPointCount, "Slot Polygon Point Count", TINT, "", 0, 20, "Points", "Number of Polygon Point of Slot Region"));

	pParm->Add(new CParmInfo(&pData->nSlotLimitAreaType, "Slot Limit Type", TENUM, "", 0, 2, "", "0-Rectangle  1-Polygon 2-Ellipse", &strArrayShapeType));
	pParm->Add(new CParmInfo(&pData->nSlotLimitPolygonPointCount, "Slot Limit Polygon Point Count", TINT, "", 0, 20, "Points", "Number of Polygon Point of Slot Limit Region"));

	pParm->Add(new CParmInfo(&pData->nSlotMaskNumber, "Slot Mask Number", TINT, "", 0, 10, "", "Number of Area to be Masked For Slot"));
	for (int nMask = 0; nMask<10; nMask++) {
		CString strTemp, strTemp2;
		strTemp.Format("Slot Mask Type %d", nMask + 1);
		strTemp2.Format("Polygon Points at Mask %d", nMask + 1);
		pParm->Add(new CParmInfo(&pData->nSlotMaskType[nMask], strTemp, TENUM, strTemp, 0, 2, "", "0-Rectangle  1-Polygon 2-Ellipse", &strArrayShapeType));
		pParm->Add(new CParmInfo(&pData->nSlotMaskPolygonPointCount[nMask], strTemp2, TINT, strTemp, 0, 20, "Points", "Number of " + strTemp2));
	}
	//m_ParametersTeach.Add(pParm);
	m_ParametersTeach.SetAt(nParamIndex++, pParm);


	/////////////Test Encap Bottom Magnus

	pParm = new CParameters();
	pParm->strInspection = "Teach Encap Magnus";
	pParm->Add(new CParmInfo(&pData->bEnable_EncapManus, "Enable", TBOOL, "", 0, 1, "", "Enable Encap MagnusPam"));
	pParm->Add(new CParmInfo(&pData->nThreshMin_EncapManus, "Min Thresh", TINT, "", 0, 255, "", "Thresh"));
	pParm->Add(new CParmInfo(&pData->nThreshMax_EncapManus, "Max Thresh", TINT, "", 0, 255, "", "Thresh"));
//	pParm->Add(new CParmInfo(&pData->nArea_Object_EncapManus, "Area Object", TINT, "", 0, 99999999, "", "Area_Object"));
	pParm->Add(new CParmInfo(&pData->nHeight_Object_magnus, "Area Height ", TINT, "", 300, 10000, "", "Area_Height"));
	pParm->Add(new CParmInfo(&pData->nWidth_Object_magnus, "Area Width", TINT, "",300, 10000, "", "Area_Width"));

//	pParm->Add(new CParmInfo(&pData->nCropW_Expand_EncapManus, "Crop Expand Width", TINT, "", 0, 500, "", "Crop Expand"));
//	pParm->Add(new CParmInfo(&pData->nCropH_Expand_EncapManus, "Crop Expand Height", TINT, "", 0, 500, "", "Crop Expand"));

//	pParm->Add(new CParmInfo(&pData->nDilateX_EncapManus, "Dilate Kernel Width", TINT, "", 1, 50, "", "Dilate_Kernel"));
	pParm->Add(new CParmInfo(&pData->nDilateY_EncapManus, "Dilate Kernel Height", TINT, "", 1, 50, "", "Dilate_Kernel"));

	pParm->Add(new CParmInfo(&pData->nOpeningX_EncapManus, "Opening Kernel Width", TINT, "", 1, 300, "", "Opening_Kernel"));
//	pParm->Add(new CParmInfo(&pData->nOpeningY_EncapManus, "Opening Kernel Height", TINT, "", 1, 100, "", "Opening_Kernel"));
	pParm->Add(new CParmInfo(&pData->nValue_OpeningCircle_magnus, "Value Opening Encap", TINT, "", 1, 200, "", "Opening_Circle_Kernel"));
	pParm->Add(new CParmInfo(&pData->nValue_OpeningCircleCrop_magnus, "Value Opening Crop", TINT, "", 1, 200, "", "Opening_Circle_Kernel"));

	
//	pParm->Add(new CParmInfo(&pData->nCrop_Smooth_EncapManus, "Crop Height Smooth", TINT, "", 5, 200, "", "Crop_Height"));
	pParm->Add(new CParmInfo(&pData->nValue_Smooth_EncapMagnus, "Value Smooth", TINT, "", 1, 500, "", "Smooth_Value"));


//	pParm->Add(new CParmInfo(&pData->nThreshMin_Black_EncapManus, "Thresh Black Min", TINT, "", 0, 255, "", "Thresh_Black"));
	pParm->Add(new CParmInfo(&pData->nThreshMax_Black_EncapManus, "Thresh Black Region", TINT, "", 0, 255, "", "Thresh_Black"));
	pParm->Add(new CParmInfo(&pData->nThreshMin_White_EncapManus, "Thresh White Region", TINT, "", 0, 255, "", "Thresh_White"));
//	pParm->Add(new CParmInfo(&pData->nThreshMax_White_EncapManus, "Thesh White Max", TINT, "", 0, 255, "", "Thresh_White"));

	
	m_ParametersTeach.SetAt(nParamIndex++, pParm);


	/////////////// end 



	//
	//if (!m_ParametersTeachTemp.IsEmpty()) {
	//	for (int i = 0; i < m_ParametersTeachTemp.GetCount(); i++)
	//		delete m_ParametersTeachTemp[i];
	//	m_ParametersTeachTemp.RemoveAll();
	//}
}

void CInspectionHandler::CleanUp()
{
	CParameters *pParmsTemp;
	if(!m_Parameters.IsEmpty()){
		for (int i = 0; i < m_Parameters.GetCount(); i++)
		{
			pParmsTemp = m_Parameters.GetAt(i);
			/*for (int j = 0; j < pParmsTemp->arrParmInfo.GetCount(); j++) {
				delete pParmsTemp->arrParmInfo[j];
			}*/
			pParmsTemp->Clean();
			//delete pParmsTemp;
		}
		m_Parameters.RemoveAll();
	}
	
}

void CInspectionHandler::CleanUpTeach()
{
	CParameters *pParmsTemp;
	if(!m_ParametersTeach.IsEmpty()){
		for(int i=0;i<m_ParametersTeach.GetCount();i++)
		{
 			pParmsTemp = m_ParametersTeach.GetAt(i);
			/*for (int j = 0; j < pParmsTemp->arrParmInfo.GetCount(); j++) {
				delete pParmsTemp->arrParmInfo[j];
			}*/
			pParmsTemp->Clean();
			//delete pParmsTemp;
		}
		m_ParametersTeach.RemoveAll();
	}
}

void CInspectionHandler::CleanUpPVI()
{
	/*CParameters *pParmsTemp;
	if(!m_ParametersInspSetUP.IsEmpty()) {
		for (int i = 0; i < m_ParametersInspSetUP.GetCount(); i++) {
			pParmsTemp = m_ParametersInspSetUP.GetAt(i);
			for (int j = 0; j < pParmsTemp->arrParmInfo.GetCount(); j++) {
				delete pParmsTemp->arrParmInfo[j];
			}
			pParmsTemp->Clean();
			delete pParmsTemp;
		}
		m_ParametersInspSetUP.RemoveAll();
	}
	if (!m_ParametersEdgePviInspSetUP.IsEmpty()) {
		for (int i = 0; i < m_ParametersEdgePviInspSetUP.GetCount(); i++) {
			pParmsTemp = m_ParametersEdgePviInspSetUP.GetAt(i);
			for (int j = 0; j < pParmsTemp->arrParmInfo.GetCount(); j++) {
				delete pParmsTemp->arrParmInfo[j];
			}
			pParmsTemp->Clean();
			delete pParmsTemp;
		}
		m_ParametersEdgePviInspSetUP.RemoveAll();
	}

	if (!m_Inspection.m_pPviEdgeInspection.m_ParametersDefect.IsEmpty()) {
		for (int i = 0; i < m_Inspection.m_pPviEdgeInspection.m_ParametersDefect.GetCount(); i++) {
			pParmsTemp = m_Inspection.m_pPviEdgeInspection.m_ParametersDefect.GetAt(i);
			for (int i = 0; i < pParmsTemp->arrParmInfo.GetCount(); i++) {
				delete pParmsTemp->arrParmInfo[i];
			}
			pParmsTemp->Clean();
			delete pParmsTemp;
		}
		
		m_Inspection.m_pPviEdgeInspection.m_ParametersDefect.RemoveAll();
	}
	for (int i = 0; i < 10; i++) {
		if (!m_Inspection.m_pPviInspectionSetup[i].m_ParametersDefect.IsEmpty()) {
			for (int k = 0; k < m_Inspection.m_pPviInspectionSetup[i].m_ParametersDefect.GetCount(); k++) {
				pParmsTemp = m_Inspection.m_pPviInspectionSetup[i].m_ParametersDefect.GetAt(k);
				for (int j = 0; j < pParmsTemp->arrParmInfo.GetCount(); j++) {
					delete pParmsTemp->arrParmInfo[j];
				}
				pParmsTemp->Clean();
				delete pParmsTemp;
			}
		}
		m_Inspection.m_pPviInspectionSetup[i].m_ParametersDefect.RemoveAll();
	}*/
}

int CInspectionHandler::SaveInspectParameters(CString strDirectoryName,CString strFileName, int nFOV)
{
	CIniFile iniFile;
	CString strDataFileName;

	//strDataFileName = strDirectoryName + "\\" + strFileName + ".cll";
	strDataFileName.Format("%s\\%s_%d.insp", strDirectoryName, strFileName, nFOV+1);
	iniFile.SetPath(strDataFileName);
	
	CString strSec, strVariable;
    strSec.Format("Inspection Data");
	CInspectinonData* pData = &m_Inspection.m_InspectionData;

	int nCount = (int)m_Parameters.GetSize();
	for(int i=0; i<nCount; i++)
		(m_Parameters.GetAt(i))->SaveParameters(&iniFile);

	iniFile.WriteFile();
	
	return 0;
}

int CInspectionHandler::LoadInspectParameters(CString strDirectoryName,CString strFileName, int nFOV)
{
	CIniFile iniFile;
	CString strDataFileName;

	//strDataFileName = strDirectoryName + "\\" + strFileName + ".cll";
	strDataFileName.Format("%s\\%s_%d.insp", strDirectoryName, strFileName, nFOV+1);
	iniFile.SetPath(strDataFileName);

	if(iniFile.ReadFile()) {
		CString strSec, strVariable;
		strSec.Format("Inspection Data");
		CInspectinonData* pData = &m_Inspection.m_InspectionData;

		int nCount = (int)m_Parameters.GetSize();
		for(int i=0; i<nCount; i++)
			(m_Parameters.GetAt(i))->LoadParameters(&iniFile);
	}
	/*else {
		OutputDebugLog("%s not found. Default values assumed", strFileName);
		SaveInspectParameters(strDirectoryName, strFileName);
	}*/
	
	return 0;
}

int CInspectionHandler::SaveTeachParameters(CString strDirectoryName,CString strFileName,int nFOV)
{
	CIniFile iniFile;
	CString strDataFileName;

	//strDataFileName = strDirectoryName + "\\" + strFileName + ".tch";
	strDataFileName.Format("%s\\%s_%d.tch", strDirectoryName, strFileName, nFOV+1);
	iniFile.SetPath(strDataFileName);
	SetTeachParameters(&iniFile);
	iniFile.WriteFile();

	return 0;
}

int CInspectionHandler::LoadTeachParameters(CString strDirectoryName,CString strFileName,int nFOV)
{
	CIniFile iniFile;
	CString strDataFileName;

	//strDataFileName = strDirectoryName + "\\" + strFileName + ".tch";
	strDataFileName.Format("%s\\%s_%d.tch", strDirectoryName, strFileName, nFOV+1);
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		GetTeachParameters(&iniFile);
	}
	else {
		// File Not Found
		/*OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		SetTeachParameters(&iniFile);
		iniFile.WriteFile();*/
		return -1;
	}

	return 0;
}

int CInspectionHandler::GetTeachParameters(CIniFile *ptrIniFile)
{
	CString strSec, strVariable;
    strSec.Format("Training Data");
	
	CTrainingData* pData = &m_Inspection.m_TrainingData;

	//pData->nCuvetteRadiusRa = ptrIniFile->GetInteger(strSec, "Cuvette Radius_Ra", pData->nCuvetteRadiusRa);
	//pData->nCuvetteRadiusRb = ptrIniFile->GetInteger(strSec, "Cuvette Radius_Rb", pData->nCuvetteRadiusRb);
	//pData->nLensRadius      = ptrIniFile->GetInteger(strSec, "Lens Radius",       pData->nLensRadius);

	int nCount = (int)m_ParametersTeach.GetSize();
	for(int i=0; i<nCount; i++)
		(m_ParametersTeach.GetAt(i))->LoadParameters(ptrIniFile);

// Device Loc //
	strVariable.Format("TeachDeviceLoc Rect");
	pData->m_rectDeviceLoc = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectDeviceLoc);
	if (pData->m_rectDeviceLoc.Width() < 5) 
		pData->m_rectDeviceLoc = CRect(100, 100, 400, 400);
	
	// Device Horizontal Line Loc //
	strVariable.Format("TeachDeviceHorizontalLineLoc Rect");
	pData->m_rectDeviceHorizontalLineLoc = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectDeviceHorizontalLineLoc);
	if (pData->m_rectDeviceHorizontalLineLoc.Width() < 5)
		pData->m_rectDeviceHorizontalLineLoc = CRect(100, 100, 400, 400);

	// Device Vertical Line Loc //
	strVariable.Format("TeachDeviceVerticalLineLoc Rect");
	pData->m_rectDeviceVerticalLineLoc = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectDeviceVerticalLineLoc);
	if (pData->m_rectDeviceVerticalLineLoc.Width() < 5)
		pData->m_rectDeviceVerticalLineLoc = CRect(100, 100, 400, 400);

// Dattam/Pattern Search //
	for(int nRectDattam=0; nRectDattam<4; nRectDattam++) {
		strVariable.Format("TeachDattam/PatternSearch Rect%d", nRectDattam+1);
		pData->m_rectDattamSearch[nRectDattam] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectDattamSearch[nRectDattam]);
	}

	//Auto Focusing Sequence ROI
	strVariable.Format("TeachAutoFocusingSequence Rect");
	CRect rectAutoFocusRoiDefault = CRect(2048 / 2 - 250, 2048 / 2 - 250, 2048 / 2 + 250, 2048 / 2 + 250);
	pData->m_rectAutoFocusROI = ptrIniFile->GetCRect(strSec, strVariable, rectAutoFocusRoiDefault);
	if (pData->m_rectAutoFocusROI.Width() < 5 || pData->m_rectAutoFocusROI.Height() < 5)
		pData->m_rectAutoFocusROI = rectAutoFocusRoiDefault;

//

// encap region //
	for (int k = 0; k < 3; k++) {
		strVariable.Format("TeachPVIEncap  Rect%d", k + 1);
		pData->m_rectEncap[k] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectEncap[k]);
		strVariable.Format("TeachPVIEncap  Width%d", k + 1);
		pData->m_nRectWidth[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->m_nRectWidth[k]);
		strVariable.Format("TeachPVIEncap  Height%d", k + 1);
		pData->m_nRectHeight[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->m_nRectHeight[k]);
	}

	for (int k = 0; k < 10; k++) {
		strVariable.Format("TeachPVIEncap  Tracing Mask Rect%d", k + 1);
		pData->m_rectMaskEncapTracing[k] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectMaskEncapTracing[k]);
		//Encap Tracing Polygon Type
		strVariable.Format("TeachPVIEncap Polygon Point Count%d", k + 1);
		pData->nEncapTracingPolygonPointNumber[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nEncapTracingPolygonPointNumber[k]);
		if (pData->nEncapTracingPolygonPointNumber[k] > 20)
			pData->nEncapTracingPolygonPointNumber[k] = 20;
		else if (pData->nEncapTracingPolygonPointNumber[k] < 3)
			pData->nEncapTracingPolygonPointNumber[k] = 3;

		for (int j = 0; j < pData->nEncapTracingPolygonPointNumber[k]; j++) {
			strVariable.Format("TeachPVIEncap PolygonPos%dPoint%d", k + 1, j + 1);
			pData->m_EncapTracingPolygonPointArea[k][j]=ptrIniFile->GetCPoint(strSec, strVariable, pData->m_EncapTracingPolygonPointArea[k][j]);
		}
		strVariable.Format("TeachPVIEncap  Surface Inspection Mask Rect%d", k + 1);
		pData->m_rectMaskEncapSurface[k] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectMaskEncapSurface[k]);
	}

	// Encap bottom Rectangle Magnus
	//strVariable.Format("Teach Rectangle DeviceLocation Encap Magnus Rect", 1);
	//pData->hRect_DeviceLocationEncap_magnus = ptrIniFile->GetCRect(strSec, strVariable, pData->hRect_DeviceLocationEncap_magnus);
//	strVariable.Format("Teach Rectangle EncapLocation Encap Magnus Rect", 1);
//	pData->hRect_EncapLocation_magnus = ptrIniFile->GetCRect(strSec, strVariable, pData->hRect_EncapLocation_magnus);
//	strVariable.Format("Teach Rectangle CropSmoothEncap Encap Magnus Rect", 2);
//	pData->hRect_CropSmoothEncap_magnus = ptrIniFile->GetCRect(strSec, strVariable, pData->hRect_CropSmoothEncap_magnus);
//	strVariable.Format("Teach Rectangle CropNoSmoothEncap Encap Magnus Rect", 3);
//	pData->hRect_CropNoSmoothEncap_magnus = ptrIniFile->GetCRect(strSec, strVariable, pData->hRect_CropNoSmoothEncap_magnus);
	strVariable.Format("Teach Area device location Magnus Int");
	pData->nArea_Object_EncapManus = ptrIniFile->GetInteger(strSec, strVariable, pData->nArea_Object_EncapManus);
	for (int k = 0; k <= 1; k++)

	{
		strVariable.Format("Teach Crop Expand Height Encap Magnus %d Int", k);
		pData->nCrop_ExpandHeight_magnus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_ExpandHeight_magnus[k]);

		strVariable.Format("Teach Crop Expand Left Encap Magnus %d Int", k);
		pData->nCrop_ExpandLeft_magnus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_ExpandLeft_magnus[k]);

		strVariable.Format("Teach Crop Expand Right Encap Magnus %d Int", k);
		pData->nCrop_ExpandRight_magnus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_ExpandRight_magnus[k]);

		strVariable.Format("Teach Crop Expand Smooth Encap Magnus %d Int", k);
		pData->nCrop_Smooth_EncapManus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_Smooth_EncapManus[k]);
	
		strVariable.Format("Teach Crop Remove Blackline Height Encap Magnus %d Int", k);
		pData->nCrop_RemoveBLHeight_magnus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_RemoveBLHeight_magnus[k]);

		strVariable.Format("Teach Crop Remove Blackline Right Encap Magnus %d Int", k);
		pData->nCrop_RemoveBLRight_magnus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_RemoveBLRight_magnus[k]);

		strVariable.Format("Teach Crop Remove Blackline Left Encap Magnus %d Int", k);
		pData->nCrop_RemoveBLLeft_magnus[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nCrop_RemoveBLLeft_magnus[k]);

	}
// cover layer //
	int nDatum = m_Inspection.m_TrainingData.nCLDatum;
	for (int n = 0; n < nDatum; n++) {
		strVariable.Format("TeachCoverLayer Datum Rect%d", n + 1);
		pData->m_rectCLDatum[n] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectCLDatum[n]);
	}

	int nCoverLayerRegion = m_Inspection.m_TrainingData.nCLInspRegion;
	for (int n = 0; n < nCoverLayerRegion; n++) {
		strVariable.Format("TeachCoverLayer Rect%d", n + 1);
		pData->m_rectCoverLayer[n] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectCoverLayer[n]);
	}

	int nPointer = m_Inspection.m_TrainingData.nPointer;
	for (int n = 0; n < nPointer; n++) {
		strVariable.Format("TeachCoverLayer Pointer Rect%d", n + 1);
		pData->m_rectPointer[n] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectPointer[n]);
	}
//Slot
	//Slot Polygon Point
	strVariable.Format("TeachSlot Polygon Point Count");
	pData->nSlotPolygonPointCount = ptrIniFile->GetInteger(strSec, strVariable, 4);
	if (pData->nSlotPolygonPointCount > 20)
		pData->nSlotPolygonPointCount = 20;
	else if (pData->nSlotPolygonPointCount < 3)
		pData->nSlotPolygonPointCount = 3;

	strVariable.Format("TeachSlotLimit Polygon Point Count");
	pData->nSlotLimitPolygonPointCount = ptrIniFile->GetInteger(strSec, strVariable, 4);
	if (pData->nSlotLimitPolygonPointCount > 20)
		pData->nSlotLimitPolygonPointCount = 20;
	else if (pData->nSlotLimitPolygonPointCount < 3)
		pData->nSlotLimitPolygonPointCount = 3;

	strVariable.Format("TeachSlot ROI Type");
	int nROIType = ptrIniFile->GetInteger(strSec, strVariable, RECTANGLE);
	if(nROIType == RECTANGLE)
		pData->nSlotAreaType = RECTANGLE;
	else
		pData->nSlotAreaType = POLYGON;

	strVariable.Format("TeachSlotLimit ROI Type");
	nROIType = ptrIniFile->GetInteger(strSec, strVariable, RECTANGLE);
	if (nROIType == RECTANGLE)
		pData->nSlotLimitAreaType = RECTANGLE;
	else
		pData->nSlotLimitAreaType = POLYGON;

	for (int k = 0; k < 5; k++) {
		strVariable.Format("TeachSlot  Rect%d", k + 1);
		pData->m_rectSlot[k] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectSlot[k]);
		strVariable.Format("TeachSlotLimit  Rect%d", k + 1);
		pData->m_rectSlotLimit[k] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectSlotLimit[k]);

		//Slot Teach REGION
		for (int j = 0; j < pData->nSlotPolygonPointCount; j++) {
			strVariable.Format("TeachSlot Area%d PolygonPoint%d", k + 1, j + 1);
			pData->m_SlotPolygonPoint[k][j] = ptrIniFile->GetCPoint(strSec, strVariable, pData->m_SlotPolygonPoint[k][j]);
		}
		for (int j = 0; j < pData->nSlotLimitPolygonPointCount; j++) {
			strVariable.Format("TeachSlotLimit Area%d PolygonPoint%d", k + 1, j + 1);
			pData->m_SlotLimitPolygonPoint[k][j] = ptrIniFile->GetCPoint(strSec, strVariable, pData->m_SlotLimitPolygonPoint[k][j]);
		}
	}

	for (int k = 0; k < 10; k++) {
		strVariable.Format("TeachPVISlot  Surface Mask Rect%d", k + 1);
		pData->m_rectMaskSlot[k] = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectMaskSlot[k]);
		//Slot Surface Polygon Type
		strVariable.Format("TeachPVISlot Polygon Point Count%d", k + 1);
		pData->nSlotMaskPolygonPointCount[k] = ptrIniFile->GetInteger(strSec, strVariable, pData->nSlotMaskPolygonPointCount[k]);
		if (pData->nSlotMaskPolygonPointCount[k] > 20)
			pData->nSlotMaskPolygonPointCount[k] = 20;
		else if (pData->nSlotMaskPolygonPointCount[k] < 3)
			pData->nSlotMaskPolygonPointCount[k] = 3;

		for (int j = 0; j < pData->nSlotMaskPolygonPointCount[k]; j++) {
			strVariable.Format("TeachPVISlot PolygonPos%dPoint%d", k + 1, j + 1);
			pData->m_SlotMaskPolygonPoint[k][j] = ptrIniFile->GetCPoint(strSec, strVariable, pData->m_SlotMaskPolygonPoint[k][j]);
		}
	}
// Tilt Teach //
	strVariable.Format("TeachTilt Rect");
	pData->m_rectTilt = ptrIniFile->GetCRect(strSec, strVariable, pData->m_rectTilt);
	/*pData->m_nXShift = ptrIniFile->GetInteger(strSec, "Tilt Region X Shift", pData->m_nXShift);
	pData->m_nYShift = ptrIniFile->GetInteger(strSec, "Tilt Region Y Shift", pData->m_nYShift);
*/
	int nLen1, nLen2;
	double dTempValue;
	CString strTemp;

	//// start PVI - 0; end PVI - 31;
	//// start Encap - 32; end Encap - 46;
	//// start CL - 47; end CL - 59;
	//// total - 60

	nLen1 = nLen2 = 0;
	CString str[] = { "PVIArea",
		"PVIAngle", "PVI2DCenter",
		"PVISize",
		"MaskNumber", "Mask2DCenterPoints",
		"MaskAngleConcat", "MaskSize",
		"SlotNumber", "Slot2DCenterPoints",
		"SlotAngleConcat",
		"SlotSize", "PVIReserve5",
		"PVIReserve6", "PVIReserve7",
		"PVIReserve8", "PVIReserve9",
		"PVIReserve10", "PVIReserve11",
		"PVIReserve12", "PVIReserve13",
		"PVIReserve14", "PVIReserve15",
		"PVIReserve16", "PVIReserve17",
		"PVIReserve18", "PVIReserve19",
		"PVIReserve20", "PVIReserve21",
		"PVIReserve22", "PVIReserve23", "PVIReserve24",
		"MinEncapContrast", "MaxEncapContrast",
		"EncapReserve1", "EncapReserve2",
		"EncapReserve3", "EncapReserve4",
		"EncapReserve5", "EncapReserve6",
		"EncapReserve7", "EncapReserve8",
		"EncapReserve9", "EncapReserve10",
		"EncapReserve11", "EncapReserve12",
		"EncapReserve13",
		"CLMaxRotAngle",
		"CLPatternSearchX", "CLPatternSearchY",
		"CLMinAccScore",
		"CLDatumRow", "CLDatumCol",
		"CLModelID",
		"CLUpperCloseDist", "CLUpperFarDist",
		"CLLowerCloseDist", "CLLowerFarDist",
		"CLSideCloseDist", "CLSideFarDist" };

	for(int i=0; i</*pData->hvec_TupleVectorPVI.GetCount()*/32; i++) {
		//nLen1 = 0;
		//strTemp.Format("PVI Teach %s Count", str[i]);
		//nLen1 = ptrIniFile->GetInteger(strSec, strTemp, nLen1);
		for(int j=0; j</*nLen1*/10; j++) {
			nLen2 = 0;
			strTemp.Format("PVI Teach %s[%d] Count", str[i], j+1);
			nLen2 = ptrIniFile->GetInteger(strSec, strTemp, nLen2);
			for(int k=0; k<nLen2; k++) {
				dTempValue = 0;
				strTemp.Format("PVI Teach %s[%d][%d]", str[i], j+1, k+1);
				dTempValue = ptrIniFile->GetDouble(strSec, strTemp, dTempValue);
				pData->darrayPVI[i][j].Add(dTempValue);
				//OutputDebugLogTo(9, TRUE, "Get PVI %s = %.2f", strTemp, dTempValue);//// for testing
			}	
		}
	}

	for (int i = 32; i<47; i++) {
		nLen1 = 0;
		strTemp.Format("PVI Teach Encap %s Count", str[i]);
		nLen1 = ptrIniFile->GetInteger(strSec, strTemp, nLen1);
		for (int j = 0; j < nLen1; j++) {
			strTemp.Format("PVI Teach Encap %s[%d]", str[i], j + 1);
			dTempValue = ptrIniFile->GetDouble(strSec, strTemp, dTempValue);
			pData->darrayEncap[i-32].Add(dTempValue);
			//OutputDebugLogTo(9, TRUE, "Get Encap %s = %.2f", strTemp, dTempValue);//// for testing
		}
	}

	//for (int i = 47; i<60; i++) {
	//	nLen1 = 0;
	//	strTemp.Format("PVI Teach CoverLayer %s Count", str[i]);
	//	nLen1 = ptrIniFile->GetInteger(strSec, strTemp, nLen1);
	//	for (int j = 0; j < nLen1; j++) {
	//		strTemp.Format("PVI Teach CoverLayer %s[%d]", str[i], j + 1);
	//		dTempValue = ptrIniFile->GetDouble(strSec, strTemp, dTempValue);
	//		pData->darrayCoverLayer[i - 47].Add(dTempValue);
	//		//OutputDebugLogTo(9, TRUE, "Get CoverLayer %s = %.2f", strTemp, dTempValue);//// for testing
	//	}
	//}
	return 0;
}

int CInspectionHandler::SetTeachParameters(CIniFile *ptrIniFile)
{
	CString strSec, strVariable;
    strSec.Format("Training Data");

	CTrainingData* pData = &m_Inspection.m_TrainingData;

	//ptrIniFile->SetInteger(strSec, "Cuvette Radius_Ra", pData->nCuvetteRadiusRa);
	//ptrIniFile->SetInteger(strSec, "Cuvette Radius_Rb", pData->nCuvetteRadiusRb);
	//ptrIniFile->SetInteger(strSec, "Lens Radius",       pData->nLensRadius);

	int nCount = (int)m_ParametersTeach.GetSize();
	for(int i=0; i<nCount; i++)
		(m_ParametersTeach.GetAt(i))->SaveParameters(ptrIniFile);

// Device Loc //
	strVariable.Format("TeachDeviceLoc Rect");
	ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectDeviceLoc);

	// Device Horizontal LineLoc //
	strVariable.Format("TeachDeviceHorizontalLineLoc Rect");
	ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectDeviceHorizontalLineLoc);

	// Device Vertical Line Loc //
	strVariable.Format("TeachDeviceVerticalLineLoc Rect");
	ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectDeviceVerticalLineLoc);

// Dattam/Pattern Search //
	for(int nRectDattam=0; nRectDattam<4; nRectDattam++){
		strVariable.Format("TeachDattam/PatternSearch Rect%d", nRectDattam+1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectDattamSearch[nRectDattam]);
	}

//Auto Focusing Sequence ROI
	strVariable.Format("TeachAutoFocusingSequence Rect");
	ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectAutoFocusROI);
	
// encap region //
	for (int k = 0; k < 3; k++) {
		strVariable.Format("TeachPVIEncap  Rect%d", k + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectEncap[k]);
		strVariable.Format("TeachPVIEncap  Width%d", k + 1);
		ptrIniFile->SetInteger(strSec, strVariable, pData->m_nRectWidth[k]);
		strVariable.Format("TeachPVIEncap  Height%d", k + 1);
		ptrIniFile->SetInteger(strSec, strVariable, pData->m_nRectHeight[k]);

	}


	for (int k = 0; k < 10; k++) {
		strVariable.Format("TeachPVIEncap  Tracing Mask Rect%d", k + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectMaskEncapTracing[k]);
		strVariable.Format("TeachPVIEncap Polygon Point Count%d", k + 1);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nEncapTracingPolygonPointNumber[k]);
		for (int j = 0; j < pData->nEncapTracingPolygonPointNumber[k]; j++) {
			strVariable.Format("TeachPVIEncap PolygonPos%dPoint%d", k + 1, j + 1);
			ptrIniFile->SetCPoint(strSec, strVariable, pData->m_EncapTracingPolygonPointArea[k][j]);
		}




		strVariable.Format("TeachPVIEncap  Surface Inspection Mask Rect%d", k + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectMaskEncapSurface[k]);
	}

// Encap bottom Rectangle Magnus
	//strVariable.Format("Teach Rectangle DeviceLocation Encap Magnus Rect", 1);
	//ptrIniFile->SetCRect(strSec, strVariable, pData->hRect_DeviceLocationEncap_magnus);
//	strVariable.Format("Teach Rectangle EncapLocation Encap Magnus Rect");
//	ptrIniFile->SetCRect(strSec, strVariable, pData->hRect_EncapLocation_magnus);
//	strVariable.Format("Teach Rectangle CropSmoothEncap Encap Magnus Rect");
//	ptrIniFile->SetCRect(strSec, strVariable, pData->hRect_CropSmoothEncap_magnus);
//	strVariable.Format("Teach Rectangle CropNoSmoothEncap Encap Magnus Rect");
//	ptrIniFile->SetCRect(strSec, strVariable, pData->hRect_CropNoSmoothEncap_magnus);

	strVariable.Format("Teach Area device location Magnus Int");
	ptrIniFile->SetInteger(strSec, strVariable, pData->nArea_Object_EncapManus);
	for (int k = 0; k <= 1; k++)

	{
		strVariable.Format("Teach Crop Expand Height Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_ExpandHeight_magnus[k]);

		strVariable.Format("Teach Crop Expand Left Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_ExpandLeft_magnus[k]);

		strVariable.Format("Teach Crop Expand Right Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_ExpandRight_magnus[k]);

		strVariable.Format("Teach Crop Expand Smooth Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_Smooth_EncapManus[k]);

		strVariable.Format("Teach Crop Remove Blackline Height Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_RemoveBLHeight_magnus[k]);

		strVariable.Format("Teach Crop Remove Blackline Right Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_RemoveBLRight_magnus[k]);

		strVariable.Format("Teach Crop Remove Blackline Left Encap Magnus %d Int", k);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nCrop_RemoveBLLeft_magnus[k]);


	}

// cover layer //
	int nDatum = m_Inspection.m_TrainingData.nCLDatum;
	for (int n = 0; n < nDatum; n++) {
		strVariable.Format("TeachCoverLayer Datum Rect%d", n + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectCLDatum[n]);
	}

	int nCoverLayerRegion = m_Inspection.m_TrainingData.nCLInspRegion;
	for (int n = 0; n < nCoverLayerRegion; n++) {
		strVariable.Format("TeachCoverLayer Rect%d", n + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectCoverLayer[n]);
	}

	int nPointer = m_Inspection.m_TrainingData.nPointer;
	for (int n = 0; n < nPointer; n++) {
		strVariable.Format("TeachCoverLayer Pointer Rect%d", n + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectPointer[n]);
	}
//Slot
	//Slot Polygon
	strVariable.Format("TeachSlot Polygon Point Count");
	ptrIniFile->SetInteger(strSec, strVariable, pData->nSlotPolygonPointCount);
	strVariable.Format("TeachSlotLimit Polygon Point Count");
	ptrIniFile->SetInteger(strSec, strVariable, pData->nSlotLimitPolygonPointCount);

	strVariable.Format("TeachSlot ROI Type");
	ptrIniFile->SetInteger(strSec, strVariable, pData->nSlotAreaType);
	strVariable.Format("TeachSlotLimit ROI Type");
	ptrIniFile->SetInteger(strSec, strVariable, pData->nSlotLimitAreaType);

	for (int k = 0; k < 5; k++) {
		strVariable.Format("TeachSlot  Rect%d", k + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectSlot[k]);
		strVariable.Format("TeachSlotLimit  Rect%d", k + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectSlotLimit[k]);

		//Slot Polygon
		for (int j = 0; j < pData->nSlotPolygonPointCount; j++) {
			strVariable.Format("TeachSlot Area%d PolygonPoint%d", k + 1, j + 1);
			ptrIniFile->SetCPoint(strSec, strVariable, pData->m_SlotPolygonPoint[k][j]);
		}
		//Slot Limit Polygon
		for (int j = 0; j < pData->nSlotLimitPolygonPointCount; j++) {
			strVariable.Format("TeachSlotLimit Area%d PolygonPoint%d", k + 1, j + 1);
			ptrIniFile->SetCPoint(strSec, strVariable, pData->m_SlotLimitPolygonPoint[k][j]);
		}
	}
	for (int k = 0; k < 10; k++) {
		strVariable.Format("TeachPVISlot  Tracing Mask Rect%d", k + 1);
		ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectMaskSlot[k]);
		strVariable.Format("TeachPVISlot Polygon Point Count%d", k + 1);
		ptrIniFile->SetInteger(strSec, strVariable, pData->nSlotMaskPolygonPointCount[k]);
		for (int j = 0; j < pData->nSlotMaskPolygonPointCount[k]; j++) {
			strVariable.Format("TeachPVISlot PolygonPos%dPoint%d", k + 1, j + 1);
			ptrIniFile->SetCPoint(strSec, strVariable, pData->m_SlotMaskPolygonPoint[k][j]);
		}
	}
// Tilt Teach //
	strVariable.Format("TeachTilt Rect");
	ptrIniFile->SetCRect(strSec, strVariable, pData->m_rectTilt);
	/*ptrIniFile->SetInteger(strSec, "Tilt Region X Shift", pData->m_nXShift);
	ptrIniFile->SetInteger(strSec, "Tilt Region Y Shift", pData->m_nYShift);*/

	int nLen1, nLen2;
	double dTempValue;
	CString strTemp;

	//// start PVI - 0; end PVI - 31;
	//// start Encap - 32; end Encap - 46;
	//// start CL - 47; end CL - 59;
	//// total - 60

	nLen1 = nLen2 = 0;
	CString str[] = { "PVIArea",
					"PVIAngle", "PVI2DCenter",
					"PVISize",
					"MaskNumber", "Mask2DCenterPoints",
					"MaskAngleConcat", "MaskSize",
					"SlotNumber", "Slot2DCenterPoints",
					"SlotAngleConcat",
					"SlotSize", "PVIReserve5",
					"PVIReserve6", "PVIReserve7",
					"PVIReserve8", "PVIReserve9",
					"PVIReserve10", "PVIReserve11",
					"PVIReserve12", "PVIReserve13",
					"PVIReserve14", "PVIReserve15",
					"PVIReserve16", "PVIReserve17",
					"PVIReserve18", "PVIReserve19",
					"PVIReserve20", "PVIReserve21",
					"PVIReserve22", "PVIReserve23", "PVIReserve24",
					"MinEncapContrast", "MaxEncapContrast",
					"EncapReserve1", "EncapReserve2",
					"EncapReserve3", "EncapReserve4",
					"EncapReserve5", "EncapReserve6",
					"EncapReserve7", "EncapReserve8",
					"EncapReserve9", "EncapReserve10",
					"EncapReserve11", "EncapReserve12",
					"EncapReserve13",
					"CLMaxRotAngle",
					"CLPatternSearchX", "CLPatternSearchY",
					"CLMinAccScore",
					"CLDatumRow", "CLDatumCol",
					"CLModelID",
					"CLUpperCloseDist", "CLUpperFarDist",
					"CLLowerCloseDist", "CLLowerFarDist",
					"CLSideCloseDist", "CLSideFarDist" };

	for(int i=0; i</*pData->hvec_TupleVectorPVI.GetCount()*/32; i++) {
		//nLen1 = (int)pData->darrayPVI[i]->GetCount();
		//strTemp.Format("PVI Teach %s Count", str[i]);
		//ptrIniFile->SetInteger(strSec, strTemp, nLen1);
		for(int j=0; j</*nLen1*/10; j++) {
			nLen2 = (int)pData->darrayPVI[i][j].GetCount();
			strTemp.Format("PVI Teach %s[%d] Count", str[i], j+1);
			ptrIniFile->SetInteger(strSec, strTemp, nLen2);
			if(nLen2==0) {
				dTempValue = 0;
				strTemp.Format("PVI Teach %s[%d]", str[i], j+1);
				ptrIniFile->SetDouble(strSec, strTemp, dTempValue);
			}
			else {
				for(int k=0; k<nLen2; k++) {
					dTempValue = pData->darrayPVI[i][j][k];
					strTemp.Format("PVI Teach %s[%d][%d]", str[i], j+1, k+1);
					ptrIniFile->SetDouble(strSec, strTemp, dTempValue);
					//OutputDebugLogTo(9, TRUE, "Set PVI %s = %.2f", strTemp, dTempValue);//// for testing
				}
			}
		}
	}

	//for (int i = 32; i<47; i++) {
	//	nLen1 = (int)pData->darrayEncap[i-32].GetCount();
	//	strTemp.Format("PVI Teach Encap %s Count", str[i]);
	//	ptrIniFile->SetInteger(strSec, strTemp, nLen1);
	//	for (int j = 0; j < nLen1; j++) {
	//		dTempValue = pData->darrayEncap[i-32][j];
	//		strTemp.Format("PVI Teach Encap %s[%d]", str[i], j + 1);
	//		ptrIniFile->SetDouble(strSec, strTemp, dTempValue);
	//		//OutputDebugLogTo(9, TRUE, "Set Encap %s = %.2f", strTemp, dTempValue);//// for testing
	//	}
	//}

	//for (int i = 47; i<60; i++) {
	//	nLen1 = (int)pData->darrayCoverLayer[i - 47].GetCount();
	//	strTemp.Format("PVI Teach CoverLayer %s Count", str[i]);
	//	ptrIniFile->SetInteger(strSec, strTemp, nLen1);
	//	for (int j = 0; j < nLen1; j++) {
	//		dTempValue = pData->darrayCoverLayer[i - 47][j];
	//		strTemp.Format("PVI Teach CoverLayer %s[%d]", str[i], j + 1);
	//		ptrIniFile->SetDouble(strSec, strTemp, dTempValue);
	//		//OutputDebugLogTo(9, TRUE, "Set CoverLayer %s = %.2f", strTemp, dTempValue);//// for testing
	//	}
	//}

	ptrIniFile->WriteFile();
	return 0;
}


int CInspectionHandler::GetPVIDefectParameters(CString strRecipeFolder, int nTrack, int nDoc, int nFov)
{
	CIniFile ptrIniFile;
	CString strRecipeFile;
	CString strDataFileName;
	strRecipeFile.Format("PVIDefectParameters_%d",nFov+1);
	strDataFileName.Format("%s\\%s.pvi",strRecipeFolder,strRecipeFile);
	ptrIniFile.SetPath(strDataFileName);

	if(ptrIniFile.ReadFile()){
		m_Inspection.m_TrainingData.nNoOfPviArea = ptrIniFile.GetInteger("PVI Area", "Count",1);
		for(int nArea=0;nArea<m_Inspection.m_TrainingData.nNoOfPviArea;nArea++)
		{
			CString str;
			str.Format("PVIArea_%d",nArea+1);
			m_Inspection.m_pPviInspectionSetup[nArea].bEnable = ptrIniFile.GetBoolean(str,"Enable",0);
			//m_Inspection.m_pPviInspectionSetup[nArea].bNonUniformSurface = ptrIniFile.GetBoolean(str,"Enable NonUniformSource",0);
			m_Inspection.m_pPviInspectionSetup[nArea].dDeviceEdgeWidth = ptrIniFile.GetDouble(str, "Device Edge Width", m_Inspection.m_pPviInspectionSetup[nArea].dDeviceEdgeWidth);
			m_Inspection.m_pPviInspectionSetup[nArea].dDeviceEdgeOffset = ptrIniFile.GetDouble(str, "Device Edge Offset", m_Inspection.m_pPviInspectionSetup[nArea].dDeviceEdgeOffset);
			m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber = ptrIniFile.GetInteger(str,"Device Mask Number",0);
			int tempAretype = ptrIniFile.GetInteger(str,"Device Mask Type",0);
			m_Inspection.m_pPviInspectionSetup[nArea].nDeviceAreaType = tempAretype==0?RECTANGLE:tempAretype==1?POLYGON:ECLIPSE;
			for(int nMask=0;nMask<m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber;nMask++){
				CString strTemp;
				strTemp.Format("Device Mask Type_%d",nMask+1);
				tempAretype = ptrIniFile.GetInteger(str,strTemp,0);
				m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskArea[nMask] = tempAretype==0?RECTANGLE:tempAretype==1?POLYGON:ECLIPSE;
			}

			m_Inspection.m_pPviInspectionSetup[nArea].bPVIRecheckEnable = ptrIniFile.GetBoolean(str, "Recheck Enable", m_Inspection.m_pPviInspectionSetup[nArea].bPVIRecheckEnable);
			m_Inspection.m_pPviInspectionSetup[nArea].nMinGVDiff = ptrIniFile.GetInteger(str, "Min. Mean GV Diff", m_Inspection.m_pPviInspectionSetup[nArea].nMinGVDiff);
			m_Inspection.m_pPviInspectionSetup[nArea].nIntensity = ptrIniFile.GetInteger(str, "Max Contrast", m_Inspection.m_pPviInspectionSetup[nArea].nIntensity);

			m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckEnable = ptrIniFile.GetBoolean(str, "Slot Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckEnable);
			m_Inspection.m_pPviInspectionSetup[nArea].nNumOfSlot = ptrIniFile.GetInteger(str, "Number of Slot", m_Inspection.m_pPviInspectionSetup[nArea].nNumOfSlot);
			m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckTopDie = ptrIniFile.GetBoolean(str, "Top Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckTopDie);
			m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckBottomDie = ptrIniFile.GetBoolean(str, "Bottom Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckBottomDie);
			m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckLeftDie = ptrIniFile.GetBoolean(str, "Left Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckLeftDie);
			m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckRightDie = ptrIniFile.GetBoolean(str, "Right Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckRightDie);

			
			CString strEdge;
			strEdge.Format("%s_Edge", str);
			m_Inspection.m_pPviInspectionSetup[nArea].bEdgeEnable = ptrIniFile.GetBoolean(strEdge, "Enable", 0);
			m_Inspection.m_pPviInspectionSetup[nArea].nMinEdgeContrast = ptrIniFile.GetInteger(strEdge, "Min Contrast", m_Inspection.m_pPviInspectionSetup[nArea].nMinEdgeContrast);
			m_Inspection.m_pPviInspectionSetup[nArea].nMaxEdgeContrast = ptrIniFile.GetInteger(strEdge, "Max Contrast", m_Inspection.m_pPviInspectionSetup[nArea].nMaxEdgeContrast);
			m_Inspection.m_pPviInspectionSetup[nArea].dEdgeRegionOpen = ptrIniFile.GetDouble(strEdge, "Edge Region Open", m_Inspection.m_pPviInspectionSetup[nArea].dEdgeRegionOpen);
			m_Inspection.m_pPviInspectionSetup[nArea].dEdgeWidth = ptrIniFile.GetDouble(strEdge, "Edge Width", m_Inspection.m_pPviInspectionSetup[nArea].dEdgeWidth);
			m_Inspection.m_pPviInspectionSetup[nArea].dEdgeOffset = ptrIniFile.GetDouble(strEdge, "Edge Offset", m_Inspection.m_pPviInspectionSetup[nArea].dEdgeOffset);

			CString strVariable;
			strVariable.Format("TeachPviArea RectPos%d", nArea+1);
			m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviArea = ptrIniFile.GetCRect(str, strVariable,m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviArea);
			for(int j = 0;j<m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber;j++){
				strVariable.Format("TeachPviAreaMaskRegion RectPos%dMask%d",nArea+1, j+1);
				m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[j] = ptrIniFile.GetCRect(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[j]);
			}
			for (int j = 0; j<m_Inspection.m_pPviInspectionSetup[nArea].nNumOfSlot; j++) {
				strVariable.Format("TeachPviAreaSlotRegion RectPos%dSlot%d", nArea + 1, j + 1);
				m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviSlot[j] = ptrIniFile.GetCRect(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviSlot[j]);
			}

			//PVI Polygon Type, save the polygon points
			strVariable.Format("TeachPviArea Polygon Point Count%d", nArea + 1);
			m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber = ptrIniFile.GetInteger(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber);
			if (m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber > 20)
				m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber = 20;
			else if (m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber < 3)
				m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber = 3;

			for (int j = 0; j < m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber; j++) {
				strVariable.Format("TeachPviArea PolygonPos%dPoint%d", nArea + 1, j + 1);
				m_Inspection.m_pPviInspectionSetup[nArea].m_polygonPviArea[j] = ptrIniFile.GetCPoint(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_polygonPviArea[j]);
			}

			m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount = ptrIniFile.GetInteger(str,"Defect Count",0);
			for(int i= 0;i<m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;i++){
				CString strKeyIndex;
				strKeyIndex.Format("%d", i + 1);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nID = ptrIniFile.GetInteger(str + "_DefectsIndex", strKeyIndex, m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nID);
				strKeyIndex.Format("%d", m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nID);
				CString strTempKey;
				//strTempKey.Format("%s_%s",str,m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName = ptrIniFile.GetCString(str+"_Defects", strKeyIndex,"");
				strTempKey.Format("%s_%s",str,m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].bEnable = ptrIniFile.GetBoolean(strTempKey,"Enable",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].bEnable);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nDefectCharacteristics = ptrIniFile.GetInteger(strTempKey,"DefectCharacteristics",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nDefectCharacteristics);

				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nSize = ptrIniFile.GetInteger(strTempKey,"Size",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nSize);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nContrast = ptrIniFile.GetInteger(strTempKey,"Contrast",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nContrast);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinLength = ptrIniFile.GetInteger(strTempKey,"MinLength",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinLength);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinSquareSize = ptrIniFile.GetInteger(strTempKey,"MinSquareSize",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinSquareSize);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinCount = ptrIniFile.GetInteger(strTempKey,"MinCount",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinCount);
			
				//The error code id will create based on the hash ID of the track ID, Doc ID and Defect name,  --> can be dynamic depend on the requirement
				strTempKey.Format("Track%d_Doc%d_%s", nTrack, nDoc, m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
				int nHasID = std::hash<std::string>()((LPCSTR)strTempKey);
				m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nErrorCodeID = abs(nHasID);
			}
		}
	}
	else
	{
		//m_ParametersInspSetUP.RemoveAll();
		//CString strSec, strVariable;
		//m_Inspection.m_TrainingData.nNoOfPviArea = 1;
		//m_Inspection.m_pPviInspectionSetup[0].bEnable = TRUE;
		//m_Inspection.m_pPviInspectionSetup[0].bNonUniformSurface = TRUE;
		//m_Inspection.m_pPviInspectionSetup[0].nDeviceMaskNumber = 0;
		//m_Inspection.m_pPviInspectionSetup[0].nDeviceAreaType = RECTANGLE; //0:-Rectangle && 1:-Eclipse
		//ptrIniFile.SetInteger("PVI Area", "Count",m_Inspection.m_TrainingData.nNoOfPviArea);
		//m_Inspection.m_pPviInspectionSetup[0].nDefectCount = 0;
		//for(int nArea=0;nArea<m_Inspection.m_TrainingData.nNoOfPviArea;nArea++)
		//{
		//	CString str;
		//	str.Format("PVIArea_%d",nArea+1);
		//	ptrIniFile.SetBoolean(str,"Enable",m_Inspection.m_pPviInspectionSetup[nArea].bEnable);
		//	ptrIniFile.SetBoolean(str,"Enable NonUniformSource",m_Inspection.m_pPviInspectionSetup[nArea].bNonUniformSurface);
		//	ptrIniFile.SetInteger(str,"Device Mask Number",m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber);
		//	ptrIniFile.SetInteger(str,"Device Mask Type",m_Inspection.m_pPviInspectionSetup[nArea].nDeviceAreaType);
		//	for(int nMask=0;nMask<m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber;nMask++){
		//		CString strTemp;
		//		strTemp.Format("Device Mask Type_%d",nMask+1);
		//		ptrIniFile.SetInteger(str,strTemp,m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskArea[nMask]);
		//	}
		//	CString strVariable;
		//	strVariable.Format("TeachPviArea RectPos%d", nArea+1);
		//	m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviArea = CRect(100,100,300,300);
		//	ptrIniFile.SetCRect(str, strVariable,m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviArea);
		//	for(int j = 0;j<m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber;j++){
		//		strVariable.Format("TeachPviAreaMaskRegion RectPos%dMask%d",nArea+1, j+1);
		//		m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[j] = CRect(100,100,300,300);
		//		ptrIniFile.SetCRect(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[j]);
		//	}
		//	int nCount = m_Inspection.m_pPviInspectionSetup[nArea].m_ParametersDefect.GetSize();
		//	m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount = m_Inspection.m_pPviInspectionSetup[nArea].m_ParametersDefect.GetSize();
		//	ptrIniFile.SetInteger(str,"Defect Count",m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount);
		//	for(int i= 0;i<nCount;i++){
		//		CString strKeyIndex;
		//		strKeyIndex.Format("%d",i);
		//		CString strTempKey;
		//		strTempKey.Format("%s_%s",str,m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
		//		ptrIniFile.SetCString("Defects", strKeyIndex,m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
		//		ptrIniFile.SetBoolean(strTempKey,"Enable",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].bEnable);
		//		ptrIniFile.SetInteger(strTempKey,"Size",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nSize);
		//	}
		//}
		//ptrIniFile.WriteFile();

		return -1;
	}
	return 0;
}


int CInspectionHandler::SetPVIDefectParameters(CString strRecipeFolder, int nTrack, int nDoc, int nFov)
{
	
	CIniFile ptrIniFile;
	CString strRecipeFile;
	CString strDataFileName;
	strRecipeFile.Format("PVIDefectParameters_%d",nFov+1);
	strDataFileName.Format("%s\\%s.pvi",strRecipeFolder,strRecipeFile);
	ptrIniFile.SetPath(strDataFileName);

	CString strSec, strVariable;
	CString str;
	ptrIniFile.SetInteger("PVI Area", "Count",m_Inspection.m_TrainingData.nNoOfPviArea);
	for(int nArea=0;nArea<m_Inspection.m_TrainingData.nNoOfPviArea;nArea++)
	{
		
		str.Format("PVIArea_%d",nArea+1);
		ptrIniFile.SetBoolean(str,"Enable",m_Inspection.m_pPviInspectionSetup[nArea].bEnable);
		//ptrIniFile.SetBoolean(str,"Enable NonUniformSource",m_Inspection.m_pPviInspectionSetup[nArea].bNonUniformSurface);
		ptrIniFile.SetDouble(str, "Device Edge Width", m_Inspection.m_pPviInspectionSetup[nArea].dDeviceEdgeWidth);
		ptrIniFile.SetDouble(str, "Device Edge Offset", m_Inspection.m_pPviInspectionSetup[nArea].dDeviceEdgeOffset);

		ptrIniFile.SetInteger(str,"Device Mask Number",m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber);
		ptrIniFile.SetInteger(str,"Device Mask Type",m_Inspection.m_pPviInspectionSetup[nArea].nDeviceAreaType);
		for(int nMask=0;nMask<m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber;nMask++){
			CString strTemp;
			strTemp.Format("Device Mask Type_%d",nMask+1);
			ptrIniFile.SetInteger(str,strTemp,m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskArea[nMask]);
		}

		ptrIniFile.SetInteger(str, "Recheck Enable", m_Inspection.m_pPviInspectionSetup[nArea].bPVIRecheckEnable);
		ptrIniFile.SetInteger(str, "Min. Mean GV Diff", m_Inspection.m_pPviInspectionSetup[nArea].nMinGVDiff);
		ptrIniFile.SetInteger(str, "Max Contrast", m_Inspection.m_pPviInspectionSetup[nArea].nIntensity);

		ptrIniFile.SetInteger(str, "Slot Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckEnable);
		ptrIniFile.SetInteger(str, "Number of Slot", m_Inspection.m_pPviInspectionSetup[nArea].nNumOfSlot);
		ptrIniFile.SetInteger(str, "Top Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckTopDie);
		ptrIniFile.SetInteger(str, "Bottom Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckBottomDie);
		ptrIniFile.SetInteger(str, "Left Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckLeftDie);
		ptrIniFile.SetInteger(str, "Right Die Connection Check", m_Inspection.m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckRightDie);



		CString strEdge;
		strEdge.Format("%s_Edge",str);
		ptrIniFile.SetBoolean(strEdge, "Enable", m_Inspection.m_pPviInspectionSetup[nArea].bEdgeEnable);

		ptrIniFile.SetInteger(strEdge, "Min Contrast", m_Inspection.m_pPviInspectionSetup[nArea].nMinEdgeContrast);
		ptrIniFile.SetInteger(strEdge, "Max Contrast", m_Inspection.m_pPviInspectionSetup[nArea].nMaxEdgeContrast);
		ptrIniFile.SetDouble(strEdge, "Edge Region Open", m_Inspection.m_pPviInspectionSetup[nArea].dEdgeRegionOpen);
		ptrIniFile.SetDouble(strEdge, "Edge Width", m_Inspection.m_pPviInspectionSetup[nArea].dEdgeWidth);
		ptrIniFile.SetDouble(strEdge, "Edge Offset", m_Inspection.m_pPviInspectionSetup[nArea].dEdgeOffset);

		CString strVariable;
		strVariable.Format("TeachPviArea RectPos%d", nArea+1);
		ptrIniFile.SetCRect(str, strVariable,m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviArea);
		for(int j = 0;j<m_Inspection.m_pPviInspectionSetup[nArea].nDeviceMaskNumber;j++){
			strVariable.Format("TeachPviAreaMaskRegion RectPos%dMask%d",nArea+1, j+1);
			ptrIniFile.SetCRect(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[j]);
		}
		for (int j = 0; j<m_Inspection.m_pPviInspectionSetup[nArea].nNumOfSlot; j++) {
			strVariable.Format("TeachPviAreaSlotRegion RectPos%dSlot%d", nArea + 1, j + 1);
			ptrIniFile.SetCRect(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_rectPviSlot[j]);
		}

		//PVI Polygon Type, Polygon Points
		strVariable.Format("TeachPviArea Polygon Point Count%d", nArea + 1);
		ptrIniFile.SetInteger(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber);
		for (int j = 0; j < m_Inspection.m_pPviInspectionSetup[nArea].nPolygonPointNumber; j++) {
			strVariable.Format("TeachPviArea PolygonPos%dPoint%d", nArea + 1, j + 1);
			ptrIniFile.SetCPoint(str, strVariable, m_Inspection.m_pPviInspectionSetup[nArea].m_polygonPviArea[j]);
		}

		//int nCount = m_Inspection.m_pPviInspectionSetup[nArea].m_ParametersDefect.GetSize();
		//m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount = m_Inspection.m_pPviInspectionSetup[nArea].m_ParametersDefect.GetSize();
		ptrIniFile.SetInteger(str,"Defect Count",m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount);
		for(int i= 0;i<m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;i++){
			CString strKeyIndex;
			strKeyIndex.Format("%d", i + 1);
			ptrIniFile.SetInteger(str + "_DefectsIndex", strKeyIndex, m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nID);
			strKeyIndex.Format("%d", m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nID);
			CString strTempKey;
			//strTempKey.Format("%s_%s",str,m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
			ptrIniFile.SetCString(str+"_Defects", strKeyIndex,m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
			strTempKey.Format("%s_%s",str,m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
			ptrIniFile.SetBoolean(strTempKey,"Enable",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].bEnable);
			ptrIniFile.SetInteger(strTempKey,"DefectCharacteristics",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nDefectCharacteristics);

			ptrIniFile.SetInteger(strTempKey,"Size",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nSize);
			ptrIniFile.SetInteger(strTempKey,"Contrast",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nContrast);
			ptrIniFile.SetInteger(strTempKey,"MinLength",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinLength);
			ptrIniFile.SetInteger(strTempKey,"MinSquareSize",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinSquareSize);
			ptrIniFile.SetInteger(strTempKey,"MinCount",m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].nMinCount);

			//The error code id will create based on the hash ID of the track ID, Doc ID and Defect name,  --> can be dynamic depend on the requirement
			strTempKey.Format("Track%d_Doc%d_%s", nTrack, nDoc, m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].strDefectName);
			int nHasID = std::hash<std::string>()((LPCSTR)strTempKey);
			m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[i].m_nErrorCodeID = abs(nHasID);
		}
	}
	
	ptrIniFile.WriteFile();
	return 0;
}

int CInspectionHandler::LoadCalibrationParameters(CString strRecipeFolder, CString strRecipeFile, int nTrack, int nFOV)
{
	CIniFile iniFile;
	CString strCalibFileName, strSec;
	CString str[] = { "Top", "Bottom", "Side" };

	//strCalibFileName.Format("%s\\%s_%d.calib", strRecipeFolder, strRecipeFile, nFOV + 1);
	strCalibFileName.Format("%s\\Light_%s.calib", strRecipeFolder, str[nTrack]);
	iniFile.SetPath(strCalibFileName);
	
	if (iniFile.ReadFile()) {
		CCalibrationData* pCalibData = &m_Inspection.m_CalibrationData;
		strSec.Format("Calibration Data");

		pCalibData->rectCalibSideTrack = iniFile.GetCRect(strSec, "Side Rectangle", pCalibData->rectCalibSideTrack);
		pCalibData->nCalibTolerence = iniFile.GetInteger(strSec, "Tolerance", pCalibData->nCalibTolerence);
		pCalibData->nTargetGreyDeviation = iniFile.GetInteger(strSec, "Gray Deviation", pCalibData->nTargetGreyDeviation);
		pCalibData->nTargetGreyMeanValue = iniFile.GetInteger(strSec, "Gray Mean Value", pCalibData->nTargetGreyMeanValue);
		pCalibData->nCalibTolerenceSide = iniFile.GetInteger(strSec, "Tolerance Side", pCalibData->nCalibTolerenceSide);
		pCalibData->nTargetGreyMeanValueSide = iniFile.GetInteger(strSec, "Gray Mean Value Side", pCalibData->nTargetGreyMeanValueSide);

		pCalibData->bEnableLightCorrection = iniFile.GetBoolean(strSec, "Enable Light Correction", FALSE);

//		OutputDebugLogTo(9, TRUE, "LoadCalibrationParameters T%d, FOV%d", nTrack + 1, nFOV + 1);//// for testing
	}

	//Light Correction Image
	CString strLightCorrectionImage;
	strLightCorrectionImage.Format("%s\\%s_Light_Correction.bmp", strRecipeFolder, str[nTrack]);
	CImageBuffer ImgLightBuf;
	int nError = LoadGrayScaleBMPFile(strLightCorrectionImage, ImgLightBuf);
	if (!nError) {
		CCalibrationData* pCalibData = &m_Inspection.m_CalibrationData;
		pCalibData->hCalibMaxImage.GenImage1("byte", ImgLightBuf.GetPitch(), ImgLightBuf.GetImgSize().cy, ImgLightBuf.GetImgPtr());
	}
	

	return 0;
}

int CInspectionHandler::SaveCalibrationParameters(CString strRecipeFolder, CString strRecipeFile, int nTrack, int nFOV)
{
	CIniFile iniFile;
	CString strCalibFileName, strSec;
	CString str[] = { "Top", "Bottom", "Side" };

	//strCalibFileName.Format("%s\\%s_%d.calib", strRecipeFolder, strRecipeFile, nFOV + 1);
	strCalibFileName.Format("%s\\Light_%s.calib", strRecipeFolder, str[nTrack]);
	iniFile.SetPath(strCalibFileName);

	CCalibrationData* pCalibData = &m_Inspection.m_CalibrationData;
	strSec.Format("Calibration Data");

	iniFile.SetCRect(strSec, "Side Rectangle", pCalibData->rectCalibSideTrack);
	iniFile.SetInteger(strSec, "Tolerance", pCalibData->nCalibTolerence);
	iniFile.SetInteger(strSec, "Gray Deviation", pCalibData->nTargetGreyDeviation);
	iniFile.SetInteger(strSec, "Gray Mean Value", pCalibData->nTargetGreyMeanValue);
	iniFile.SetInteger(strSec, "Tolerance Side", pCalibData->nCalibTolerenceSide);
	iniFile.SetInteger(strSec, "Gray Mean Value Side", pCalibData->nTargetGreyMeanValueSide);
	iniFile.SetBoolean(strSec, "Enable Light Correction", pCalibData->bEnableLightCorrection);

	iniFile.WriteFile();

	return 0;
}


int CInspectionHandler::SaveDotCalibrationParameters(CString strRecipeFolder, CString strRecipeFile, int nTrack, int nFOV)
{
	CIniFile iniFile;
	CString strCalibFileName, strSec;
	CString str[] = { "Top", "Bottom", "Side" };

	//strCalibFileName.Format("%s\\%s_%d.calib", strRecipeFolder, strRecipeFile, nFOV + 1);
	strCalibFileName.Format("%s\\Dot_%s.calib", strRecipeFolder, str[nTrack]);
	iniFile.SetPath(strCalibFileName);

	CCalibrationData* pCalibData = &m_Inspection.m_CalibrationData;
	strSec.Format("Dot Calibration Data");

	iniFile.SetCRect(strSec, "Dot Calib Rectangle", pCalibData->rectDotCalib);
	iniFile.SetInteger(strSec, "Dot Input AlongX", pCalibData->nInputAlongX);
	iniFile.SetInteger(strSec, "Dot Input AlongY", pCalibData->nInputAlongY);
	iniFile.SetDouble(strSec, "Dot ResolutionAlongXInMPerP", pCalibData->dResolutionAlongXInMicronPerPixel);
	iniFile.SetDouble(strSec, "Dot ResolutionAlongYInMPerP", pCalibData->dResolutionAlongYInMicronPerPixel);

	iniFile.SetInteger(strSec, "Minimum Radius Dot Target In Pixel", pCalibData->nInputMinRadiusDotTargetPixel);

	iniFile.WriteFile();

	return 0;

}

int CInspectionHandler::LoadDotCalibrationParameters(CString strRecipeFolder, CString strRecipeFile, int nTrack, int nFOV)
{
	CIniFile iniFile;
	CString strCalibFileName, strSec;
	CString str[] = { "Top", "Bottom", "Side" };

	//strCalibFileName.Format("%s\\%s_%d.calib", strRecipeFolder, strRecipeFile, nFOV + 1);
	strCalibFileName.Format("%s\\Dot_%s.calib", strRecipeFolder, str[nTrack]);
	iniFile.SetPath(strCalibFileName);

	if (iniFile.ReadFile()) {
		CCalibrationData* pCalibData = &m_Inspection.m_CalibrationData;
		strSec.Format("Dot Calibration Data");

		pCalibData->rectDotCalib = iniFile.GetCRect(strSec, "Dot Calib Rectangle", pCalibData->rectDotCalib);
		pCalibData->nInputAlongX = iniFile.GetInteger(strSec, "Dot Input AlongX", pCalibData->nInputAlongX);
		pCalibData->nInputAlongY = iniFile.GetInteger(strSec, "Dot Input AlongY", pCalibData->nInputAlongY);
		pCalibData->dResolutionAlongXInMicronPerPixel = iniFile.GetDouble(strSec, "Dot ResolutionAlongXInMPerP", pCalibData->dResolutionAlongXInMicronPerPixel);
		pCalibData->dResolutionAlongYInMicronPerPixel = iniFile.GetDouble(strSec, "Dot ResolutionAlongYInMPerP", pCalibData->dResolutionAlongYInMicronPerPixel);

		pCalibData->nInputMinRadiusDotTargetPixel = iniFile.GetInteger(strSec, "Minimum Radius Dot Target In Pixel", pCalibData->nInputMinRadiusDotTargetPixel);

		//		OutputDebugLogTo(9, TRUE, "LoadDotCalibrationParameters T%d, FOV%d", nTrack+1, nFOV+1);//// for testing
	}

	return 0;
}