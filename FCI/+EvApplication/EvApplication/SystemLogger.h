
class CApplication;

#pragma once

class CSystemLoggingInfo
{
	
public:
            CSystemLoggingInfo();
			~CSystemLoggingInfo();
	   
			  
	        SYSTEMTIME Stime;
		
	        CString str_user_name,str_acess_level,DayFolder;
			BOOL bPrintHead;
			CApplication* pApp;
				

public  :void SaveLoginDetails(SYSTEMTIME* Stime,
				                CString str_user_name="",
								CString str_acess_level="");
		
            void    SaveLogoutDetails(SYSTEMTIME* Stime);

			void    SaveParameterChanges(SYSTEMTIME *Stime,
									CString strParmName,
									CString strParmSetName,
									double NewValue,
									double OldValue,
									int nTrackIdx);
			void SaveLightSettingChanges(SYSTEMTIME *Stime,
									int* NewValue,
									int* OldValue,
									int nTrackIdx);

			void SaveStatisticsResetChanges(SYSTEMTIME *Stime,
										CString strParmName,
										CString strParmSetName,
										double NewValue,
										double OldValue,
										int nTrackIdx);
		void SaveStatisticsCounterReset(SYSTEMTIME *Stime);
		void SaveLotChanges(SYSTEMTIME *Stime,
									CString strParmName,
									CString strParmSetName,
									double NewValue,
									bool flag);
		void SaveCameraParameterChanges(SYSTEMTIME *Stime,
										CString strParmName,
										CString strParmSetName,
										double NewValue,
										double OldValue,
										int nTrackIdx);
		void SaveParameterChangesAllTracks(SYSTEMTIME *Stime,
										CString strParmName,
										CString strParmSetName,
										double NewValue,
										double OldValue,
										int nTrackIdx);
			 
			void LogModeChange(SYSTEMTIME* Stime, bool bOnline);

			void ConfigChanged(SYSTEMTIME *Stime,
								CString OldConfig,
								CString NewConfig,
								int nFlag);

			CString CSystemLoggingInfo::filepath();//fo storing login details in seprate directory
            CString CSystemLoggingInfo::loginname();//to find the loginname on to the OS
			void takepermission(CString filename);
			void putfilepermission(CString filename);
private:

	CString GetLoggingFilePath(SYSTEMTIME *Stime);
		
};
