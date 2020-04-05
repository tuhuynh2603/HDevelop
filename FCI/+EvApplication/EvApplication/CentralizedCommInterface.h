/// Comm Interface
/// Udupa; Jan'2011

#pragma once

#include "CommDef.h"

#define NO_OF_PORT_CENTRALIZED 3

#define MAX_STRING_NAME 255
#define MAX_FOV				20
#define MAX_DIES			5
#define MAX_VIEWS			8

struct CentralizedHeader{
	BYTE nCommand;
	unsigned int nBufferLength;
};

struct CentralizedDefectInfo {
	int nCenterX;
	int nCenterY;
	int nTop;
	int nLeft;
	int nBottom;
	int nRight;
};

struct CentralizedDefectRegionInfo {
	int nCenterX;
	int nCenterY;
	int nTop;
	int nLeft;
	int nBottom;
	int nRight;
	int nDefectPointCount;
};

struct CentralizedDeviceConfigInfo {
	int nDevicePerX;
	int nDevicePerY;
	int nFovPerX;
	int nFovPerY;
	BOOL bHorizontalDevice;
};


struct CentralizedResult {
	int nErrorCode[MAX_DIES][MAX_FOV][MAX_VIEWS];
	int nFovPartErrorCode[MAX_DIES][MAX_FOV];
	int nPartErrorCode[MAX_DIES];
	int nDieErrorCodeStats[MAX_DIES];
};

struct CentralizedStatsInfo {
	long lTotalUnitsInspected[MAX_DIES];
	long lTotalUnitsPass[MAX_DIES];
	long lTotalUnitsFail[MAX_DIES];
	long lTotalFalseRejection[MAX_DIES];
	double dTotalYield[MAX_DIES];
};

class CCentralizedCommManager;

class CCentralizedCommInterface
{
public:
	CCentralizedCommInterface();
	~CCentralizedCommInterface();

	void Initialize();
	void Close();
	void ResetConnection();
	void LoadRecipeFromCOMM();

	BOOL m_bIsTCPServer;
	char m_strIpAddress[16];
	char m_strSubnetMask[16];
	char m_strPortNumbers[NO_OF_PORT_CENTRALIZED][5];

	int m_nControllerTimeout;
	int m_nDelayTime;//Used for interrupt connection between 1 client to Centralized PC so that another clinent can able to connect (ms)

	BOOL bInitialized;
//protected:
	CCentralizedCommManager* m_pCommManager[NO_OF_PORT_CENTRALIZED];
};
