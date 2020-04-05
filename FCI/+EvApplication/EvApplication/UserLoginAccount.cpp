// UserLoginAccount.cpp: implementation of the CUserLoginAccount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Application.h"
#include "UserLoginAccount.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserLoginAccount::CUserLoginAccount()
{
	m_pInfo = &((CApplication*)AfxGetApp())->m_UserAccInfo;
}

CUserLoginAccount::~CUserLoginAccount()
{
	if(m_pInfo != NULL) {
		if(m_pInfo->UserAccount != NULL)
			delete [] m_pInfo->UserAccount;
		if(m_pInfo->strUserNames != NULL)
			delete [] m_pInfo->strUserNames;
		m_pInfo->UserAccount = NULL;
		m_pInfo->strUserNames = NULL;
	}
}

////////////////////////////////////////////////////////////////////
//Implementation
////////////////////////////////////////////////////////////////////

int CUserLoginAccount::SaveUserLoginInfo(CString strDirectoryName, CString strFileName, LoginInfo *UserInfo, int *nUser)
{
	CIniFile iniFile;
	CString strDataFileName;
	
	strDataFileName = strDirectoryName + "\\" + strFileName + ".lgn";
	iniFile.SetPath(strDataFileName);
	iniFile.ReadFile();
	SetUserLoginInfo(&iniFile, UserInfo, nUser);
	iniFile.WriteFile();
	return 0;
}

int CUserLoginAccount::LoadUserLoginInfo(CString strDirectoryName, CString strFileName, LoginInfo *UserInfo, int *nUser)
{
	CIniFile iniFile;
	CString strDataFileName;
	
	strDataFileName = strDirectoryName + "\\" + strFileName + ".lgn";
	iniFile.SetPath(strDataFileName);
	
	if (iniFile.ReadFile()) {
		// File Exist
		GetUserLoginInfo(&iniFile, UserInfo, nUser);
	}
	else {
		// File Not Found
		int nIndex = (*nUser) + 1;
		SetUserLoginInfo(&iniFile, UserInfo, &nIndex);
		iniFile.WriteFile();
	}
	
	return 0;
}

void CUserLoginAccount::SetUserLoginInfo(CIniFile *ptrIniFile, LoginInfo *UserInfo, int *nUser)
{
	CString strSec;
	
	ptrIniFile->SetInteger("NUM USER","NoOfUsers", *nUser);	
	
	strSec.Format("User%d", *nUser);
	ptrIniFile->SetCString(strSec, "Name", UserInfo->strUsername);
	ptrIniFile->SetCString(strSec, "Level", UserInfo->strAccLevel);
	ptrIniFile->SetCString(strSec, "Pswd", UserInfo->strPassword);
}

void CUserLoginAccount::GetUserLoginInfo(CIniFile *ptrIniFile, LoginInfo *UserInfo, int *nUser)
{
	CString strSec;
	
	strSec.Format("User%d", (*nUser)+1);
	UserInfo->strUsername = ptrIniFile->GetCString(strSec, "Name", UserInfo->strUsername);
	UserInfo->strAccLevel = ptrIniFile->GetCString(strSec, "Level", UserInfo->strAccLevel);
	UserInfo->strPassword = ptrIniFile->GetCString(strSec, "Pswd", UserInfo->strPassword);
}

CString CUserLoginAccount::EncodePassWord(CString *strPwd)
{
	CString strEcpwd;
	int i, nLength;
	unsigned char *ptrChar = NULL;
	nLength = strPwd->GetLength();


	
	ptrChar = new unsigned char[nLength]; 
	
	for(i = 0; i < nLength; i++) {
		ptrChar[i] = strPwd->GetAt(i);
		
		ptrChar[i]-=15;
		
		
	}
	
	strEcpwd.Empty();
	
	for(i = 0; i < nLength; i++) {
		strEcpwd += ptrChar[i];
	}
	
	if(ptrChar != NULL)
		delete [] ptrChar;
	ptrChar = NULL;
	return strEcpwd; 
}


CString CUserLoginAccount::DecodePassWord(CString *strEcpwd)
{
	CString strDcpwd;
	int i, nLength;
	unsigned char *ptrChar = NULL;
	nLength = strEcpwd->GetLength();

	
	
	ptrChar = new unsigned char[nLength]; 
	
	for(i = 0; i < nLength; i++) {
		ptrChar[i] = strEcpwd->GetAt(i);
		
		ptrChar[i]+=15;
		
			}
		
		strDcpwd.Empty();
		
		for(i = 0; i < nLength; i++) {
			strDcpwd += ptrChar[i];
		}

	if(ptrChar != NULL)
		delete [] ptrChar;
	ptrChar = NULL;
		return strDcpwd;	
}


int CUserLoginAccount::LoadUserAccountData()

{
	int k;
	CString strDataFileName;
	CIniFile iniFile;
	CString strFileName = "LogAccount";
	
	CApplication* pApp = (CApplication*) AfxGetApp();
	strDataFileName = pApp->m_strMainCfgDir + "\\" + strFileName + ".lgn";
	iniFile.SetPath(strDataFileName);
	iniFile.ReadFile();
	
	m_pInfo->nNumofUsers = GetNumOfUsers(&iniFile);
	
	k = -1;
	LoadUserLoginInfo(pApp->m_strMainCfgDir, strFileName, &m_pInfo->DefaultUser, &k);
	
	if(m_pInfo->nNumofUsers) {
		if(m_pInfo->UserAccount != NULL)
			delete [] m_pInfo->UserAccount;
		if(m_pInfo->strUserNames != NULL)
			delete [] m_pInfo->strUserNames;
		m_pInfo->UserAccount = NULL;
		m_pInfo->strUserNames = NULL;

		m_pInfo->UserAccount = new LoginInfo[m_pInfo->nNumofUsers];
		m_pInfo->strUserNames = new CString[m_pInfo->nNumofUsers];

		for(k = 0; k < m_pInfo->nNumofUsers; k++) {
			LoadUserLoginInfo(pApp->m_strMainCfgDir, strFileName, &m_pInfo->UserAccount[k], &k);
			m_pInfo->strUserNames[k] = m_pInfo->UserAccount[k].strUsername;
		}
	}

	return 0;
}

int CUserLoginAccount::GetNumOfUsers(CIniFile *ptrIniFile)
{
	int nUsers;
	
	nUsers = ptrIniFile->GetInteger("NUM USER","NoOfUsers", 0);
	
	return nUsers;
}

int CUserLoginAccount::SetNumOfUsers(CIniFile *ptrIniFile, int* nUsers )
{
	ptrIniFile->SetInteger("NUM USER","NoOfUsers", *nUsers);	
	
	return 0;
}

