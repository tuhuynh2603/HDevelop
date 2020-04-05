//////////////////implementation file for SystemLogger/////////
///////////////////
#include "stdafx.h"
#include "afx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "SystemLogger.h"
#include "MainFrame.h"
#include "AppDoc.h"
#include <direct.h>

//#include "RegAcces.h"
//#include "Inifile.h"
//#include "MaskText.h"//for masking 
//#include "doscopy.h"//if you call the direct dos command then the dos prompt will be displayed for a moment -to avoid -used

//bool alreadycreated=false;
extern bool closing;
CString parameter_File;

CSystemLoggingInfo::CSystemLoggingInfo()
	{
	   
       
	   Stime.wDay=0;
	   Stime.wHour=0;
	   Stime.wMilliseconds=0;
	   Stime.wMinute=0;
	   Stime.wDayOfWeek=NULL;
	   Stime.wMonth=0;
	   Stime.wSecond=0;
	   Stime.wYear=0;
	   str_user_name="";
	   str_acess_level="";
	   DayFolder="";
	   pApp = (CApplication*) AfxGetApp();
	  }
CString CSystemLoggingInfo::loginname()
	{
	 CString login_name="";
     login_name=GetRegistryEntry("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer","Logon User Name",login_name);
     return(login_name);     
	}
void CSystemLoggingInfo::takepermission(CString filename)
	{

	   
           /* CString str1,login_name="";
			login_name=loginname();
	        str1.Format("oShell.Run \"cacls %s /t /g %s:f\"\n",filename,login_name);
			//domain\username 
            _mkdir("C:\\Permission Setting");
	        FILE *file1=fopen("C:\\Permission Setting\\filepermission.vbs","w+");
			fprintf(file1,"Option explicit\n");
			fprintf(file1,"dim oShell,WrLines,FileSys\n");
			fprintf(file1,"set oShell= Wscript.CreateObject(\"WScript.Shell\")\n");
			fprintf(file1,str1);
			fprintf(file1,"WScript.Sleep 100\n");
			fprintf(file1,"oShell.Sendkeys \"y~\"\n");
			fclose(file1);
            ShellExecute(NULL,"open","C:\\Permission Setting\\filepermission.vbs",NULL,NULL,SW_HIDE);    
		   Sleep(1000);*/
		 
	}

void CSystemLoggingInfo::putfilepermission(CString filename)
    {
	       /* CString str1,login_name="";
			login_name=loginname();
            str1.Format("oShell.Run \"cacls %s /t /g %s:r\"\n",filename,login_name);
            FILE *file1=fopen("C:\\Permission Setting\\filepermission.vbs","w+");
			fprintf(file1,"Option explicit\n");
			fprintf(file1,"dim oShell,WrLines,FileSys\n");
			fprintf(file1,"set oShell= Wscript.CreateObject(\"WScript.Shell\")\n");
			fprintf(file1,str1);
			fprintf(file1,"WScript.Sleep 100\n");
			fprintf(file1,"oShell.Sendkeys \"y~\"\n");
			//fprintf(file1,"Wscript.Quit\n");
			fclose(file1);
			//system("C:\\Permission Setting\\filepermission.vbs");
			//system("exit");
			ShellExecute(NULL,"open","C:\\Permission Setting\\filepermission.vbs",NULL,NULL,SW_HIDE);    	*/
			
           

    }


CString CSystemLoggingInfo::filepath()//for contact lens only
	{
     CString file_path ="";
	file_path=GetRegistryEntry(READ_REGPATH + "Preferences", "Folder: LoginDetails", file_path);
	 if(file_path.IsEmpty())
		 {
	       PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder: LoginDetails", "C:\\LoginDetails");
           file_path="C:\\LoginDetails";
		 }
	 return(file_path);
	}///for creating login details in a seprate folder 

CSystemLoggingInfo::~CSystemLoggingInfo(){}
	   
void CSystemLoggingInfo::SaveLoginDetails(SYSTEMTIME* Stime,
										  CString strUserName,
										   CString strAcessLevel)
	{
  

	  CString strTime, strDate, LogFileName;
	  LogFileName = GetLoggingFilePath(Stime);
	  strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	  strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	  FILE* file;
	  fopen_s(&file, LogFileName,"a+");
	  if(!file)
		  return;

	///////////////////////////////////
   	fprintf(file,"\n\n            ______________________________________________________________\n");
	fprintf(file,"\n            =====================[ LOGIN DETAILS ]=====================\n");
	fprintf(file,"            ______________________________________________________________\n\n");
    fprintf(file,"\tUSER\t\t::\t%s\n",strUserName);
	fprintf(file,"\tLEVEL\t\t::\t%s\n",strAcessLevel);
	fprintf(file,"\tDATE\t\t::\t%s\n",strDate);
	fprintf(file,"\tTIME\t\t::\t%s\n\n",strTime);
	
	fclose(file);
	//alreadycreated=true;

  
	strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);

	}
void CSystemLoggingInfo::SaveLogoutDetails(SYSTEMTIME* Stime)
	{
	   
        
	CString strTime, LogFileName;
	  LogFileName = GetLoggingFilePath(Stime);
	  strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	  FILE* file;
	  fopen_s(&file, LogFileName,"a+");
	  if(!file)
	    return;

	////////////////////////////////
	  fprintf(file,"\n\n            ______________________________________________________________\n");
	  fprintf(file,"\n\tLOG OUT TIME\t::\t%s\n",strTime);
	  fprintf(file,"            ______________________________________________________________\n");
	  fclose(file);
	}

void CSystemLoggingInfo::SaveParameterChanges(SYSTEMTIME *Stime,
										    CString strParmName,
											CString strParmSetName,
											double NewValue,
											double OldValue,
											int nTrackIdx)
{
	CAppDoc* pDoc = pApp->m_pTracks[nTrackIdx].m_pDoc[0];
	CString strTime, LogFileName,strDate;

	LogFileName = GetLoggingFilePath(Stime);

	strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	
	
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;
	
	if(bPrintHead){
		
		pApp->m_strConfigModifiedDate.Format("%s::%s",strDate,strTime); 	

		fprintf(file,"\n\n            ______________________________________________________________\n");
		fprintf(file,"\n\t%s      TRACK %d\tTIME::%s\tCONFIG :: %s\n",strParmSetName,nTrackIdx,strTime,pApp->m_strConfigFile);
		fprintf(file,"            ______________________________________________________________\n\n");
		fprintf(file,"\t%-45s OLD VALUE\tNEW VALUE\n\n","PARAMETER");
		fprintf(file,"\t%s\t%.3f\t\t%.3f\n",strParmName,OldValue,NewValue);

		///Udupa; TODO
		//pApp->SaveConfigModifyDate();
		//pApp->LoadConfigModifyDate(0);
        
		bPrintHead = false;
	}
	
	else
		fprintf(file,"\t%s\t%.3f\t\t%.3f\n",strParmName,OldValue,NewValue);
	
	fclose(file);
}



void CSystemLoggingInfo::SaveParameterChangesAllTracks(SYSTEMTIME *Stime,
										    CString strParmName,
											CString strParmSetName,
											double NewValue,
											double OldValue,
											int nTrackIdx)
{
	CAppDoc* pDoc = pApp->m_pTracks[nTrackIdx].m_pDoc[0];
	CString strTime, LogFileName,strDate;

	LogFileName = GetLoggingFilePath(Stime);

	strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	
	
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;
	
	if(bPrintHead){
		
		pApp->m_strConfigModifiedDate.Format("%s::%s",strDate,strTime); 	

		fprintf(file,"\n\n            ______________________________________________________________\n");
		fprintf(file,"\n\t%s           \tTIME::%s\tCONFIG :: %s\n",strParmSetName,strTime,pApp->m_strConfigFile);
		fprintf(file,"            ______________________________________________________________\n\n");
		fprintf(file,"\t%-45s TRACK OLD VALUE  NEW VALUE\n\n","PARAMETER");
		fprintf(file,"\t%s\t    %d\t%.3f\t  %.3f\n",strParmName,nTrackIdx,OldValue,NewValue);
		//fprintf(file,"\t%-45s%d\t%.3f     \t  %.3f\n",strParmName,nTrackIdx,OldValue,NewValue);

		///Udupa; TODO
		//pApp->SaveConfigModifyDate();
		//pApp->LoadConfigModifyDate(0);
        
		bPrintHead = false;
	}
	
	else
		fprintf(file,"\t%s\t    %d\t%.3f\t  %.3f\n",strParmName,nTrackIdx,OldValue,NewValue);
		//fprintf(file,"\t%-45s%d\t%.3f     \t  %.3f\n",strParmName,nTrackIdx,OldValue,NewValue);
	
	fclose(file);
}



void CSystemLoggingInfo :: LogModeChange( SYSTEMTIME* Stime, bool bOnline)

{
	  CString strTime, LogFileName;
	  LogFileName = GetLoggingFilePath(Stime);
	  strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	  FILE* file;
	  fopen_s(&file, LogFileName,"a+");
	  if(!file)
		  return;
	  if(bOnline)
		  fprintf(file,"\n\tMODE\t\t::        PRODUCTIONMODE      ::\t%s\n",strTime);
	  
	  else
		  fprintf(file,"\n\tMODE\t\t::\tOFFLINE \t\t::\t%s\n",strTime);
	  
	  fclose(file);
}

void CSystemLoggingInfo :: SaveLightSettingChanges(SYSTEMTIME *Stime,
												   int* NewValue,
												   int* OldValue,
												   int nTrackIdx)
{ 
	  CAppDoc* pDoc = pApp->m_pTracks[nTrackIdx].m_pDoc[0];
	  CString strTime, LogFileName;
	  bPrintHead = true;
	  LogFileName = GetLoggingFilePath(Stime);
	  strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	  FILE* file;
	  fopen_s(&file, LogFileName,"a+");
	  if(!file)
		  return;

	  for(int i = 0;i<NO_OF_SEGMENTS;i++){
		  if(*NewValue != *OldValue){
			  
			  if(bPrintHead){
				  
				  
				  fprintf(file,"\n\n            ______________________________________________________________\n");
				  fprintf(file,"\tLIGHT SETTINGS\tTRACK%d\t\tTIME::%s\tCONFIG :: %s\n",nTrackIdx,strTime,pApp->m_strConfigFile);
				  fprintf(file,"            ______________________________________________________________\n");
				  fprintf(file,"\t%-45sOLD VALUE\t\t   NEW VALUE\n\n","LIGHT BIT");
				  fprintf(file,"\t%-50d\t%-3d\t\t\t%-3d\n\n",i+1,*OldValue,*NewValue);
				  bPrintHead = false;
				  
			  }
			  else{
				  
				  fprintf(file,"\t%-50d\t%-3d\t\t\t%-3d\n\n",i+1,*OldValue,*NewValue);
				  
			  }
		  }
		  NewValue++;
		  OldValue++;
		  
	  }
	  //fprintf(file,"----------------------------------------------------------------------------------------------------\n\n");
	  fclose(file);

}

void CSystemLoggingInfo::ConfigChanged(SYSTEMTIME *Stime,
										  CString OldConfig,
										  CString NewConfig,
										  int nFlag)
{
	CString strTime, LogFileName;
	
	LogFileName = GetLoggingFilePath(Stime);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;

	fprintf(file,"\n\n            ______________________________________________________________\n");
	fprintf(file,"\tCONFIGURATION CHANGE\t\t\t\tTIME::%s\n",strTime);
	fprintf(file,"            ______________________________________________________________\n");
	
	if(1==nFlag)
		fprintf(file,"\tChanged Configuration  From [%s] To [%s]\n",OldConfig,NewConfig);
	if(2==nFlag)
		fprintf(file,"\tDeleted Configuration  : [%s]",OldConfig);
	if(3==nFlag)
		fprintf(file,"\tCreated New Configuration : [%s]",NewConfig);

	fprintf(file,"\n            ______________________________________________________________\n");			  
	fclose(file);

}

CString CSystemLoggingInfo::GetLoggingFilePath(SYSTEMTIME *Stime)

{
	  
	  CString strTime,LogFileName,monthfolder;
	  
      monthfolder.Format("%02u_%02u",Stime->wYear, Stime->wMonth);

	  CString strSystemLoggingDir = pApp->m_strSystemLoggingDir;
      _mkdir(strSystemLoggingDir);
	  strSystemLoggingDir += "\\"+monthfolder;
	  _mkdir(strSystemLoggingDir);
	  
	  LogFileName.Format("%02u_%02u_%02u.log",Stime->wYear,Stime->wMonth,Stime->wDay);
	  
	  LogFileName = strSystemLoggingDir +"\\"+LogFileName;

	  return LogFileName;

}

void CSystemLoggingInfo::SaveStatisticsResetChanges(SYSTEMTIME *Stime,
										    CString strParmName,
											CString strParmSetName,
											double NewValue,
											double OldValue,
											int nTrackIdx)
{
	CAppDoc* pDoc = pApp->m_pTracks[nTrackIdx].m_pDoc[0];
	CString strTime, LogFileName,strDate;

	LogFileName = GetLoggingFilePath(Stime);

	strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	
	
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;
	
	if(bPrintHead){
		
		pApp->m_strConfigModifiedDate.Format("%s::%s",strDate,strTime); 	

		fprintf(file,"\n\n            ______________________________________________________________\n");
		fprintf(file,"\t%s\t\tTIME::%s\tCONFIG :: %s\n",strParmSetName,strTime,pApp->m_strConfigFile);
		fprintf(file,"            ______________________________________________________________\n");
		fprintf(file,"\t%-50sOLD VALUE\t\tNEW VALUE\n\n","PARAMETER");
		fprintf(file,"\t%-50s%.3f\t\t\t%.3f\n",strParmName,OldValue,NewValue);

	//	pApp->SaveConfigModifyDate();
	//	pApp->LoadConfigModifyDate(0);
        
		bPrintHead = false;
	}
	
	else
		fprintf(file,"\t%-50s%.3f\t\t%.3f\n",strParmName,OldValue,NewValue);
	
	fclose(file);
}

void CSystemLoggingInfo::SaveStatisticsCounterReset(SYSTEMTIME *Stime)
{
	CString strTime, LogFileName;//,strDate;
	
	LogFileName = GetLoggingFilePath(Stime);
	
	//strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	
	
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;
	
	fprintf(file,"\n\n\tRESET\t\t::\tSTATISTICS COUNTERS \t::\t%s\n\n",strTime);
	
	fclose(file);
}

void CSystemLoggingInfo::SaveLotChanges(SYSTEMTIME *Stime,
										CString strParmName,
										CString strParmSetName,
										double NewValue,
										bool flag)
{
	CString strTime, LogFileName;//,strDate;
	
	LogFileName = GetLoggingFilePath(Stime);
	
	//strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	
	
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;

	if(flag)
		fprintf(file,"\n\tOPEN-LOT\t::\t%s \t::\t%s\n",strParmSetName,strTime);
	else
		fprintf(file,"\n\tCLOSE-LOT\t::\t%s \t::\t%s\n",strParmSetName,strTime);
	
	fclose(file);
}


void CSystemLoggingInfo::SaveCameraParameterChanges(SYSTEMTIME *Stime,
										    CString strParmName,
											CString strParmSetName,
											double NewValue,
											double OldValue,
											int nTrackIdx)
{
	CAppDoc* pDoc = pApp->m_pTracks[nTrackIdx].m_pDoc[0];
	CString strTime, LogFileName,strDate;

	LogFileName = GetLoggingFilePath(Stime);

	strDate.Format("%02u-%02u-%02u",Stime->wDay,Stime->wMonth,Stime->wYear);
	strTime.Format("%02u:%02u:%02u",Stime->wHour,Stime->wMinute,Stime->wSecond);
	
	
	FILE* file;
	fopen_s(&file, LogFileName,"a+");
	if(!file)
		return;

	if(bPrintHead){
		
		pApp->m_strConfigModifiedDate.Format("%s::%s",strDate,strTime); 	

		fprintf(file,"\n\n            ______________________________________________________________\n");
		fprintf(file,"\n\t%s   TRACK %d\tTIME::%s\tCONFIG :: %s\n",strParmSetName,nTrackIdx,strTime,pApp->m_strConfigFile);
		fprintf(file,"            ______________________________________________________________\n\n");
		fprintf(file,"\t%-45s OLD VALUE\tNEW VALUE\n\n","PARAMETER");
		fprintf(file,"\t%-50s\t%.2f\t\t%.2f\n",strParmName,OldValue,NewValue);

		///Udupa; TODO
		//pApp->SaveConfigModifyDate();
		//pApp->LoadConfigModifyDate(0);
        
		bPrintHead = false;
	}
	
	else
		fprintf(file,"\t% s\t\t%d\t\t%d\n",strParmName,(int)OldValue,(int)NewValue);
	
	fclose(file);
}