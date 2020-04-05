/// 7856.cpp: 7856 Motion Control interface
/// Udupa; March 2013;

#ifndef _PCI_8102
#define _PCI_8102

#undef _WIN32_WINNT

#include "APS168.h"
#include "APS_Define.h"
#include "ErrorCodeDef.h"


#define AXIS_COUNT 2

extern "C" int _stdcall SetAcceleration(int nAxis, int nAcc, int nDecc);

#endif