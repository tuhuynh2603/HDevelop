/// Cuda Functions Interface class
/// Udupa; June'2014

#pragma once

class CImageBuffer;

class CCudaInterface
{
public:
	CCudaInterface();
	~CCudaInterface();

	void InitializeCudaKernel(CSize szImage);
	void CloseCudaKernel();

	int ImgAND(BYTE *pRowSrc0,
			int nImgPitchSrc0,
			BYTE *pRowSrc1,
			int nImgPitchSrc1,
			BYTE *pRowDst,
			int nImgPitchDst,
			int nWidthDst,
			int nHeightDst);
	int ImgOR(BYTE *pRowSrc0,
			int nImgPitchSrc0,
			BYTE *pRowSrc1,
			int nImgPitchSrc1,
			BYTE *pRowDst,
			int nImgPitchDst,
			int nWidthDst,
			int nHeightDst);
	int GetGradMaxImage(CImageBuffer *pImgSrc,
						CRect* prcSrc,
						int nColor,
						CImageBuffer *pImgDst,
						CRect* prcDst,
						CRect *prcGradStep);

	int ImageBinDilation(CImageBuffer *pImgSrc, 
						CRect *prcSrc,
						int nBlobType,
						CImageBuffer *pImgDst,
						CRect *prcDst,
						CSize* pszMask);

	BOOL m_bOptimizeGpu;
	BOOL m_bOptimizeCpu;
	BOOL m_bGpuAvailable;
	CRITICAL_SECTION m_csCuda;
};

