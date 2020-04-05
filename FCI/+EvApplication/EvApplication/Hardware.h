#ifndef _HARDWARE_ACCESS
#define _HARDWARE_ACCESS

class CAppDoc;
class CDialogPane;

int Grab(CAppDoc *pDoc, BOOL bDisplay);
void GetImageFilePaths(CString strStartFolder, CStringArray& arrayFilePaths, CStringArray& arrayFileFolders, BOOL bParseSub, CString strChar);
void GetImageFolderPaths(CString strStartFolder, CStringArray& arrayFileFolders);

// Inspect Track Threads
UINT TrackLiveAll(LPVOID pParam);
UINT TrackLiveImage(LPVOID pParam);
UINT TrackInspectCycle(LPVOID pParam);
UINT TrackAutoRunThread(LPVOID pParam);

UINT TrackLightCalibrationLiveImage(LPVOID pParam);

UINT TrackCommandThread(LPVOID pParam);

UINT InspectSeqThread(LPVOID pParam);
UINT VisionResetThread(LPVOID pParam);
UINT PollingThread(LPVOID pParam);

UINT TrackInspectSeqThread(LPVOID pParam);
UINT DocInspectCycle(LPVOID pParam);
//DWORD WINAPI TrackInspectImages(LPVOID pParam);
UINT TrackInspectImages(LPVOID pParam);

UINT TrackUpdateThread(LPVOID pParam);

//UINT TrackImageUpdate(LPVOID pParam);

UINT TrackUpdateImage(LPVOID pParam);//
UINT RefreshLightThread(LPVOID pParam);//// added to update refersh light
UINT RefreshMotionStop(LPVOID pParam);
UINT RefreshMotionStart(LPVOID pParam);

UINT TrackLiveImageSingle(LPVOID pParam);
UINT LogInspectionData(LPVOID pParam);

UINT ResetMotor(LPVOID pParam);
UINT StartScanPosition(LPVOID pParam);

UINT TrackAutoFocusing(LPVOID pParam);


#endif //HARDWARE_ACCESS

