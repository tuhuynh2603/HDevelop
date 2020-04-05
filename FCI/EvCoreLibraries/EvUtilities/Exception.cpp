

#include "stdafx.h"
#include "Exception.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CExceptionInfo::CExceptionInfo()
{
	strExceptionFunction.Empty();
}

CExceptionInfo::~CExceptionInfo()
{
	Clean();
}

void CExceptionInfo::Clean()
{
	strExceptionFunction.Empty();
}

int CExceptionInfo::Inspection(char* pcharExceptionFunction)
{
	if(strExceptionFunction == "") {
		strExceptionFunction.Format(pcharExceptionFunction);
	}
	return 0;
}
