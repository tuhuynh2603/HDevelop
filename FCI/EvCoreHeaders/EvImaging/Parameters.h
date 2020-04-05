///Udupa; Apr'2012

#pragma once

#include "Utilities.h"
#include <stdarg.h>

enum ParmTypes {
	TINT,
	TDOUBLE,
	TENUM,
	TBOOL,
	TCOLOR
};

struct CParmInfo
{
public:
	void* pParm;
	CString strName;
	ParmTypes parmType;
	CString strSection;
	int nMin;
	int nMax;
	CString strUnit;
	CString strDescription;
	CStringArray strEnumeration;
	BOOL bVisible;
	BOOL bSuperUser;

	CParmInfo();
	CParmInfo(void* pParm, CString strName, ParmTypes parmType, CString strSection="", int nMin=0, int nMax=256, CString strUnit="", CString strDescription="", CStringArray* pstrEnumeration=NULL, BOOL bVisible=TRUE, BOOL bSuperUser=TRUE);
};

class CParameters
{
public:
	CParameters();
	~CParameters();

	CString strInspection;
	CArray<CParmInfo*, CParmInfo*> arrParmInfo;

	int LoadParameters(CIniFile *ptrIniFile);
	int SaveParameters(CIniFile *ptrIniFile);

	int m_nSize;
	void Initialize();
	BYTE* GetParameters(BYTE* pData);
	BYTE* SetParameters(BYTE* pData);

	int Add(CParmInfo* pParm);
	void Clean();
	void SetEnum(int nIndex, char* arg1);
	void SetEnum(int nIndex, char* arg1, char* arg2);
	void SetEnum(int nIndex, char* arg1, char* arg2, char* arg3);
	void SetEnum(int nIndex, char* arg1, char* arg2, char* arg3, char* arg4);
};

BYTE* GetParameterData(int nDoc, CParameters* pParameters, int nCount, int& nDataSize);
void SetParameterData(CParameters* pParameters, int nCount, BYTE* pData, int nDataSize);
