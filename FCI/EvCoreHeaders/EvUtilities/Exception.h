#pragma once

#include "stdafx.h"

class CExceptionInfo
{
public:
	CExceptionInfo();
	~CExceptionInfo();
	void Clean();
	int Inspection(char* pcharExceptionFunction);

	CString strExceptionFunction;

};
