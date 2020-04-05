// IniFile.h: interface for the CIniFile class.
// Written by: Adam Clauss
// Email: pandcc3@comwerx.net
// You may use this class/code as you wish in your programs.  Feel free to distribute it, and
// email suggested changes to me.
//////////////////////////////////////////////////////////////////////

#pragma once

#include <afxtempl.h>
#include <iostream>
#include "MathDef.h"

using namespace std;

class CIniFile  
{
	//all private variables
private:

	//stores pathname of ini file to read/write
	CString path;
	
	//all keys are of this time
	struct key
	{
		//list of values in key
		CArray<CString, CString> values; 

		//corresponding list of value names
		CArray<CString, CString> names;
	};

	//list of keys in ini
	CArray<key, key> keys; 

	//corresponding list of keynames
	CArray<CString, CString> names; 
	
	
	//all private functions
private:
	//overloaded to take CString
	istream & getline( istream & is, CString & str );

	//returns index of specified value, in the specified key, or -1 if not found
	int FindValue(int nKeyNum, CString strValueName);

	//returns index of specified key, or -1 if not found
	int FindKey(CString strKeyName);


	//public variables
public:

	//will contain error info if one occurs
	//ended up not using much, just in ReadFile and GetValue
	CString error;


	//public functions
public:
	BOOL IsInputOK();
	BOOL m_bFileReadSuccessful;
	BOOL IsModified();

	double GetDouble(CString strKeyName, CString strValueName, double dValue);
	int GetInteger(CString strKeyName, CString strValueName, int nValue);
	int GetLong(CString strKeyName, CString strValueName, long lValue);
	CString GetCString(CString strKeyName, CString strValueName, CString strValue);
	BOOL GetBoolean(CString strKeyName, CString strValueName, BOOL bValue);
	CPoint GetCPoint(CString strKeyName, CString strValueName, CPoint pt);
	CPoint2D GetCPoint2D(CString strKeyName, CString strValueName, CPoint2D pt2D);
	CPoint3D GetCPoint3D(CString strKeyName, CString strValueName, CPoint3D pt3D);
	CRect GetCRect(CString strKeyName, CString strValueName, CRect rect);
	CLine2D GetCLine2D(CString strKeyName, CString strValueName, CLine2D ln2D);
	CRectangle2D GetCRect2D(CString strKeyName, CString strValueName, CRectangle2D rc2D);


	//sets value of [strKeyName] strValueName =.
	//specify the optional paramter as false (0) if you do not want it to create
	//the key if it doesn't exist. Returns true if data entered, false otherwise
	//overloaded to accept CString, int, and double
	BOOL SetCString(CString strKeyName, CString strValueName, CString strValue, BOOL bCreate = 1);
	BOOL SetInteger(CString strKeyName, CString strValueName, int nValue, BOOL bCreate = 1);
	BOOL SetLong(CString strKeyName, CString strValueName, long lValue, BOOL bCreate = 1);
	BOOL SetDouble(CString strKeyName, CString strValueName, double dValue, BOOL bCreate = 1);
	BOOL SetBoolean(CString strKeyName, CString strValueName, BOOL bValue, BOOL bCreate = TRUE);
	BOOL SetCPoint(CString strKeyName, CString strValueName, CPoint pt, BOOL bCreate = TRUE);
	BOOL SetCPoint2D(CString strKeyName, CString strValueName, CPoint2D pt2D, BOOL bCreate = TRUE);
	BOOL SetCRect(CString strKeyName, CString strValueName, CRect rect, BOOL bCreate = TRUE);
	BOOL SetCLine2D(CString strKeyName, CString strValueName, CLine2D ln2D, BOOL bCreate = TRUE);
	BOOL SetCPoint3D(CString strKeyName, CString strValueName, CPoint3D pt3D, BOOL bCreate = TRUE);
	BOOL SetCRect2D(CString strKeyName, CString strValueName, CRectangle2D rc2D,BOOL bCreate = TRUE);

	
	//default constructor
	CIniFile();

	//constructor, can specify pathname here instead of using SetPath later
	CIniFile(CString strIniPath);

	//default destructor
	virtual ~CIniFile();

	//sets path of ini file to read and write from
	void SetPath(CString strNewPath);

	//reads ini file specified using CIniFile::SetPath()
	//returns true if successful, false otherwise
	BOOL ReadFile();

	//writes data stored in class to ini file
	void WriteFile(); 

	//deletes all stored ini data
	void Reset();

	//returns number of keys currently in the ini
	int GetNumKeys();

	//returns number of values stored for specified key
	int GetNumValues(CString strKeyName);
/*
	//gets value of [strKeyName] strValueName = 
	//overloaded to return CString, int, and double,
	//returns "", or 0 if key/value not found.  Sets error member to show problem
	CString GetValue(CString strKeyName, CString strValueName); 
	int GetValueI(CString strKeyName, CString strValueName); 
	double GetValueF(CString strKeyName, CString strValueName);
*/
	//deletes specified value
	//returns true if value existed and deleted, false otherwise
	BOOL DeleteValue(CString strKeyName, CString strValueName);

	//deletes specified key and all values contained within
	//returns true if key existed and deleted, false otherwise
	BOOL DeleteKey(CString strKeyName);
protected:
	BOOL m_bModified;
};
