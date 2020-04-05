/// Comm Interface
/// Udupa; Jan'2011
/// Modified for 2D-Wire.. E Santosh Jul'12

#pragma once

#include "CommDef.h"

#define NO_OF_PORT	1

class CCommManager;

class CCommInterface
{
public:
	CCommInterface();
	~CCommInterface();

	void Initialize();
	void Close();
	void ResetConnection();
	void LoadRecipeFromCOMM();

	BOOL m_bIsTCPServer;
	char m_strIpAddress[16];
	char m_strSubnetMask[16];
	char m_strPortNumbers[NO_OF_PORT][5];

	int m_nControllerTimeout;
	BOOL bInitialized;
//protected:
	CCommManager* m_pCommManager[2];
};
