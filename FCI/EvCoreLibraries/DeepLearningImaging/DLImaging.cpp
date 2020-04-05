
#include "stdafx.h"
#include "DLImaging.h"
#include "BitmapFunctions.h"
#include <string>
#include <atlstr.h>

int _stdcall GetDefectEmbeddedInfo(char * pcFileName, int** items, int* itemCount)
{
	std::vector<CRect> prcDefectBoundingBox;
	std::vector<CPoint> pptDefectCenter;
	CImageBuffer imageTemp;
	imageTemp.Allocate(CSize(2048, 2048), true, false);

	std::string strFileName(pcFileName);
	CString cstrFilename(strFileName.c_str());

	int nError = LoadDefectInfoBMPEmbeddedFile(cstrFilename, prcDefectBoundingBox, pptDefectCenter);

	//int nError = LoadGrayScaleBMPEmbeddedFile(cstrFilename, imageTemp, prcDefectBoundingBox, pptDefectCenter, true);
	if (nError) {
		return -1;
	}
	else {
		std::vector<int> id_x_y;
		for (int i = 0; i < pptDefectCenter.size(); i++) {
			id_x_y.push_back(pptDefectCenter[i].x);
			id_x_y.push_back(pptDefectCenter[i].y);

			id_x_y.push_back(prcDefectBoundingBox[i].TopLeft().x);
			id_x_y.push_back(prcDefectBoundingBox[i].TopLeft().y);

			id_x_y.push_back(prcDefectBoundingBox[i].BottomRight().x);
			id_x_y.push_back(prcDefectBoundingBox[i].BottomRight().y);
		}

		*itemCount = id_x_y.size();
		auto size = (*itemCount) * sizeof(int);

		*items = static_cast<int*>(CoTaskMemAlloc(size));
		memcpy(*items, id_x_y.data(), size);
		return 0;
	}

}