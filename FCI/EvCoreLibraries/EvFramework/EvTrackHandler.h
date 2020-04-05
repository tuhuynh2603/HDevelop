/// EvTrackHandler.h : interface of the CEvTrackHandler class
/// Udupa; August'2012

#pragma once

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

enum {
	CAMERA_EXPOSURE,
	CAMERA_GAIN,
	CAMERA_BLACKLEVEL
};

struct CFailImageStack;

class DECLSPEC_WINDOW_IF CEvTrackHandler
{
public:
	CEvTrackHandler();
	~CEvTrackHandler();
	int m_nTrack;

	virtual void SetCamActive(BOOL bActive)=0;

	virtual int GetFailedImage(int nIndex, CFailImageStack*& pFailure) = 0;
};
