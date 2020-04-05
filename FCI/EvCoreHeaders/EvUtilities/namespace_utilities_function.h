#pragma once

#include <string>
#include <list>
#include <fstream>
#include <ctime>
#include <atlstr.h>
#include <sstream>
#include <Windows.h>

namespace 
{
	bool IsCalibrationFolder(CString * pstrFolderName)
	{
		//Calibration folder names is reserved by application
		//and can't be used

		BOOL bCalibrationFolder = TRUE;

		CString strCalibrationFolderName;
		strCalibrationFolderName.Format("Calibration");

		bCalibrationFolder &= (pstrFolderName->CompareNoCase(strCalibrationFolderName) != 0);

		return !bCalibrationFolder;
	}

	bool IsImageFolder(CString * pstrFolderName)
	{
		//Calibration folder names is reserved by application
		//and can't be used

		BOOL bImageFolder = TRUE;

		CString strImageFolderName;
		strImageFolderName.Format("Images");

		bImageFolder &= (pstrFolderName->CompareNoCase(strImageFolderName) != 0);

		return !bImageFolder;
	}

	BOOL IsPackageFolder(CString * pstrFolderName)
	{
		//Every folder contains two system directories (this is from DOS system)
		BOOL bPackageFolder = TRUE;

		CString strSystemFolder[2];
		strSystemFolder[0].Format("."); //Current Directory
		strSystemFolder[1].Format(".."); //Parent Directory

		for(int i=0;i<2;i++) {
			bPackageFolder &= (pstrFolderName->CompareNoCase(strSystemFolder[i]) != 0);
		}

		bPackageFolder &= !IsCalibrationFolder(pstrFolderName);
		bPackageFolder &= !IsImageFolder(pstrFolderName);

		return bPackageFolder;
	}

	bool isLotInformationFile(std::string currentLotId, std::string detectedLotId)
	{
		return strcmp( currentLotId.c_str(), detectedLotId.c_str() ) == 0;
	}

	std::list < std::string > readWordsFromFile( const std::string & fileName )
	{
		std::list < std::string > word;

		std::ifstream file (fileName);

		if (file.is_open()) {
			std::string data;

			while( file >> data ) {
				word.push_back( data );
			}

			file.close();
		}

		return word;
	};

	void writeWordsToFile( const std::string & fileName, const std::list < std::string > & data )
	{
		std::list < std::string > word;

		std::ofstream file(fileName);

		if(file.is_open()) {
			for( std::list < std::string >::const_iterator d = data.begin(); d != data.end(); ++d )
				file << *d << " ";

			file.close();
		}
	};

	std::string getTime()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%I:%M %p", timeinfo);
		puts(buffer);

		return buffer;
	}

	std::string getTime2()
	{
	time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%I%M%S", timeinfo);
		puts(buffer);

		return buffer;
	
	}

	std::string getDate()		//DD/MM/YY
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%d/%m/%Y", timeinfo);
		puts(buffer);

		return buffer;
	}

	std::string getDate2()		//YYMMDD
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%Y%m%d", timeinfo);
		puts(buffer);

		return buffer;
	}
	std::string getDate3()		//YYMMDD
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%d%m%Y", timeinfo);
		puts(buffer);

		return buffer;
	}

	std::string getDetailedTime()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%d %b %Y, %I:%M %p", timeinfo);
		puts(buffer);

		return buffer;
	}

	std::list< std::string > getConfigurationFileNameList(CString directoryPath, bool newFileStructure = false)
	{
		std::list< std::string > configurationNameList;

		CString strFindName,	// Full path and filename with wildcard ".*" for file searching
			    strFndName,		// Found FileName
				folderPath;		// Folders path

		HANDLE hFile;
		WIN32_FIND_DATA FindFileData;

		//New configuration file structure will separate to 3 category (Engineering, Production, Qualification)
		//During production, it will only use the "Production" folder configuration to perform inspection and other task
		folderPath = newFileStructure ? directoryPath + "\\Production Config\\*.*" : directoryPath + "\\*.*"; 

		if((hFile = FindFirstFile(folderPath, &FindFileData)) != INVALID_HANDLE_VALUE) {
			do {
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					CString fileName = FindFileData.cFileName;
					if(IsPackageFolder( &fileName )) 
						configurationNameList.push_back( CString(FindFileData.cFileName).GetString() );
				}
			} while (FindNextFile(hFile, &FindFileData));
		}

		return configurationNameList;
	}

	bool isLogicalDriveExist(std::string logicalDrive)
	{
		bool driveExist = true;

		UINT result = GetDriveType( logicalDrive.c_str() );
		switch( result ) {
			case DRIVE_UNKNOWN: 
			case DRIVE_NO_ROOT_DIR:
				driveExist = false;
				break;
			case DRIVE_REMOVABLE:
			case DRIVE_FIXED:
			case DRIVE_REMOTE: 
			case DRIVE_CDROM:
			case DRIVE_RAMDISK:
				driveExist = true;
				break;
			default :
				driveExist = false;
		}

		return driveExist;
	}

	std::list < std::string > getFileNamesInFolders( std::string folder, std::string fileExtension, std::string currentLotId )
	{
		HANDLE file;
		WIN32_FIND_DATA FindFileData;

		std::list < std::string > fileName;
		std::ostringstream wildCard;

		wildCard << folder << "\\*." << fileExtension;

		if( (file = FindFirstFile( wildCard.str().c_str() , &FindFileData )) != INVALID_HANDLE_VALUE ) {

			do {
				if(!isLotInformationFile(currentLotId, FindFileData.cFileName))
					fileName.push_back( FindFileData.cFileName );

			} while( FindNextFile(file, &FindFileData) );

		}

		FindClose(file);

		return fileName;
	}

	bool CreateDirectoryTree(LPCTSTR szPathTree, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL)
	{
		bool bSuccess = false;
		const BOOL bCD = CreateDirectory(szPathTree, lpSecurityAttributes);
		DWORD dwLastError = 0;
		if(!bCD){
			dwLastError = GetLastError();
		}else{
			return true;
		}
		switch(dwLastError){
		case ERROR_ALREADY_EXISTS:
			bSuccess = true;
			break;
		case ERROR_PATH_NOT_FOUND:
			{
				TCHAR szPrev[MAX_PATH] = {0};
				LPCTSTR szLast = _tcsrchr(szPathTree,'\\');
				_tcsnccpy(szPrev,szPathTree,(int)(szLast-szPathTree));
				if(CreateDirectoryTree(szPrev,lpSecurityAttributes)){
					bSuccess = CreateDirectory(szPathTree,lpSecurityAttributes)!=0;
					if(!bSuccess){
						bSuccess = (GetLastError()==ERROR_ALREADY_EXISTS);
					}
				}else{
					bSuccess = false;
				}
			}
			break;
		default:
			bSuccess = false;
			break;
		}

		return bSuccess;
	}

	void appendMessage( CString & report, CString & message )
	{
		report.Append( message );
		report.Append( "\n" );
	}

	//helper function to convert value to CString
	template<typename T>
	CString getValueInString(T value, int precision = 2)
	{
		CString stringValue = "";

		if(typeid(value) == typeid(int)) {
			stringValue.Format("%d", value);
		}
		else if(typeid(value) == typeid(unsigned int)) {
			stringValue.Format("%u", value);
		}
		else if(typeid(value) == typeid(bool) || typeid(value) == typeid(BOOL)) {
			stringValue = value ? "true" : "false";
		}
		else if(typeid(value) == typeid(float) || typeid(value) == typeid(double)) {
			//Assume the precision up to 4 decimal places or 
			//defaults precision decimals place for doubles
			if(precision == 0)
				stringValue.Format("%.f", value);
			else if(precision == 1)
				stringValue.Format("%.1f", value);
			else if(precision == 2)
				stringValue.Format("%.2f", value);
			else if(precision == 3)
				stringValue.Format("%.3f", value);
			else if(precision == 4)
				stringValue.Format("%.4f", value);
		}

		return stringValue;
	}
};