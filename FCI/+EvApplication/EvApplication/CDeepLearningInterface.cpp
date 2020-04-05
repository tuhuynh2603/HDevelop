#include "stdafx.h"
#include "CDeepLearningInterface.h"
#include "Utilities.h"

CDeepLearningInterface::CDeepLearningInterface()
{
	m_nDeepLearningMode = 0;
	hDLL = 0;

	lpInitModelDLL = 0;
	lpDLInspectDLL = 0;
	lpDLKillProcessDLL = 0;
	lpDLGetModelListDLL = 0;
	lpDLFreeModelDLL = 0;
	lpDLInspectFileDLL = 0;
	lpDLInspectFileROIDLL = 0;
}


CDeepLearningInterface::~CDeepLearningInterface()
{

}

int CDeepLearningInterface::LoadDeepLearningDll(CString strDeepLearningDll)
{
	DWORD dwErr = 0;

	OutputDebugLog("Registering Deep Learning Dll ...");

	if (strDeepLearningDll.CompareNoCase("NONE") != 0) {
		hDLL = AfxLoadLibrary(strDeepLearningDll);

		if (hDLL == NULL) {
			CString str;
			str.Format("Error Loading Deep Learning library %s", strDeepLearningDll);
			AfxMessageBox(str, MB_OK | MB_ICONERROR);
			return -1;
		}

		lpInitModelDLL = (INIT_DEEP_LEARNING_MODEL_DLL)GetProcAddress(hDLL, "InitModel");
		if (lpInitModelDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll InitModel() Function:%d", GetLastError());

		lpDLInspectDLL = (DEEP_LEARNING_INSPECT_DLL)GetProcAddress(hDLL, "DLInspect");
		if (lpDLInspectDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll DLInspect(...) Function:%d", GetLastError());

		lpDLKillProcessDLL = (DEEP_LEARNING_KILL_PROCESS_DLL)GetProcAddress(hDLL, "KillProcess");
		if (lpDLKillProcessDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll KillProcess(...) Function:%d", GetLastError());

		lpDLGetModelListDLL = (DEEP_LEARNING_GET_MODELS_LIST_DLL)GetProcAddress(hDLL, "getModelList");
		if (lpDLGetModelListDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll getModelList(...) Function:%d", GetLastError());

		lpDLFreeModelDLL = (DEEP_LEARNING_FREE_MODEL_DLL)GetProcAddress(hDLL, "FreeDLModel");
		if (lpDLFreeModelDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll FreeDLModel(...) Function:%d", GetLastError());

		lpDLInspectFileDLL = (DEEP_LEARNING_INSPECT_FILE_DLL)GetProcAddress(hDLL, "DLInspectFile");
		if (lpDLInspectFileDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll DLInspectFile(...) Function:%d", GetLastError());

		lpDLInspectFileROIDLL = (DEEP_LEARNING_INSPECT_FILE_ROI_DLL)GetProcAddress(hDLL, "DLInspectFileROI");
		if (lpDLInspectFileROIDLL == NULL)
			OutputDebugLog("Fail to register Deep Learning Dll DLInspectFileROI(...) Function:%d", GetLastError());
	}
	else {
		OutputDebugLog("No Deep Learning Dll");
	}

	return 0;
}

int CDeepLearningInterface::InitializeModel(CString strModelName, bool debug)
{
	int nError = 0;

	if (lpInitModelDLL) {
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString(strModelName);
		// construct a std::string using the LPCSTR input
		std::string strStdModelName(pszConvertedAnsiString);

		nError = lpInitModelDLL(strStdModelName, debug);
	}

	return nError;
}

int CDeepLearningInterface::DLInspect(CString strModelName, int nWidth, int nHeight, BYTE* imageBuffer, CString& strDefectName, bool debug)
{
	int nError = 0;

	if (lpDLInspectDLL) {
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString(strModelName);
		// construct a std::string using the LPCSTR input
		std::string strStdModelName(pszConvertedAnsiString);
		std::string strStdDefectName;

		nError = lpDLInspectDLL(strStdModelName, nWidth, nHeight, imageBuffer, strStdDefectName, debug);
		strDefectName = strStdDefectName.c_str();
	}

	return nError;
}

int CDeepLearningInterface::DLInspectFile(CString strModelName, CString strImagePath, int noOfCenter, int CenterArrayX[], int CenterArrayY[], CString & defectName, bool debug)
{
	int nError = 0;

	if (lpDLInspectFileDLL) {
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString(strModelName);
		// construct a std::string using the LPCSTR input
		std::string strStdModelName(pszConvertedAnsiString);

		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString2(strImagePath);
		// construct a std::string using the LPCSTR input
		std::string strStdImagePath(pszConvertedAnsiString2);


		std::string strStdDefectName;

		nError = lpDLInspectFileDLL(strStdModelName, strStdImagePath, noOfCenter, CenterArrayX, CenterArrayY, strStdDefectName, debug);
		defectName = strStdDefectName.c_str();
	}

	return nError;
}

int CDeepLearningInterface::DLInspectFileROI(CString strModelName, CString strImagePath, int noOfROI, int TopLeftArrayX[], int TopLeftArrayY[], int BotRightArrayX[], int BotRightArrayY[], CString & defectName, int & noOfDefectDL, struct ROI_array & DL_ROI, bool debug)
{
	int nError = 0;

	if (lpDLInspectFileROIDLL) {
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString(strModelName);
		// construct a std::string using the LPCSTR input
		std::string strStdModelName(pszConvertedAnsiString);

		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString2(strImagePath);
		// construct a std::string using the LPCSTR input
		std::string strStdImagePath(pszConvertedAnsiString2);


		std::string strStdDefectName;

		nError = lpDLInspectFileROIDLL(strStdModelName, strStdImagePath, noOfROI, TopLeftArrayX, TopLeftArrayY, BotRightArrayX, BotRightArrayY,
			strStdDefectName, noOfDefectDL, DL_ROI, debug);
		defectName = strStdDefectName.c_str();
	}

	return nError;
}

int CDeepLearningInterface::KillProcess(CString strModelName, bool debug)
{
	int nError = 0;

	if (lpDLKillProcessDLL) {
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString(strModelName);
		// construct a std::string using the LPCSTR input
		std::string strStdModelName(pszConvertedAnsiString);
		std::string strStdDefectName;

		nError = lpDLKillProcessDLL(strStdDefectName, debug);
	}

	return nError;
}

int CDeepLearningInterface::GetModelList(std::vector<CString> &modelNameList, bool debug)
{
	int nError = 0;

	if (lpDLGetModelListDLL) {
		std::vector<std::string> strStdModelList;
		nError = lpDLGetModelListDLL(strStdModelList, debug);
		if (!nError) {
			for (int i = 0; i < strStdModelList.size(); i++) {
				modelNameList.push_back(CString(strStdModelList[i].c_str()));
			}
		}
	}

	return nError;
}

int CDeepLearningInterface::FreeDLModel(bool debug)
{
	int nError = 0;

	if (lpDLFreeModelDLL) {
		nError = lpDLFreeModelDLL(debug);
	}

	return nError;
}