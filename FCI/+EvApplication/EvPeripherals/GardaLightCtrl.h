/// CGardaLightCtrl: GardaSoft Light Control class derived from CLightControl
/// Udupa; May 2012

#pragma once

#include "LightControl.h"

class CGardaLightCtrl : public CLightControl
{
public :
	CGardaLightCtrl();
	~CGardaLightCtrl();
	virtual int ResetSequenceStep();

protected:
	virtual int SendString(CString* pstrData);
	virtual int SendMaxLightValues(BOOL *bTrackSegments);
	virtual int SendMaxLightValue(int nChannel, int nPulseWidth);
	virtual int SendLightValue(int Channel, int nCurrent);
	virtual int SendLightValues(int nStep, int nSet, BOOL *bTrackSegments);
	virtual int SendStepCount(int nSteps);
	virtual int SaveInController();
};
