/// CInspectionHandlerCore: Vision Inspection manager base class
/// Udupa; Feb'2015

#include "StdAfx.h"
#include "InspectionHandlerCore.h"


CInspectionHandlerCore::CInspectionHandlerCore()
{
}

CInspectionHandlerCore::~CInspectionHandlerCore()
{
	CleanUp();
}

void CInspectionHandlerCore::CleanUp()
{
	if(!m_Parameters.IsEmpty()) {
		for(int i=0; i<m_Parameters.GetCount(); i++)
			delete m_Parameters[i];
		m_Parameters.RemoveAll();
	}
}

void CInspectionHandlerCore::Initialize(int nTrack)
{
}

void CInspectionHandlerCore::CloseDocument()
{
}

int CInspectionHandlerCore::LoadTeachParameters(CString strRecipeFolder, CString strRecipeFile)
{
	CIniFile iniFile;
	CString strDataFileName;

	strDataFileName = strRecipeFolder + "\\" + strRecipeFile + ".dat";
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		GetTeachParameters(&iniFile);
	}
	else {
		OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		SetTeachParameters(&iniFile);
		iniFile.WriteFile();
	}
	return 0;
}

int CInspectionHandlerCore::SaveTeachParameters(CString strRecipeFolder, CString strRecipeFile)
{
	CIniFile iniFile;
	CString strDataFileName;

	strDataFileName = strRecipeFolder + "\\" + strRecipeFile + ".dat";
	iniFile.SetPath(strDataFileName);
	iniFile.ReadFile();
	SetTeachParameters(&iniFile);
	iniFile.WriteFile();
	return 0;
}

int CInspectionHandlerCore::LoadInspParameters(CString strRecipeFolder, CString strRecipeFile)
{
	CIniFile iniFile;
	CString strDataFileName;

	strDataFileName = strRecipeFolder + "\\" + strRecipeFile + ".dat";
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		GetInspParameters(&iniFile);
	}
	else {
		OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		SetInspParameters(&iniFile, 0);
		iniFile.WriteFile();
	}

	return 0;
}

int CInspectionHandlerCore::SaveInspParameters(CString strRecipeFolder, CString strRecipeFile, int nCamNo)
{
	CIniFile iniFile;
	CString strDataFileName;

	strDataFileName = strRecipeFolder + "\\" + strRecipeFile + ".dat";
	iniFile.SetPath(strDataFileName);
	iniFile.ReadFile();
	SetInspParameters(&iniFile, nCamNo);
	iniFile.WriteFile();

	return 0;
}

int CInspectionHandlerCore::GetInspParameters(CIniFile *ptrIniFile)
{
	for(int i=0; i<m_Parameters.GetCount(); i++)
		m_Parameters[i]->LoadParameters(ptrIniFile);

	return 0;
}

int CInspectionHandlerCore::SetInspParameters(CIniFile *ptrIniFile, int nCamNo)
{
	for(int i=0; i<m_Parameters.GetCount(); i++)
		m_Parameters[i]->SaveParameters(ptrIniFile);

	return 0;
}
