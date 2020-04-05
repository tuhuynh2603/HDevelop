// IniFile.cpp: implementation of the CIniFile class.
// Written by: Adam Clauss
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////

//default constructor
CIniFile::CIniFile()
{
	m_bModified = FALSE;
	m_bFileReadSuccessful = FALSE;
}

//constructor, can specify pathname here instead of using SetPath later
CIniFile::CIniFile(CString strIniPath)
{
	path = strIniPath;
	m_bModified = FALSE;
}

//default destructor
CIniFile::~CIniFile()
{

}

/////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////

//sets path of ini file to read and write from
void CIniFile::SetPath(CString strNewPath)
{
	path = strNewPath;
}

//reads ini file specified using CIniFile::SetPath()
//returns true if successful, false otherwise
BOOL CIniFile::ReadFile()
{
	CFile file;
	CFileStatus status;
	CString strReadInfo;
	CString strKeyName,
			strValueName,
			strValue;

	m_bFileReadSuccessful = FALSE;
	if (!file.GetStatus(path,status))
		return 0;
	ifstream inifile;
	inifile.open(path);
	if (inifile.fail())	{
		error = "Unable to open ini file.";
		return 0;
	}
	while (getline(inifile,strReadInfo)) {
		if (strReadInfo != "") {
			 //if a section heading
			if (strReadInfo[0] == '[' && strReadInfo[strReadInfo.GetLength()-1] == ']')	{
				strKeyName = strReadInfo;
				strKeyName.TrimLeft('[');
				strKeyName.TrimRight(']');
			}
			else {
				//if a value
				strValueName = strReadInfo.Left(strReadInfo.Find("="));
				strValue = strReadInfo.Right(strReadInfo.GetLength()-strValueName.GetLength()-1);
				SetCString(strKeyName,strValueName,strValue);
			}
		}
	}
	m_bModified = FALSE;
	m_bFileReadSuccessful = TRUE;
	inifile.close();
	return 1;
}

//writes data stored in class to ini file
void CIniFile::WriteFile()
{
	ofstream inifile;
	inifile.open(path);
	for (int nKeyNum=0;nKeyNum<=names.GetUpperBound();nKeyNum++) {
		if (keys[nKeyNum].names.GetSize() != 0)	{
			inifile << '[' << names[nKeyNum] << ']' << endl;
			for (int nValueNum=0;nValueNum<=keys[nKeyNum].names.GetUpperBound();nValueNum++)
			{
				inifile << keys[nKeyNum].names[nValueNum] << "=" << keys[nKeyNum].values[nValueNum];
				if (nValueNum != keys[nKeyNum].names.GetUpperBound())
					inifile << endl;
				else
					if (nKeyNum < names.GetSize())
						inifile << endl;
			}
			if (nKeyNum < names.GetSize())
				inifile << endl;
		}
	}
	m_bModified = FALSE;
	inifile.close();
}

//deletes all stored ini data
void CIniFile::Reset()
{
	keys.SetSize(0);
	names.SetSize(0);
}

//returns number of keys currently in the ini
int CIniFile::GetNumKeys()
{
	return (int) keys.GetSize();
}

//returns number of values stored for specified key, or -1 if key found
int CIniFile::GetNumValues(CString strKeyName)
{
	int nKeyNum = FindKey(strKeyName);
	if (nKeyNum == -1)
		return -1;
	else
		return (int) keys[nKeyNum].names.GetSize();
}
/*
//gets value of [strKeyName] strValueName = 
//overloaded to return CString, int, and double
CString CIniFile::GetValue(CString strKeyName, CString strValueName)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		return "";
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		return "";
	}
	return keys[nKeyNum].values[nValueNum];
}

//gets value of [strKeyName] strValueName = 
//overloaded to return CString, int, and double
int CIniFile::GetValueI(CString strKeyName, CString strValueName)
{
	return atoi(GetValue(strKeyName,strValueName));
}

//gets value of [strKeyName] strValueName = 
//overloaded to return CString, int, and double
double CIniFile::GetValueF(CString strKeyName, CString strValueName)
{
	return atof(GetValue(strKeyName, strValueName));
}
*/


//sets value of [strKeyName] strValueName =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
BOOL CIniFile::SetCString(CString strKeyName, CString strValueName, CString strValue, BOOL bCreate)
{
	int nKeyNum = FindKey(strKeyName);
	int nValueNum = 0;

	//find key
	if (nKeyNum == -1) {
		//if key doesn't exist
		if (!bCreate) //and user does not want to create it,
			return 0; //stop entering this key
		names.SetSize(names.GetSize()+1);
		keys.SetSize(keys.GetSize()+1);
		nKeyNum = (int) names.GetSize() - 1;
		names[nKeyNum] = strKeyName;
	}

	//find value
	nValueNum = FindValue(nKeyNum,strValueName);
	if (nValueNum == -1) {
		if (!bCreate)
			return 0;
		keys[nKeyNum].names.SetSize(keys[nKeyNum].names.GetSize()+1);
		keys[nKeyNum].values.SetSize(keys[nKeyNum].names.GetSize()+1);
		nValueNum = (int) keys[nKeyNum].names.GetSize() - 1;
		keys[nKeyNum].names[nValueNum] = strValueName;
	}
	keys[nKeyNum].values[nValueNum] = strValue;
	m_bModified = TRUE;
	return 1;
}

//sets value of [strKeyName] strValueName =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
BOOL CIniFile::SetInteger(CString strKeyName, CString strValueName, int nValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format("%d",nValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

//sets value of [strKeyName] strValueName =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
BOOL CIniFile::SetLong(CString strKeyName, CString strValueName, long lValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format("%ld",lValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

//sets value of [strKeyName] strValueName =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
BOOL CIniFile::SetDouble(CString strKeyName, CString strValueName, double dValue, BOOL bCreate)
{
	CString strTemp;
	strTemp.Format("%e",dValue);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

//sets value of [strKeyName] strValueName =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
BOOL CIniFile::SetBoolean(CString strKeyName, CString strValueName, BOOL bValue, BOOL bCreate)
{
	CString strTemp;
	if (bValue)
		strTemp.Format("%d",1);
	else
		strTemp.Format("%d",0);
	return SetCString(strKeyName, strValueName, strTemp, bCreate);
}

BOOL CIniFile::SetCPoint(CString strKeyName, CString strValueName, CPoint pt, BOOL bCreate)
{
	CString strPtX = strValueName + "X";
	CString	strPtY = strValueName + "Y";

	CString strTemp;
	strTemp.Format("%d",pt.x);
	if (!SetCString(strKeyName, strPtX, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%d",pt.y);
	if (!SetCString(strKeyName, strPtY, strTemp, bCreate))
		return FALSE;

	return TRUE;
}

BOOL CIniFile::SetCPoint2D(CString strKeyName, CString strValueName, CPoint2D pt2D, BOOL bCreate)
{
	CString strPtX = strValueName + "X";
	CString	strPtY = strValueName + "Y";

	CString strTemp;
	strTemp.Format("%e",pt2D.x);
	if (!SetCString(strKeyName, strPtX, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%e",pt2D.y);
	if (!SetCString(strKeyName, strPtY, strTemp, bCreate))
		return FALSE;

	return TRUE;
}

BOOL CIniFile::SetCPoint3D(CString strKeyName, CString strValueName, CPoint3D pt3D, BOOL bCreate)
{
	CString strPtX = strValueName + "X";
	CString	strPtY = strValueName + "Y";
	CString	strPtZ = strValueName + "Z";

	CString strTemp;
	strTemp.Format("%e",pt3D.x);
	if (!SetCString(strKeyName, strPtX, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%e",pt3D.y);
	if (!SetCString(strKeyName, strPtY, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%e",pt3D.z);
	if (!SetCString(strKeyName, strPtZ, strTemp, bCreate))
		return FALSE;

	return TRUE;
}


BOOL CIniFile::SetCLine2D(CString strKeyName, CString strValueName, CLine2D ln2D, BOOL bCreate)
{
	CPoint2D Position;   //The line passes through the point Position
	CVector2D Direction; //The direction vector of the line
	BOOL m_bInValid;	 // Status of This Line :  1: Invalid;  0: Valid

	CString strPtX = strValueName + "Pt2DX";
	CString	strPtY = strValueName + "Pt2DY";
	CString strVt2DX = strValueName + "Vt2DX";
	CString	strVt2DY = strValueName + "Vt2DY";
	CString strInValid =  strValueName + "Invalid";

	Position = ln2D.GetPosition();
	Direction = ln2D.GetDirection();
	m_bInValid = ln2D.IsInValid();

	CString strTemp;
	strTemp.Format("%e",Position.x);
	if (!SetCString(strKeyName, strPtX, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%e",Position.y);
	if (!SetCString(strKeyName, strPtY, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%e",Direction.x);
	if (!SetCString(strKeyName, strVt2DX, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%e",Direction.y);
	if (!SetCString(strKeyName, strVt2DY, strTemp, bCreate))
		return FALSE;

	if (m_bInValid)
		strTemp.Format("%d",1);
	else
		strTemp.Format("%d",0);
	if (!SetCString(strKeyName, strInValid, strTemp, bCreate))
		return FALSE;

	return TRUE;
}


BOOL CIniFile::SetCRect(CString strKeyName, CString strValueName, CRect rect, BOOL bCreate)
{
	CString strRectTop = strValueName + "Top";
	CString strRectBot = strValueName + "Bot";
	CString strRectLft = strValueName + "Lft";
	CString strRectRht = strValueName + "Rht";

	CString strTemp;
	strTemp.Format("%d",rect.top);
	if (!SetCString(strKeyName, strRectTop, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%d",rect.bottom);
	if (!SetCString(strKeyName, strRectBot, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%d",rect.left);
	if (!SetCString(strKeyName, strRectLft, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%d",rect.right);
	if (!SetCString(strKeyName, strRectRht, strTemp, bCreate))
		return FALSE;

	return TRUE;
}


BOOL CIniFile::SetCRect2D(CString strKeyName, CString strValueName, CRectangle2D rc2D, BOOL bCreate)
{
	CString strRect2DCenterX = strValueName + "Rect2DCenterX";
	CString strRect2DCenterY = strValueName + "Rect2DCenterY";
	CString strRect2DHeight = strValueName + "Rect2DHeight";
	CString strRect2DWidth  = strValueName + "Rect2DWidth";
	CString strRect2DAngle  = strValueName + "Rect2DAngle";

	CString strTemp;
	strTemp.Format("%f",rc2D.GetCenterPoint().x);
	if (!SetCString(strKeyName, strRect2DCenterX, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%f",rc2D.GetCenterPoint().y);
	if (!SetCString(strKeyName, strRect2DCenterY, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%f",rc2D.GetWidth());
	if (!SetCString(strKeyName, strRect2DWidth, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%f",rc2D.GetHeight());
	if (!SetCString(strKeyName, strRect2DHeight, strTemp, bCreate))
		return FALSE;

	strTemp.Format("%f",rc2D.GetAngle());
	if (!SetCString(strKeyName, strRect2DAngle, strTemp, bCreate))
		return FALSE;

	return TRUE;
}

//deletes specified value
//returns true if value existed and deleted, false otherwise
BOOL CIniFile::DeleteValue(CString strKeyName, CString strValueName)
{
	int nKeyNum = FindKey(strKeyName);
	int nValueNum = FindValue(nKeyNum,strValueName);
	if (nKeyNum == -1 || nValueNum == -1)
		return 0;

	keys[nKeyNum].names.RemoveAt(nValueNum);
	keys[nKeyNum].values.RemoveAt(nValueNum);
	return 1;
}

//deletes specified key and all values contained within
//returns true if key existed and deleted, false otherwise
BOOL CIniFile::DeleteKey(CString strKeyName)
{
	int nKeyNum = FindKey(strKeyName);
	if (nKeyNum == -1)
		return 0;
	keys.RemoveAt(nKeyNum);
	names.RemoveAt(nKeyNum);
	return 1;
}

/////////////////////////////////////////////////////////////////////
// Private Functions
/////////////////////////////////////////////////////////////////////

//returns index of specified key, or -1 if not found
int CIniFile::FindKey(CString strKeyName)
{
	int nKeyNum = 0;
	while ( nKeyNum < keys.GetSize() && names[nKeyNum] != strKeyName)
		nKeyNum++;
	if (nKeyNum == keys.GetSize())
		return -1;
	return nKeyNum;
}

//returns index of specified value, in the specified key, or -1 if not found
int CIniFile::FindValue(int nKeyNum, CString strValueName)
{
	if (nKeyNum == -1)
		return -1;
	int nValueNum = 0;
	while (nValueNum < keys[nKeyNum].names.GetSize() && keys[nKeyNum].names[nValueNum] != strValueName)
		nValueNum++;
	if (nValueNum == keys[nKeyNum].names.GetSize())
		return -1;
	return nValueNum;
}

//overloaded from original getline to take CString
istream & CIniFile:: getline(istream & is, CString & str)
{
    char buf[2048];
    is.getline(buf,2048);
    str = buf;
    return is;
}

CString CIniFile::GetCString(CString strKeyName, CString strValueName, CString defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetCString(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetCString(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return keys[nKeyNum].values[nValueNum];
}

int CIniFile::GetInteger(CString strKeyName, CString strValueName, int defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return atoi(keys[nKeyNum].values[nValueNum]);
}

int CIniFile::GetLong(CString strKeyName, CString strValueName, long defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetLong(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetLong(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return atol(keys[nKeyNum].values[nValueNum]);
}

double CIniFile::GetDouble(CString strKeyName, CString strValueName, double defaultvalue)
{
	int nKeyNum = FindKey(strKeyName), nValueNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetDouble(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}

	if (nValueNum == -1)
	{
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strValueName, defaultvalue);
		return defaultvalue;
	}
	return atof(keys[nKeyNum].values[nValueNum]);
}

BOOL CIniFile::GetBoolean(CString strKeyName, CString strValueName, BOOL boolValue)
{
	int nVal;

	if(boolValue)
		nVal = 1;
	else
		nVal = 0;

	int nKeyNum = FindKey(strKeyName);
	int nBoolNum = FindValue(nKeyNum,strValueName);

	if (nKeyNum == -1)
	{
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strValueName, nVal);
		return boolValue;
	}

	if (nBoolNum == -1)
	{
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strValueName, nVal);
		return boolValue;
	}

	nVal = atoi(keys[nKeyNum].values[nBoolNum]);
	if (nVal)
		return TRUE;
	else 
		return FALSE;
}

CPoint CIniFile::GetCPoint(CString strKeyName, CString strValueName, CPoint pt)
{
	CString strPtX = strValueName + "X";
	CString	strPtY = strValueName + "Y";
	int nKeyNum = FindKey(strKeyName);
	int nPtXNum = FindValue(nKeyNum,strPtX);
	int nPtYNum = FindValue(nKeyNum,strPtY);

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strPtX, pt.x);
		SetInteger(strKeyName, strPtY, pt.y);
		return pt;
	}
	if (nPtXNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strPtX, pt.x);
		SetInteger(strKeyName, strPtY, pt.y);
		return pt;
	}
	if (nPtYNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strPtX, pt.x);
		SetInteger(strKeyName, strPtY, pt.y);
		return pt;
	}

	pt.x = atoi(keys[nKeyNum].values[nPtXNum]);
	pt.y = atoi(keys[nKeyNum].values[nPtYNum]);
	
	return pt;
}


CPoint2D CIniFile::GetCPoint2D(CString strKeyName, CString strValueName, CPoint2D pt2D)
{
	CString strPtX = strValueName + "X";
	CString	strPtY = strValueName + "Y";
	int nKeyNum = FindKey(strKeyName);
	int nPtXNum = FindValue(nKeyNum,strPtX);
	int nPtYNum = FindValue(nKeyNum,strPtY);

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetDouble(strKeyName, strPtX, pt2D.x);
		SetDouble(strKeyName, strPtY, pt2D.y);
		return pt2D;
	}
	if (nPtXNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPtX, pt2D.x);
		SetDouble(strKeyName, strPtY, pt2D.y);
		return pt2D;
	}
	if (nPtYNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPtX, pt2D.x);
		SetDouble(strKeyName, strPtY, pt2D.y);
		return pt2D;
	}
	pt2D.x = atof(keys[nKeyNum].values[nPtXNum]);
	pt2D.y = atof(keys[nKeyNum].values[nPtYNum]);
	return pt2D;
}

CPoint3D CIniFile::GetCPoint3D(CString strKeyName, CString strValueName, CPoint3D pt3D)
{
	CString strPtX = strValueName + "X";
	CString	strPtY = strValueName + "Y";
	CString	strPtZ = strValueName + "Z";
	int nKeyNum = FindKey(strKeyName);
	int nPtXNum = FindValue(nKeyNum,strPtX);
	int nPtYNum = FindValue(nKeyNum,strPtY);
	int nPtZNum = FindValue(nKeyNum,strPtZ);

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetDouble(strKeyName, strPtX, pt3D.x);
		SetDouble(strKeyName, strPtY, pt3D.y);
		SetDouble(strKeyName, strPtZ, pt3D.z);
		return pt3D;
	}
	if (nPtXNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPtX, pt3D.x);
		SetDouble(strKeyName, strPtY, pt3D.y);
		SetDouble(strKeyName, strPtZ, pt3D.z);
		return pt3D;
	}
	if (nPtYNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPtX, pt3D.x);
		SetDouble(strKeyName, strPtY, pt3D.y);
		SetDouble(strKeyName, strPtZ, pt3D.z);
		return pt3D;
	}
	if (nPtZNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPtX, pt3D.x);
		SetDouble(strKeyName, strPtY, pt3D.y);
		SetDouble(strKeyName, strPtZ, pt3D.z);
		return pt3D;
	}
	pt3D.x = atof(keys[nKeyNum].values[nPtXNum]);
	pt3D.y = atof(keys[nKeyNum].values[nPtYNum]);
	pt3D.z = atof(keys[nKeyNum].values[nPtZNum]);
	return pt3D;
}



CLine2D CIniFile::GetCLine2D(CString strKeyName, CString strValueName, CLine2D ln2D)
{
	CString strPt2DX = strValueName + "Pt2DX";
	CString	strPt2DY = strValueName + "Pt2DY";
	CString strVt2DX = strValueName + "Vt2DX";
	CString	strVt2DY = strValueName + "Vt2DY";
	CString strInValid =  strValueName + "Invalid";
	int nKeyNum = FindKey(strKeyName);
	int nPt2DXNum = FindValue(nKeyNum,strPt2DX);
	int nPt2DYNum = FindValue(nKeyNum,strPt2DY);
	int nVt2DXNum = FindValue(nKeyNum,strVt2DX);
	int nVt2DYNum = FindValue(nKeyNum,strVt2DY);
	int nInvalidNum = FindValue(nKeyNum,strInValid);
	CPoint2D Position; 
	CVector2D Direction;
	BOOL m_bInValid;	

	Position = ln2D.GetPosition();
	Direction = ln2D.GetDirection();
	m_bInValid = ln2D.IsInValid();
	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetDouble(strKeyName, strPt2DX, Position.x);
		SetDouble(strKeyName, strPt2DY, Position.y);
		SetDouble(strKeyName, strVt2DX, Direction.x);
		SetDouble(strKeyName, strVt2DY, Direction.y);
		SetInteger(strKeyName, strInValid, m_bInValid);
		return ln2D;
	}
	if (nPt2DXNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPt2DX, Position.x);
		SetDouble(strKeyName, strPt2DY, Position.y);
		SetDouble(strKeyName, strVt2DX, Direction.x);
		SetDouble(strKeyName, strVt2DY, Direction.y);
		SetInteger(strKeyName, strInValid, m_bInValid);
		return ln2D;
	}
	if (nPt2DYNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPt2DX, Position.x);
		SetDouble(strKeyName, strPt2DY, Position.y);
		SetDouble(strKeyName, strVt2DX, Direction.x);
		SetDouble(strKeyName, strVt2DY, Direction.y);
		SetInteger(strKeyName, strInValid, m_bInValid);
		return ln2D;
	}
	if (nVt2DXNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPt2DX, Position.x);
		SetDouble(strKeyName, strPt2DY, Position.y);
		SetDouble(strKeyName, strVt2DX, Direction.x);
		SetDouble(strKeyName, strVt2DY, Direction.y);
		SetInteger(strKeyName, strInValid, m_bInValid);
		return ln2D;
	}
	if (nVt2DYNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPt2DX, Position.x);
		SetDouble(strKeyName, strPt2DY, Position.y);
		SetDouble(strKeyName, strVt2DX, Direction.x);
		SetDouble(strKeyName, strVt2DY, Direction.y);
		SetInteger(strKeyName, strInValid, m_bInValid);
		return ln2D;
	}
	if (nInvalidNum == -1) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strPt2DX, Position.x);
		SetDouble(strKeyName, strPt2DY, Position.y);
		SetDouble(strKeyName, strVt2DX, Direction.x);
		SetDouble(strKeyName, strVt2DY, Direction.y);
		SetInteger(strKeyName, strInValid, m_bInValid);
		return ln2D;
	}
	Position.x = atof(keys[nKeyNum].values[nPt2DXNum]);
	Position.y = atof(keys[nKeyNum].values[nPt2DYNum]);
	Direction.x = atof(keys[nKeyNum].values[nVt2DXNum]);
	Direction.y = atof(keys[nKeyNum].values[nVt2DYNum]);
///	Position.x = atoi(keys[nKeyNum].values[nInvalidNum]); //Udupa; Buggy; Commented
	ln2D.SetPosition(Position);
	ln2D.SetDirection(Direction);
	return ln2D;
}


CRect CIniFile::GetCRect(CString strKeyName, CString strValueName, CRect rect)
{
	CString strRectTop = strValueName + "Top";
	CString strRectBot = strValueName + "Bot";
	CString strRectLft = strValueName + "Lft";
	CString strRectRht = strValueName + "Rht";
	int nKeyNum = FindKey(strKeyName);
	int nRectTopNum = FindValue(nKeyNum,strRectTop);
	int nRectBotNum = FindValue(nKeyNum,strRectBot);
	int nRectLftNum = FindValue(nKeyNum,strRectLft);
	int nRectRhtNum = FindValue(nKeyNum,strRectRht);

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectTopNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectBotNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectLftNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}
	if (nRectRhtNum == -1) {
		error = "Unable to locate specified value.";
		SetInteger(strKeyName, strRectTop, rect.top);
		SetInteger(strKeyName, strRectBot, rect.bottom);
		SetInteger(strKeyName, strRectLft, rect.left);
		SetInteger(strKeyName, strRectRht, rect.right);
		return rect;
	}

	rect.top = atoi(keys[nKeyNum].values[nRectTopNum]);
	rect.bottom = atoi(keys[nKeyNum].values[nRectBotNum]);
	rect.left = atoi(keys[nKeyNum].values[nRectLftNum]);
	rect.right = atoi(keys[nKeyNum].values[nRectRhtNum]);
	
	return rect;
}




BOOL CIniFile::IsModified()
{
	return m_bModified;
}


BOOL CIniFile::IsInputOK()
{
	return m_bFileReadSuccessful;
}

CRectangle2D CIniFile::GetCRect2D(CString strKeyName, CString strValueName, CRectangle2D rc2D)
{
	CPoint2D pt2DCenter;
	double dWidth,dHeight,dAngle;

	CString strRect2DCenterX = strValueName + "Rect2DCenterX";
	CString strRect2DCenterY = strValueName + "Rect2DCenterY";
	CString strRect2DHeight = strValueName + "Rect2DHeight";
	CString strRect2DWidth  = strValueName + "Rect2DWidth";
	CString strRect2DAngle  = strValueName + "Rect2DAngle";
	int nKeyNum = FindKey(strKeyName);

	int nCenterXNum = FindValue(nKeyNum,strRect2DCenterX);
	int nCenterYNum = FindValue(nKeyNum,strRect2DCenterY);
	int nHeightNum  = FindValue(nKeyNum,strRect2DHeight);
	int nWidthNum   = FindValue(nKeyNum,strRect2DWidth);
	int nAngle      = FindValue(nKeyNum,strRect2DAngle);

	pt2DCenter = rc2D.GetCenterPoint();
	dWidth = rc2D.GetWidth();
	dHeight = rc2D.GetHeight();
	dAngle = rc2D.GetAngle();

	if (nKeyNum == -1) {
		error = "Unable to locate specified key.";
		SetDouble(strKeyName, strRect2DCenterX,pt2DCenter.x);
		SetDouble(strKeyName, strRect2DCenterY,pt2DCenter.y);
		SetDouble(strKeyName, strRect2DHeight, dHeight);
		SetDouble(strKeyName, strRect2DWidth, dWidth);
		SetDouble(strKeyName, strRect2DAngle, dAngle);
		return rc2D;
	}
	if ((nCenterXNum == -1) || (nCenterYNum == -1) || (nHeightNum == -1)
		||(nWidthNum == -1) || (nAngle == -1)) {
		error = "Unable to locate specified value.";
		SetDouble(strKeyName, strRect2DCenterX,pt2DCenter.x);
		SetDouble(strKeyName, strRect2DCenterY,pt2DCenter.y);
		SetDouble(strKeyName, strRect2DHeight, dHeight);
		SetDouble(strKeyName, strRect2DWidth, dWidth);
		SetDouble(strKeyName, strRect2DAngle, dAngle);
		return rc2D;
	}
	

	pt2DCenter.x = atof(keys[nKeyNum].values[nCenterXNum]);
	pt2DCenter.y = atof(keys[nKeyNum].values[nCenterYNum]);
	dHeight = atof(keys[nKeyNum].values[nHeightNum]);
	dWidth = atof(keys[nKeyNum].values[nWidthNum]);
	dAngle = atof(keys[nKeyNum].values[nAngle]);


	rc2D.ShiftTo(pt2DCenter);
	rc2D.SetSize(CSize2D(dWidth,dHeight));
	rc2D.RotateTo(dAngle);


	
	return rc2D;
}

