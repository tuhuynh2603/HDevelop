#include "StdAfx.h"
#include "ParameterCore.h"


CParameterCore::CParameterCore(CString strInspName, CString strShortInspName, int nErrorCode)
{
	bEnable = TRUE;
	dInspTime = 0;

	CParameterCore::strInspName = strInspName;
	CParameterCore::strShortInspName = strShortInspName;
	CParameterCore::nErrorCode = nErrorCode;
	nIndex = abs(nErrorCode);
}

CParameterCore::~CParameterCore()
{
}
