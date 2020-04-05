#pragma once

void PutRegistryEntry(CString strKeyPath, CString strKeyName, CString strValue);
void PutRegistryEntry(CString strKeyPath, CString strKeyName, int nValue);
void PutRegistryEntry(CString strKeyPath, CString strKeyName, BYTE* pValue, int nLength);


CString GetRegistryEntry(CString strKeyPath, CString strKeyName, CString strDefValue);
int GetRegistryEntry(CString strKeyPath, CString strKeyName, int nDefValue);
void GetRegistryEntry(CString strKeyPath, CString strKeyName, BYTE* pStoredValue, int& nLength);
void DeleteRegistryEntry(CString strKeyPath, CString strKeyName);
