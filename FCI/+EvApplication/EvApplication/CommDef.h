/// Comm definitions

#pragma once

enum Status
{
	INFO			= 0,//= 0x30,	 /// 0 in Character
	INFOACK			= 1,//= 0x31,	 /// 1 in Character
	START			= 2,//= 0x32,	 /// 2 in Character
	STARTACK		= 3,//= 0x33,	 /// 3 in Character
	OKTOINDEX		= 4,//= 0x34,	 /// 4 in Character
	RESULT			= 5,//= 0x35,	 /// 5 in Character
	RESULTACK		= 6,//= 0x36,	 /// 6 in Character
	STARTLOT		= 7,//= 0x37,	 /// 7 in Character
	STARTLOTACK		= 8,//= 0x38,	 /// 8 in Character
	STOPLOT			= 9,//= 0x39,	 /// 9 in Character
	STOPLOTACK		= 10,//= 0x40,	 /// 10 in Character
	RESET			= 11,//= 0x41,	 /// 11 in Character
	RESETACK		= 12,//= 0x42,	 /// 12in Character
	STOP			= 13,// 13 in Character
	WARNING			= 20,// 20 in Character
	WARNINGACK		= 21,// 21 in Character
	STOPMACHINE		= 30,// 30 in Character
	STOPMACHINEACK	= 31,// 31 in Character
	UNKNOWN			= 99,
};		

enum CentralizedStatus
{
	/*INFO			= 0,
	INFOACK			= 1,*/
	//FILE_TX						= 1,
	VERIFICATION_RESULT			= 2,
	VRESULTACK						,
	RESCAN							,
	RESCANACK						,
	SEND_IMAGE_BUFFER				,
	SEND_LIST_IMAGE_BUFFER			,
	MAPPING_RESULT_READY			,
	MAPPING_RESULT_INFO				,
	MAPPING_RESULT					,
	SEND_DEFECT_INFO				,
	SEND_MESSAGE					,
	DISCONNECT						
};
					
struct CCommMessage	
{					
	INT32 nCommand;
	INT32 nDataLength;
	char* pData;

	HANDLE hMessageEvent;
	HANDLE hHandshakeEvent;

	CCommMessage()
	{
		hMessageEvent	= CreateEventA(NULL, FALSE, FALSE, NULL);
		hHandshakeEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

		nCommand	= -1;
		nDataLength = 0;
		pData		= NULL;		
	}

	CCommMessage(int nCom, INT32 nLen=0, void* pSrc=NULL)
	{
		nCommand		= nCom;
		nDataLength		= nLen;
		pData			= (char*) pSrc;
	}
};
	
struct CMessageContents {
	INT32 nDataLength;
	char* pData;
};

enum Comm_Terminator_Char
{
	ASCII_CR			= 0xD,   /// CR in character Carriage Return
	ASCII_LF			= 0xA    /// LF in character Line Feed
};