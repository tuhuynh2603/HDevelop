// JPEG image load/save function
// Udupa; 2003
// Udupa; March'2013; Added generic format (color/grayscale) saving option
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cdjpeg.h"
#include "JpegFunctions.h"
#include "MathValidations.h"
#include "ImageDef.h"
#include "ImageFunction.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int LoadJPGFile(CString strFileName, CImageBuffer& bufImage)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * input_file;
	JDIMENSION num_scanlines;
	
	DWORD dwTime = GetTickCount();

	// Initialize the JPEG decompression object with default error handling
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// Add some application-specific error messages (from cderror.h)
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;
	
	// Open the input file
	errno_t err = fopen_s(&input_file, strFileName.GetBuffer(0), READ_BINARY);
	if (err || input_file == NULL)
		return FALSE;
	
	try {
		// Specify data source for decompression
		jpeg_stdio_src(&cinfo, input_file);

		// Read file header, set default decompression parameters
		(void) jpeg_read_header(&cinfo, TRUE);
		
		// Set JPEG Parameters
		cinfo.err->trace_level = 0;
		cinfo.dct_method = JDCT_FLOAT;
		
		// Start decompressor
		(void) jpeg_start_decompress(&cinfo);
		
		// Process data
		int nPitchSrc = cinfo.output_width*cinfo.num_components;
		UCHAR* pImageBuf = bufImage.GetImgPtr();
		UCHAR* pTempBuf = new UCHAR[abs(nPitchSrc)];
		UCHAR** pTempBufPtr = &pTempBuf;

		UINT nImageWidth = bufImage.GetImgSize().cx;
		UINT nImageHeight = bufImage.GetImgSize().cy;
		UINT nPitchDst = bufImage.GetPitch();
		int i, j;

		while (cinfo.output_scanline < cinfo.output_height) {
			num_scanlines = jpeg_read_scanlines(&cinfo, pTempBufPtr, 1);
			if(cinfo.output_scanline<nImageHeight) {
				if(cinfo.num_components == 3) {
					for(i=0, j=0; i<nPitchSrc; i+=3, j++) {
						if((*pTempBufPtr)[i]>100 && (*pTempBufPtr)[i]-(*pTempBufPtr)[i+1]>50 && (*pTempBufPtr)[i]-(*pTempBufPtr)[i+1]>50)
							pImageBuf[j] = 0;
						else if((*pTempBufPtr)[i+2]>100 && (*pTempBufPtr)[i+2]-(*pTempBufPtr)[i]>50 && (*pTempBufPtr)[i+2]-(*pTempBufPtr)[i+1]>50)
							pImageBuf[j] = 1;
						else if((*pTempBufPtr)[i]==0 || (*pTempBufPtr)[i]==1)
							pImageBuf[j] = 2;
						else
							pImageBuf[j] = (*pTempBufPtr)[i];
					}
				}
				else {
					memcpy(pImageBuf, *pTempBufPtr, nImageWidth);
				}
				pImageBuf += nPitchDst;
			}
		}
		if(bufImage.GetAdapter() != NULL) {
			RGBQUAD *pRGB = new RGBQUAD[256];
			for(int i=0;i<256;i++)
				pRGB[i].rgbRed = pRGB[i].rgbGreen = pRGB[i].rgbBlue = i;
			if(cinfo.num_components == 3) {
				pRGB[0].rgbRed = 255;
				pRGB[1].rgbRed = 0;
				pRGB[1].rgbGreen = 0;
				pRGB[1].rgbBlue = 255;
			}
			bufImage.GetAdapter()->SetColorTable(256, pRGB);
			//bufImage.SetLinearLookupTable(TRUE);
		}
	}
	catch(CException* pe) {
		(void) jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		AfxMessageBox("Read error");
		pe->Delete();
		return -1;
	}
	// Finish decompression and release memory.
	// I must do it in this order because output module has allocated memory
	// of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	// Close files, if we opened them
	if (input_file != stdin)
		fclose(input_file);

	dwTime = GetTickCount() - dwTime;
	CString strMsg;
	strMsg.Format("Time for Jpeg DeCompression = %d", dwTime);
	//AfxMessageBox(strMsg);

	return (jerr.num_warnings ? -1 : 0);
}

int SaveJPGFile(CString strFileName, CImageBuffer& bufImage, BYTE nQuality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * output_file;
	
	DWORD dwTime = GetTickCount();
	
	// Initialize the JPEG compression object with default error handling
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	// Add some application-specific error messages (from cderror.h)
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;
	
	
	// Initialize JPEG parameters.
	cinfo.in_color_space = bufImage.IsColor() ? JCS_RGB : JCS_GRAYSCALE; // arbitrary guess
	jpeg_set_defaults(&cinfo);

	errno_t err = fopen_s(&output_file, strFileName.GetBuffer(0), WRITE_BINARY);
	if(err || output_file == NULL)
		return FALSE;

	cinfo.image_width = bufImage.GetImgSize().cx;
	cinfo.image_height = bufImage.GetImgSize().cy;
	cinfo.input_components = bufImage.IsColor() ? 3 : 1;
	cinfo.data_precision = 8;

	// Now that we know input colorspace, fix colorspace-dependent defaults
	jpeg_default_colorspace(&cinfo);

	if(bufImage.IsColor()) {
		cinfo.X_density = (UINT16) (2925/100);
		cinfo.Y_density = (UINT16) (2925/100);
		cinfo.density_unit = 2;
	}
	else {
		cinfo.X_density = 2925;
		cinfo.Y_density = 2925;
	}


	//Set JPEG Parameters
	cinfo.err->trace_level = 0;
	cinfo.dct_method = JDCT_FLOAT;
	jpeg_set_colorspace(&cinfo, bufImage.IsColor() ? JCS_RGB : JCS_GRAYSCALE);		// Force a monochrome JPEG file to be generated
	cinfo.optimize_coding = TRUE;		// Enable entropy parm optimization
	jpeg_set_quality(&cinfo, nQuality, FALSE);		// Set quantization tables for selected quality

	try {
		// Specify data destination for compression
		jpeg_stdio_dest(&cinfo, output_file);
		
		// Start compressor
		jpeg_start_compress(&cinfo, TRUE);
		
		// Process data
		UCHAR* pBuf = bufImage.GetImgPtr();
		UCHAR** pBufPtr = &pBuf;
		int nImageWidth = cinfo.image_width;
		if(bufImage.IsColor())
			nImageWidth *= 3;
		while (cinfo.next_scanline < cinfo.image_height) {
			(void) jpeg_write_scanlines(&cinfo, pBufPtr, 1);
			(*pBufPtr)+=nImageWidth;
		}
	}
	catch(CException* pe) {
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		pe->Delete();
		AfxMessageBox("write error");
		return -1;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	if (output_file != stdout)
		fclose(output_file);

	dwTime = GetTickCount() - dwTime;
	CString strMsg;
	strMsg.Format("Time for Jpeg Compression (Grayscale) = %d", dwTime);
	//AfxMessageBox(strMsg);
	return (jerr.num_warnings ? -1 : 0);
}

int SaveDaiIchiDefectsJPGFile(CString strFileName, 
						    CImageBuffer& bufImage, 
						    CRect* prcDefects, 
						    int nDefects,
							int nInnerRectOffset,
							int nMinInnerSize,
							int nOuterInnerDis,
							int nLineWidth,
							BYTE nQuality)
{
	CRect* prcLocationDefects;
	int nLocationDefects;
	int nError;

	if(nLineWidth<1)
		nLineWidth = 1;
	nLocationDefects = 2* nDefects;
	prcLocationDefects = new CRect[abs(nLocationDefects)];
	nError = CreateDefectsLocationWindow(bufImage.GetImgSize(),
										  prcDefects, 
										  nDefects,
										  &prcLocationDefects[0],
										  nLocationDefects,
										  nInnerRectOffset,
										  nMinInnerSize,
										  nOuterInnerDis);
	if(nError)
		goto ErrorAbort;
	nError = SaveDefectsJPGFile(strFileName, 
							    bufImage, 
								&prcLocationDefects[0], 
								nLocationDefects,
								nLineWidth,
								nQuality);
	delete [] prcLocationDefects;
	return 0;
ErrorAbort:
	delete [] prcLocationDefects;
	return -1;
}


int SaveDefectsJPGFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects, 
					   int nDefects, 
					   int nLineWidth,
					   BYTE nQuality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * output_file;
	int i, j, k;
	
	DWORD dwTime = GetTickCount();

	// Initialize the JPEG compression object with default error handling
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	// Add some application-specific error messages (from cderror.h)
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;
	
	
	// Initialize JPEG parameters.
	cinfo.in_color_space = JCS_RGB; // arbitrary guess
	jpeg_set_defaults(&cinfo);

	errno_t err = fopen_s(&output_file, strFileName.GetBuffer(0), WRITE_BINARY);
    if(err || output_file == NULL)
		return FALSE;

	cinfo.image_width = bufImage.GetImgSize().cx;
	cinfo.image_height = bufImage.GetImgSize().cy;
	cinfo.input_components = 3;
	cinfo.data_precision = 8;

	// Now that we know input colorspace, fix colorspace-dependent defaults
	jpeg_default_colorspace(&cinfo);

	cinfo.X_density = (UINT16) (2925/100);
	cinfo.Y_density = (UINT16) (2925/100);
	cinfo.density_unit = 2;

	//Set JPEG Parameters
	cinfo.err->trace_level = 0;
	cinfo.dct_method = JDCT_FLOAT;
	cinfo.optimize_coding = TRUE;		// Enable entropy parm optimization
	jpeg_set_quality(&cinfo, nQuality, FALSE);		// Set quantization tables for selected quality


	CSize csImg = bufImage.GetImgSize();

	if(nLineWidth < 1)
		nLineWidth = 1;

	int nSizeImage = csImg.cx*csImg.cy;
	UCHAR* pBuf = bufImage.GetImgPtr();
	UCHAR* bufImageRGB = new UCHAR[abs(nSizeImage*3)];
	for(i=0, j=0; i<nSizeImage; i++, j+=3)
		memset(bufImageRGB+j, pBuf[i], 3);

	CRect rcDefect;
	int	nDefectWidth;
	int	nDefectHeight;
	BYTE	*pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int nPitch = bufImage.GetPitch()*3;
	pBuf = bufImageRGB;
	int nHeight = bufImage.GetImgSize().cy;

	
	
	
	for(i=0; i<nDefects; i++) {
		if(ValidateRect(&prcDefects[i], &csImg))
			continue;
		rcDefect = prcDefects[i];
		rcDefect.InflateRect(nLineWidth,nLineWidth,nLineWidth,nLineWidth);
		if(CheckRect(&rcDefect, &csImg)) 
			continue;
		for(k=0; k<nLineWidth; k++) {
			if(CheckRect(&rcDefect, &csImg))
				break;
			nDefectWidth = rcDefect.Width();
			nDefectHeight = rcDefect.Height();
			pImgTopLftPtr = pBuf + rcDefect.top * nPitch + rcDefect.left*3;
			pImgProcLeftLine=pImgTopLftPtr;
			pImgProcRightLine = pImgTopLftPtr + (nDefectWidth - 1)*3;
			for(j = 0; j < nDefectHeight; j++, pImgProcLeftLine += nPitch, pImgProcRightLine += nPitch) {
				pImgProcLeftLine[0] = 255;
				pImgProcLeftLine[1] = 0;
				pImgProcLeftLine[2] = 0;
				pImgProcRightLine[0] = 255;
				pImgProcRightLine[1] = 0;
				pImgProcRightLine[2] = 0;
			}
			pImgProcTopLine = pImgTopLftPtr;
			pImgProcBotLine = pImgTopLftPtr + nPitch * (nDefectHeight - 1);
			for(j = 0; j < nDefectWidth; j++) {
				pImgProcTopLine[0] = 255;
				pImgProcTopLine[1] = 0;
				pImgProcTopLine[2] = 0;
				pImgProcBotLine[0] = 255;
				pImgProcBotLine[1] = 0;
				pImgProcBotLine[2] = 0;
				
				pImgProcTopLine+=3;
				pImgProcBotLine+=3;
			}
			rcDefect.DeflateRect(1,1,1,1);
			if((rcDefect.left >= rcDefect.right) || (rcDefect.top >= rcDefect.bottom))
				break;
		}
	}

	
	try {
		// Specify data destination for compression
		jpeg_stdio_dest(&cinfo, output_file);
		
		// Start compressor
		jpeg_start_compress(&cinfo, TRUE);
		
		// Process data
		pBuf = bufImageRGB;
		UCHAR** pBufPtr = &pBuf;
		int nImageWidth = bufImage.GetImgSize().cx * 3;
		while (cinfo.next_scanline < cinfo.image_height) {
			(void) jpeg_write_scanlines(&cinfo, pBufPtr, 1);
			(*pBufPtr)+=nImageWidth;
		}
	}
	catch(CException* pe) {
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		delete [] bufImageRGB;
		pe->Delete();
		AfxMessageBox("write error");
		return -1;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	delete [] bufImageRGB;

	if (output_file != stdout)
		fclose(output_file);

	dwTime = GetTickCount() - dwTime;
	CString strMsg;
	strMsg.Format("Time for Jpeg Compression (Color) = %d", dwTime);
	//AfxMessageBox(strMsg);
	return (jerr.num_warnings ? -1 : 0);
}



// Color - With Arrow
int SaveDefectsJPGFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects,
					   int nDefects,
					   int* pnArrowSide,
					   int nArrowOffset,
					   CImageBuffer* pImgArrow,
					   BYTE nQuality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * output_file;
	int i, j;
	
	DWORD dwTime = GetTickCount();

	// Initialize the JPEG compression object with default error handling
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	// Add some application-specific error messages (from cderror.h)
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;
	
	
	// Initialize JPEG parameters.
	cinfo.in_color_space = JCS_RGB; // arbitrary guess
	jpeg_set_defaults(&cinfo);

	errno_t err = fopen_s(&output_file, strFileName.GetBuffer(0), WRITE_BINARY);
    if(err || output_file == NULL)
		return FALSE;

	cinfo.image_width = bufImage.GetImgSize().cx;
	cinfo.image_height = bufImage.GetImgSize().cy;
	cinfo.input_components = 3;
	cinfo.data_precision = 8;

	// Now that we know input colorspace, fix colorspace-dependent defaults
	jpeg_default_colorspace(&cinfo);

	cinfo.X_density = (UINT16) (2925/100);
	cinfo.Y_density = (UINT16) (2925/100);
	cinfo.density_unit = 2;

	//Set JPEG Parameters
	cinfo.err->trace_level = 0;
	cinfo.dct_method = JDCT_FLOAT;
	cinfo.optimize_coding = TRUE;		// Enable entropy parm optimization
	jpeg_set_quality(&cinfo, nQuality, FALSE);		// Set quantization tables for selected quality


	CSize csImg = bufImage.GetImgSize();

	int nSizeImage = csImg.cx*csImg.cy;
	UCHAR* pBuf = bufImage.GetImgPtr();
	UCHAR* bufImageRGB = new UCHAR[abs(nSizeImage*3)];
	for(i=0, j=0; i<nSizeImage; i++, j+=3)
		memset(bufImageRGB+j, pBuf[i], 3);

	CRect rcDefect;
	int	nDefectWidth;
	int	nDefectHeight;
	BYTE	*pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int nPitch = bufImage.GetPitch()*3;
	pBuf = bufImageRGB;
	for(i=0;i<nDefects;i++) {
		if(ValidateRect(&prcDefects[i], &csImg))
			continue;
		rcDefect = prcDefects[i];
		nDefectWidth = rcDefect.Width();
		nDefectHeight = rcDefect.Height();
		pImgTopLftPtr = pBuf + rcDefect.top * nPitch + rcDefect.left*3;
		pImgProcLeftLine=pImgTopLftPtr;
		pImgProcRightLine=pImgTopLftPtr + (nDefectWidth - 1)*3;
		for(j = 0; j < nDefectHeight; j++, pImgProcLeftLine += nPitch, pImgProcRightLine += nPitch) {
			pImgProcLeftLine[0] = 255;
			pImgProcLeftLine[1] = 0;
			pImgProcLeftLine[2] = 0;
			pImgProcRightLine[0] = 255;
			pImgProcRightLine[1] = 0;
			pImgProcRightLine[2] = 0;
		}
		pImgProcTopLine = pImgTopLftPtr;
		pImgProcBotLine = pImgTopLftPtr + nPitch * (nDefectHeight - 1);
		for(j = 0; j < nDefectWidth; j++) {
			pImgProcTopLine[0] = 255;
			pImgProcTopLine[1] = 0;
			pImgProcTopLine[2] = 0;
			pImgProcBotLine[0] = 255;
			pImgProcBotLine[1] = 0;
			pImgProcBotLine[2] = 0;
			
			pImgProcTopLine+=3;
			pImgProcBotLine+=3;
		}


		CRect rcArrowRelative;
		CPoint ptOffsetOfArrow;
		CRect rcSrc;
		CRect rcOverlap;
		CRect rcArrow;
		long lIndex, lIndexTemp;
		int nArrowPitch;
		BYTE* pImgArrowPtr;
		
		rcSrc = CRect(CPoint(0,0), bufImage.GetImgSize());
		switch(pnArrowSide[i]) {
		case LEFT:
			ptOffsetOfArrow = CPoint(prcDefects[i].left - nArrowOffset - pImgArrow[LEFT].GetImgSize().cx,
				prcDefects[i].CenterPoint().y - pImgArrow[LEFT].GetImgSize().cy/2);
			rcArrowRelative = CRect(CPoint(0,0), pImgArrow[LEFT].GetImgSize());
			break;
		case RIGHT:
			ptOffsetOfArrow = CPoint(prcDefects[i].right + nArrowOffset,
				prcDefects[i].CenterPoint().y - pImgArrow[RIGHT].GetImgSize().cy/2);
			rcArrowRelative = CRect(CPoint(0,0), pImgArrow[RIGHT].GetImgSize());
			break;
		case TOP:
			ptOffsetOfArrow = CPoint(prcDefects[i].CenterPoint().x - pImgArrow[TOP].GetImgSize().cx/2,
				(prcDefects[i].top - nArrowOffset - pImgArrow[TOP].GetImgSize().cy));
			rcArrowRelative = CRect(CPoint(0,0), pImgArrow[TOP].GetImgSize());
			break;
		case BOTTOM:
			ptOffsetOfArrow = CPoint(prcDefects[i].CenterPoint().x - pImgArrow[BOTTOM].GetImgSize().cx/2,
				(prcDefects[i].bottom + nArrowOffset));
			rcArrowRelative = CRect(CPoint(0,0), pImgArrow[BOTTOM].GetImgSize());
			break;
		}
		pImgArrowPtr = pImgArrow[pnArrowSide[i]].GetImgPtr();
		nArrowPitch = pImgArrow[pnArrowSide[i]].GetPitch();
		
		rcArrow = CRect(rcArrowRelative.left + ptOffsetOfArrow.x,
			rcArrowRelative.top + ptOffsetOfArrow.y,
			rcArrowRelative.right + ptOffsetOfArrow.x,
			rcArrowRelative.bottom + ptOffsetOfArrow.y);
		rcOverlap = rcSrc;
		if(rcOverlap.left < rcArrow.left) rcOverlap.left = rcArrow.left;
		if(rcOverlap.top < rcArrow.top) rcOverlap.top = rcArrow.top;
		if(rcOverlap.right > rcArrow.right) rcOverlap.right = rcArrow.right;
		if(rcOverlap.bottom > rcArrow.bottom) rcOverlap.bottom = rcArrow.bottom;
		
		if (ValidateRect(&rcOverlap,&(bufImage.GetImgSize()))) {
			return -1;
		}
		
		
		lIndex = rcOverlap.top * nPitch;
		lIndexTemp = (rcOverlap.top - ptOffsetOfArrow.y) * nArrowPitch;
		pBuf = bufImageRGB;
		int i1, j1, j2;
		for(i1 = rcOverlap.top; i1 <rcOverlap.bottom; i1++) {
			for(j1 = rcOverlap.left, j2 = rcOverlap.left*3; j1 < rcOverlap.right; j1++, j2+=3) {
				if(pImgArrowPtr[lIndexTemp + j1 - ptOffsetOfArrow.x] == 255) {
					pBuf[lIndex + j2 + 0] = 0;
					pBuf[lIndex + j2 + 1] = 0;
					pBuf[lIndex + j2 + 2] = 255;
				}
			}
			lIndex += nPitch;
			lIndexTemp += nArrowPitch;
		}
	}



	try {
		// Specify data destination for compression
		jpeg_stdio_dest(&cinfo, output_file);
		
		// Start compressor
		jpeg_start_compress(&cinfo, TRUE);
		
		// Process data
		pBuf = bufImageRGB;
		UCHAR** pBufPtr = &pBuf;
		int nImageWidth = bufImage.GetImgSize().cx * 3;
		while (cinfo.next_scanline < cinfo.image_height) {
			(void) jpeg_write_scanlines(&cinfo, pBufPtr, 1);
			(*pBufPtr)+=nImageWidth;
		}
	}
	catch(CException* pe) {
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		delete [] bufImageRGB;
		pe->Delete();
		AfxMessageBox("write error");
		return -1;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	delete [] bufImageRGB;

	if (output_file != stdout)
		fclose(output_file);

	dwTime = GetTickCount() - dwTime;
	CString strMsg;
	strMsg.Format("Time for Jpeg Compression (Color) = %d", dwTime);
	//AfxMessageBox(strMsg);
	return (jerr.num_warnings ? -1 : 0);
}
