// Morphology.h: interface for the CMorphology class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Imaging.h"

//#include "EdgeMatching.h"
//#include "Imaging.h"

#define GRAY_SCALE_IMAGE	1
#define BINARY_IMAGE		2


#define DILATION	1
#define EROSION		2
#define AVERAGING	3


class CMorphology
{
public:
	CMorphology();
	~CMorphology();
	void Clean();
	int Process(CImageBuffer* pImgIn,
				CRect* prcIn,
				int nImgType,
				int nProcType,
				CSize* pcsKernel,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int Process(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgMask,
				CRect* prcMask,
				int nImgType,
				int nProcType,
				CSize* pcsKernel,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int ProcessBySampling(CImageBuffer* pImgIn,
				CRect* prcIn,
				int nImgType,
				int nProcType,
				CImageBuffer* pKernel,
				CRect* prcKernel,
				CImageBuffer* pImgMask,
				CImageBuffer* pImgOut,
				CRect* prcOut,
				CSize* pcsSampling,
				int nThreshold=-1);

private:
	int* pnKernelAddress;
	int nKernelAddress;
	int nKernelWidth;
	int nKernelArea;
	int nKernelHeight;
	int nKernelCtrX;
	int nKernelCtrY;

private:
	int GrayErosionProcessBySampling(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					CSize* pcsSampling,
					int nThreshold=-1);

	int Morphology1D(CImageBuffer* pImgSrc,
				  CRect* prcSrc,
				  int nDir,
				  int nProcMode,
				  int nWidth,
				  CImageBuffer* pImgDst,
				  CRect* prcDst);

	int Morphology1D(CImageBuffer* pImgSrc,
				  CRect* prcSrc,
				  CImageBuffer* pImgMask,
				  CRect* prcMask,
				  int nDir,
				  int nProcMode,
				  int nWidth,
				  CImageBuffer* pImgDst,
				  CRect* prcDst);

	int EdgePartMorphology(CImageBuffer* pImgSrc,
				  CRect* prcSrc,
				  int nDir,
				  int nWidth,
				  CImageBuffer* pImgDst,
				  CRect* prcDst);

	int CopyEdgeNoMorphology(CImageBuffer* pImgSrc,
				  CRect* prcSrc,
				  int nDir,
				  int nWidth,
				  CImageBuffer* pImgDst,
				  CRect* prcDst);

	int ValidCheck(CImageBuffer* pImgIn,
					CRect* prcIn,
					int nImgType,
					int nProcType,
					CImageBuffer* pKernel,
					CRect* prcKernel,
					CImageBuffer* pImgOut,
					CRect* prcOut);

	int KernelAnalysis(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pKernel,
				CRect* prcKernel);
	int GrayProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				int nImgType,
				int nProcType,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int GrayDilationProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int GrayDilationProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgMask,
				CImageBuffer* pImgOut,
				CRect* prcOut,
				int nThreshold);

	int GrayDilationProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold);

	int GrayErosionProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int GrayErosionProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgMask,
				CImageBuffer* pImgOut,
				CRect* prcOut,
				int nThreshold);

	int GrayErosionProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold);

	int BinaryProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				int nImgType,
				int nProcType,
				CImageBuffer* pImgOut,
				CRect* prcOut);
	
	int BinaryDilationProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int BinaryErosionProcess(CImageBuffer* pImgIn,
				CRect* prcIn,
				CImageBuffer* pImgOut,
				CRect* prcOut);

	int GrayAveragingProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold);

	int GrayAveragingProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold);
};

int GrayMaskBinarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
				   CRect *prcSrc,				// ROI of Source Image
				   int nBlobType,
				   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
				   CRect *prcMsk, 				// ROI of Mask Image
				   BYTE nMaskValue,
				   BYTE byteThresVal, 
				   CImageBuffer *pImgDst,  			// Pointer to Destination Image
				   CRect *prcDst); 				// ROI of Destination Image

int GrayMaskAdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
					  CRect *prcSrc,			// ROI of the Source Image
					  CImageBuffer *pImgMsk,			// Pointer to Mask Image
					  CRect *prcMsk,			// ROI of the Mask Image
					  BYTE nMaskValue,
					  long *plAry);				// Histogram Data
