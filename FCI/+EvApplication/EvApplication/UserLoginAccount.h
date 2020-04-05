#pragma once

typedef struct UserLogin
{
	CString strUsername;
	CString strPassword;
	CString strAccLevel;
} LoginInfo;

struct UserAccountInfo
{
	CString m_strCurUserName;
	LoginInfo DefaultUser;
	bool m_bAccessLevel;
	int m_nCurrentAccessLevel;
	CString m_strCurrentAccessLevel;
	int nNumofUsers;
	LoginInfo *UserAccount;
	CString m_strNewPassword;
	CString *strUserNames;
	time_t tmStartLogoutTimeHr;
	int m_nLogoutTime;

	//int m_nLogTimeExt;
};

class CUserLoginAccount  
{
public:
	CUserLoginAccount();
	virtual ~CUserLoginAccount();

	UserAccountInfo* m_pInfo;

	int SaveUserLoginInfo(CString strDirectoryName, CString strFileName, LoginInfo *UserInfo, int *nUser);
	int LoadUserLoginInfo(CString strDirectoryName, CString strFileName, LoginInfo *UserInfo, int *nUser);
	void SetUserLoginInfo(CIniFile *ptrIniFile, LoginInfo *UserInfo, int *nUser);
	void GetUserLoginInfo(CIniFile *ptrIniFile, LoginInfo *UserInfo, int *nUser);
	CString EncodePassWord(CString *strPwd);
	CString DecodePassWord(CString *strEcpwd);
	int LoadUserAccountData();
	int SetNumOfUsers(CIniFile *ptrIniFile, int* nUsers );
	int GetNumOfUsers(CIniFile *ptrIniFile);

};
