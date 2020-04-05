///Udupa; Apr'2012; Created
///Udupa; Jan'2013; Added Persistence
///Udupa; Mar'2013; Added Serialization of Parameter data

#include "StdAfx.h"
#include "Parameters.h"


BYTE* GetParameterData(int nDoc, CParameters* pParameters, int nCount, int& nDataSize)
{
	int nSizeInt = sizeof(int);
	nDataSize = nSizeInt;
	for(int i=0; i<nCount; i++)
		nDataSize += pParameters[i].m_nSize;

	if(nDataSize <= nSizeInt)
		return NULL;

	BYTE* pData = (BYTE*) malloc(nDataSize);
	memcpy(pData, (BYTE*) (&nDoc), nSizeInt);

	BYTE* pNewData = pData + nSizeInt;
	for(int i=0; i<nCount; i++)
		pNewData = pParameters[i].GetParameters(pNewData);

	return pData;
}

void SetParameterData(CParameters* pParameters, int nCount, BYTE* pData, int nDataSize)
{
	if(pData==NULL)
		return;

	int nSize = 0;
	for(int i=0; i<nCount; i++) {
		nSize += pParameters[i].m_nSize;
		if(nSize > nDataSize)
			return;

		pData = pParameters[i].SetParameters(pData);
	}
}

CParmInfo::CParmInfo()
{
	pParm = NULL;
	strName = "";
	parmType = TINT;
	strSection = "";
	nMin = 0;
	nMax = 0;
	strUnit = "";
	strDescription = "";
	bVisible = TRUE;
	bSuperUser = TRUE;
}

CParmInfo::CParmInfo(void* pParm, CString strName, ParmTypes parmType, CString strSection, int nMin, int nMax, CString strUnit, CString strDescription, CStringArray* pstrEnumeration, BOOL bVisible, BOOL bSuperUser)
{
	CParmInfo::pParm = pParm;
	CParmInfo::strName = strName;
	CParmInfo::parmType = parmType;
	CParmInfo::strSection = strSection;
	if(pstrEnumeration != NULL) {
		for(int i=0; i<pstrEnumeration->GetCount(); i++)
			strEnumeration.Add(pstrEnumeration->GetAt(i));
	}
	CParmInfo::nMin = nMin;
	CParmInfo::nMax = nMax;
	CParmInfo::strUnit = strUnit;
	CParmInfo::strDescription = strDescription;
	CParmInfo::bVisible = bVisible;
	CParmInfo::bSuperUser = bSuperUser;
}



CParameters::CParameters(void)
{
	m_nSize = 0;
}


CParameters::~CParameters(void)
{
	int nCount = (int) arrParmInfo.GetCount();
	for(int i=0; i<nCount; i++)
		delete arrParmInfo[i];

	arrParmInfo.RemoveAll();
}

void CParameters::Clean()
{
	arrParmInfo.RemoveAll();
}

int CParameters::Add(CParmInfo* pParm)
{
	return (int) arrParmInfo.Add(pParm);
}

void CParameters::SetEnum(int nIndex, char* arg1)
{
	arrParmInfo[nIndex]->strEnumeration.RemoveAll();
	arrParmInfo[nIndex]->strEnumeration.Add(arg1);
}

void CParameters::SetEnum(int nIndex, char* arg1, char* arg2)
{
	arrParmInfo[nIndex]->strEnumeration.RemoveAll();
	arrParmInfo[nIndex]->strEnumeration.Add(arg1);
	arrParmInfo[nIndex]->strEnumeration.Add(arg2);
}

void CParameters::SetEnum(int nIndex, char* arg1, char* arg2, char* arg3)
{
	arrParmInfo[nIndex]->strEnumeration.RemoveAll();
	arrParmInfo[nIndex]->strEnumeration.Add(arg1);
	arrParmInfo[nIndex]->strEnumeration.Add(arg2);
	arrParmInfo[nIndex]->strEnumeration.Add(arg3);
}

void CParameters::SetEnum(int nIndex, char* arg1, char* arg2, char* arg3, char* arg4)
{
	arrParmInfo[nIndex]->strEnumeration.RemoveAll();
	arrParmInfo[nIndex]->strEnumeration.Add(arg1);
	arrParmInfo[nIndex]->strEnumeration.Add(arg2);
	arrParmInfo[nIndex]->strEnumeration.Add(arg3);
	arrParmInfo[nIndex]->strEnumeration.Add(arg4);
}

//void CParameters::SetEnum(int nIndex, ...)
//{
//   va_list vl;
//   va_start(vl, szTypes);

//	if(pstrEnumeration != NULL) {
//		for(int i=0; i<pstrEnumeration->GetCount(); i++)
//			arrParmInfo[nIndex].strEnumeration.Add(pstrEnumeration->GetAt(i));
//	}
//}

///Udupa; Jan'2013;
int CParameters::LoadParameters(CIniFile* pIniFile)
{
	if(!pIniFile)
		return -1;

	int nCount = (int) arrParmInfo.GetCount();
	CParmInfo* pParmInfo;
	CString strSection;

	for(int i=0; i<nCount; i++) {
		pParmInfo = arrParmInfo[i];
		if(!pParmInfo)
			continue;
		
		if(!pParmInfo->strSection.Trim().IsEmpty())
			strSection.Format("%s :: %s", strInspection, pParmInfo->strSection);
		else
			strSection = strInspection;

		if((pParmInfo->parmType==TINT) || (pParmInfo->parmType==TCOLOR) || (pParmInfo->parmType==TENUM)) {
			int* pVal = (int*) (pParmInfo->pParm);
			*pVal = pIniFile->GetInteger(strSection, pParmInfo->strName, *pVal);
		}
		else if(pParmInfo->parmType==TDOUBLE) {
			double* pVal = (double*) (pParmInfo->pParm);
			*pVal = pIniFile->GetDouble(strSection, pParmInfo->strName, *pVal);
		}
		else if(pParmInfo->parmType==TBOOL) {
			BOOL* pVal = (BOOL*) (pParmInfo->pParm);
			*pVal = pIniFile->GetBoolean(strSection, pParmInfo->strName, *pVal);
		}
	}

	return 0;
}

///Udupa; Jan'2013;
int CParameters::SaveParameters(CIniFile* pIniFile)
{
	if(!pIniFile)
		return -1;

	int nCount = (int) arrParmInfo.GetCount();
	CParmInfo* pParmInfo;
	CString strSection;

	for(int i=0; i<nCount; i++) {
		pParmInfo = arrParmInfo[i];
		if(!pParmInfo)
			continue;
		
		if(!pParmInfo->strSection.Trim().IsEmpty())
			strSection.Format("%s :: %s", strInspection, pParmInfo->strSection);
		else
			strSection = strInspection;

		if((pParmInfo->parmType==TINT) || (pParmInfo->parmType==TCOLOR) || (pParmInfo->parmType==TENUM)) {
			int* pVal = (int*) (pParmInfo->pParm);
			pIniFile->SetInteger(strSection, pParmInfo->strName, *pVal);
		}
		else if(pParmInfo->parmType==TDOUBLE) {
			double* pVal = (double*) (pParmInfo->pParm);
			pIniFile->SetDouble(strSection, pParmInfo->strName, *pVal);
		}
		else if(pParmInfo->parmType==TBOOL) {
			BOOL* pVal = (BOOL*) (pParmInfo->pParm);
			pIniFile->SetBoolean(strSection, pParmInfo->strName, *pVal);
		}
	}

	return 0;
}

void CParameters::Initialize()
{	
	int nCount = (int) arrParmInfo.GetCount();
	CParmInfo* pParmInfo;

	m_nSize = 0;
	for(int i=0; i<nCount; i++) {
		pParmInfo = arrParmInfo[i];
		if(!pParmInfo)
			continue;
		
		if((pParmInfo->parmType==TINT) || (pParmInfo->parmType==TCOLOR) || (pParmInfo->parmType==TENUM))
			m_nSize += sizeof(int);
		else if(pParmInfo->parmType==TDOUBLE)
			m_nSize += sizeof(double);
		else if(pParmInfo->parmType==TBOOL)
			m_nSize += sizeof(BOOL);
	}
}

BYTE* CParameters::GetParameters(BYTE* pData)
{
	if(pData==NULL || m_nSize<1)
		return pData;

	int nCount = (int) arrParmInfo.GetCount();
	CParmInfo* pParmInfo;
	int nSizeInt = sizeof(int);
	int nSizeDouble = sizeof(double);
	int nSizeBool = sizeof(BOOL);
	BYTE* pDest;
	for(int i=0; i<nCount; i++) {
		pParmInfo = arrParmInfo[i];
		if(!pParmInfo)
			continue;
		
		pDest = (BYTE*) pParmInfo->pParm;
		if((pParmInfo->parmType==TINT) || (pParmInfo->parmType==TCOLOR) || (pParmInfo->parmType==TENUM)) {
			memcpy(pData, pDest, nSizeInt);
			pData += nSizeInt;
		}
		else if(pParmInfo->parmType==TDOUBLE) {
			memcpy(pData, pDest, nSizeDouble);
			pData += nSizeDouble;
		}
		else if(pParmInfo->parmType==TBOOL) {
			memcpy(pData, pDest, nSizeBool);
			pData += nSizeBool;
		}
	}

	return pData;
}

BYTE* CParameters::SetParameters(BYTE* pData)
{
	if(pData==NULL)
		return NULL;

	int nCount = (int) arrParmInfo.GetCount();
	CParmInfo* pParmInfo;
	int nSizeInt = sizeof(int);
	int nSizeDouble = sizeof(double);
	int nSizeBool = sizeof(BOOL);
	BYTE* pDest;
	for(int i=0; i<nCount; i++) {
		pParmInfo = arrParmInfo[i];
		if(!pParmInfo)
			continue;
		
		if((pParmInfo->parmType==TINT) || (pParmInfo->parmType==TCOLOR) || (pParmInfo->parmType==TENUM)) {
			pDest = (BYTE*) pParmInfo->pParm;
			memcpy(pDest, pData, nSizeInt);
			pData += nSizeInt;
		}
		else if(pParmInfo->parmType==TDOUBLE) {
			pDest = (BYTE*) (pParmInfo->pParm);
			memcpy(pDest, pData, nSizeDouble);
			pData += nSizeDouble;
		}
		else if(pParmInfo->parmType==TBOOL) {
			pDest = (BYTE*) (pParmInfo->pParm);
			memcpy(pDest, pData, nSizeBool);
			pData += nSizeBool;
		}
	}

	return pData;
}
