#pragma once

class CParameterCore
{
public:
	CParameterCore(CString strInspName, CString strShortInspName, int nErrorCode);
	~CParameterCore();

	BOOL bEnable;
	double dInspTime;
	CString strInspName;
	CString strShortInspName;

	int nErrorCode;
	int nError;
	int nIndex;
	COLORREF colorDefect;
};
