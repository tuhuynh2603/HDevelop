#if !defined(_SMARTSTRING_H)
#define _SMARTSTRING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SmartString.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSmartString

class CSmartString
{
	// Constuctors and d-tor
public:
	CSmartString(LPCTSTR pText = NULL);
	CSmartString(UINT nIdText);
	//CSmartString(const CString& strText);
	CSmartString(const CSmartString& d);

	~CSmartString();

	// Operators
	CSmartString& operator=(const CSmartString& data);
	//CSmartString& operator=(const CString& strText);
	CSmartString& operator=(LPCTSTR pData);

	operator LPCTSTR() const;

	// Operations
protected:
	void SetCString(const CString& strText);
	void SetLPCTSTR(LPCTSTR pText);
	void Clear();
	
	// Data
private:
	union 
	{
		CString* m_pStrText;
		LPCTSTR m_pText;
	};
	bool m_bCString;
};

inline CSmartString::CSmartString(LPCTSTR pText)
{	
	SetLPCTSTR(pText);
}

/*
inline CSmartString::CSmartString(const CString& strText)
{
	SetCString(strText);
}
*/

#endif // !defined(_SMARTSTRING_H)