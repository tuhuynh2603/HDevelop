#pragma once
#include <string>
#include <vector>
#define MAX_ROI 100

struct ROI_array {
	int DL_TopLeft_x[MAX_ROI] = { 0 };
	int DL_TopLeft_y[MAX_ROI] = { 0 };
	int DL_BotRight_x[MAX_ROI] = { 0 };
	int DL_BotRight_y[MAX_ROI] = { 0 };
};

typedef int (CALLBACK* INIT_DEEP_LEARNING_MODEL_DLL)(std::string modelName, bool debug);
typedef int (CALLBACK* DEEP_LEARNING_INSPECT_DLL)(std::string modelName, int nWidht, int nHeight, BYTE* bufferImage, std::string & defectName, bool debug);
typedef int (CALLBACK* DEEP_LEARNING_KILL_PROCESS_DLL)(std::string modelName, bool debug);
typedef int (CALLBACK* DEEP_LEARNING_GET_MODELS_LIST_DLL)(std::vector<std::string> &modelNameList, bool debug);
typedef int (CALLBACK* DEEP_LEARNING_FREE_MODEL_DLL)(bool debug);
typedef int (CALLBACK* DEEP_LEARNING_INSPECT_FILE_DLL)(std::string modelName, std::string imagePath, int noOfCenter, int CenterArrayX[], int CenterArrayY[], std::string & defectName, bool debug);
typedef int (CALLBACK* DEEP_LEARNING_INSPECT_FILE_ROI_DLL)(std::string modelName, std::string imagePath, int noOfROI, int TopLeftArrayX[], int TopLeftArrayY[], int BotRightArrayX[], int BotRightArrayY[], std::string & defectName, int & noOfDefectDL, struct ROI_array & DL_ROI, bool debug);

class CDeepLearningInterface
{
public:

	int m_nDeepLearningMode;
	std::vector< CString > m_DeepLearningModelList;
	BOOL m_bDLModelInspectFromFile;
	// Variables
	HINSTANCE			hDLL;				// Frame Grabber DLL Instance

	INIT_DEEP_LEARNING_MODEL_DLL			lpInitModelDLL;			// DLL InitModel(...) Function
	DEEP_LEARNING_INSPECT_DLL				lpDLInspectDLL;			// DLL DLInspect(...) Function
	DEEP_LEARNING_KILL_PROCESS_DLL			lpDLKillProcessDLL;			// DLL KillProcess(...) Function
	DEEP_LEARNING_GET_MODELS_LIST_DLL		lpDLGetModelListDLL;			// DLL getModelList(...) Function
	DEEP_LEARNING_FREE_MODEL_DLL			lpDLFreeModelDLL;			// DLL FreeDLModel(...) Function
	DEEP_LEARNING_INSPECT_FILE_DLL			lpDLInspectFileDLL;	// DLL DLInspectFile(...) Function
	DEEP_LEARNING_INSPECT_FILE_ROI_DLL			lpDLInspectFileROIDLL;	// DLL DLInspectFileROI(...) Function


	CDeepLearningInterface();
	~CDeepLearningInterface();

	int LoadDeepLearningDll(CString strDeepLearningDll);
	int InitializeModel(CString strModelName, bool debug);
	int DLInspect(CString strModelName, int nWidth, int nHeight, BYTE* imageBuffer, CString& strDefectName, bool debug);
	int KillProcess(CString strModelName, bool debug);
	int GetModelList(std::vector<CString> &modelNameList, bool debug);
	int FreeDLModel(bool debug);

	int DLInspectFile(CString strModelName, CString strImagePath, int noOfCenter, int CenterArrayX[], int CenterArrayY[], CString & defectName, bool debug);
	int DLInspectFileROI(CString strModelName, CString strImagePath, int noOfROI, int TopLeftArrayX[], int TopLeftArrayY[], int BotRightArrayX[],
						int BotRightArrayY[], CString & defectName, int & noOfDefectDL, struct ROI_array & DL_ROI, bool debug);

};