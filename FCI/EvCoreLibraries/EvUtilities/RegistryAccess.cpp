
#include "stdafx.h"
#include "RegistryAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetRegistryEntry(CString strKeyPath, CString strKeyName, CString strDefValue)
{
	HKEY hControlKey;
	char buf[256];
	DWORD dwType;
	DWORD dwLength;
	DWORD dwReturn;
	DWORD dwDisposition;
	CString strStoredValue;

	if (RegCreateKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hControlKey,
		&dwDisposition)!=ERROR_SUCCESS) {
		::AfxMessageBox("ERROR in Creating/Reading Registry",MB_OK|MB_ICONSTOP|MB_ICONERROR|MB_ICONHAND);
	}
		
	dwType=REG_SZ;
	dwLength=256;
	if ((dwReturn=RegQueryValueEx(hControlKey,
		strKeyName,
		NULL,
		&dwType,
		(unsigned char *)buf,
		&dwLength))!=ERROR_SUCCESS) {
		strStoredValue=strDefValue;
	}
	else {
	 	strStoredValue=buf;
	}
	RegCloseKey(hControlKey);
	return strStoredValue;
}

int GetRegistryEntry(CString strKeyPath, CString strKeyName, int nDefValue)
{
	HKEY hControlKey;
	DWORD dwType;
	DWORD dwLength;
	DWORD dwReturn;
	DWORD dwDisposition;
	DWORD dwValue;
	int nStoredValue;

	if (RegCreateKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hControlKey,
		&dwDisposition)!=ERROR_SUCCESS) {
		::AfxMessageBox("ERROR in Creating/Reading Registry",MB_OK|MB_ICONSTOP|MB_ICONERROR|MB_ICONHAND);
	}
		
	dwType=REG_DWORD;
	dwLength=4;
	if ((dwReturn=RegQueryValueEx(hControlKey,
		strKeyName,
		NULL,
		&dwType,
		(unsigned char *)&dwValue,
		&dwLength))!=ERROR_SUCCESS) {
		nStoredValue=nDefValue;
	}
	else {
	 	nStoredValue=dwValue;
	}
	RegCloseKey(hControlKey);
	return nStoredValue;
}

void GetRegistryEntry(CString strKeyPath, CString strKeyName, BYTE* pValue, int& nLength)
{
	HKEY hControlKey;
	BYTE buf[2048];
	DWORD dwType;
	DWORD dwLength;
	DWORD dwReturn;
	DWORD dwDisposition;

	if(RegCreateKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hControlKey,
		&dwDisposition)!=ERROR_SUCCESS)
	{
		::AfxMessageBox("ERROR in Creating/Reading Registry",MB_OK|MB_ICONSTOP|MB_ICONERROR|MB_ICONHAND);
	}
		
	dwType = REG_BINARY;
	dwLength = 2048;
	if((dwReturn=RegQueryValueEx(hControlKey,
		strKeyName,
		NULL,
		&dwType,
		buf,
		&dwLength)) == ERROR_SUCCESS)
	{
		memcpy(pValue, buf, dwLength);
		nLength = dwLength;
	}
	RegCloseKey(hControlKey);
}

void PutRegistryEntry(CString strKeyPath, CString strKeyName, CString strValue)
{
	HKEY hInspectionKey;
	DWORD dwDisposition;

	RegCreateKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hInspectionKey,
		&dwDisposition);
	
	if (RegSetValueEx(hInspectionKey,
		strKeyName,
		NULL,
		REG_SZ,
		(unsigned char *)strValue.GetBuffer(0),
		strValue.GetLength()+1)!=ERROR_SUCCESS) {
		::AfxMessageBox("Error Setting Value");
	}
	RegCloseKey(hInspectionKey);
}

void PutRegistryEntry(CString strKeyPath, CString strKeyName, int nValue)
{
	HKEY hInspectionKey;
	DWORD dwDisposition;
	DWORD dValue;

	dValue = (DWORD)nValue;

	RegCreateKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hInspectionKey,
		&dwDisposition);
	
	if (RegSetValueEx(hInspectionKey,
		strKeyName,
		NULL,
		REG_DWORD,
		(unsigned char *)&dValue,
		4)!=ERROR_SUCCESS) {
		::AfxMessageBox("Error Setting Value");
	}
	RegCloseKey(hInspectionKey);
}


void PutRegistryEntry(CString strKeyPath, CString strKeyName, BYTE* pValue, int nLength)
{
	HKEY hInspectionKey;
	DWORD dwDisposition;

	RegCreateKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hInspectionKey,
		&dwDisposition);
	
	if (RegSetValueEx(hInspectionKey,
		strKeyName,
		NULL,
		REG_BINARY,
		pValue,
		nLength)!=ERROR_SUCCESS) {
		::AfxMessageBox("Error Setting Value");
	}
	RegCloseKey(hInspectionKey);
}

///Udupa; 23Jan'2013
void DeleteRegistryEntry(CString strKeyPath, CString strKeyName)
{
	HKEY hInspectionKey;
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
		strKeyPath,
		0,
		KEY_ALL_ACCESS,
		&hInspectionKey);
	
	if(lResult==ERROR_SUCCESS) {
		RegDeleteValue(hInspectionKey, strKeyName);
		RegCloseKey(hInspectionKey);
	}
}
