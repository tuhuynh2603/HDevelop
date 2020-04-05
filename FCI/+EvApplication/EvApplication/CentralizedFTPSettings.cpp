// CentralizedFTPSettings.cpp : implementation file
//

#include "stdafx.h"
#include "CentralizedFTPSettings.h"
#include "afxdialogex.h"
#include "Application.h"


// CCentralizedFTPSettings dialog

IMPLEMENT_DYNAMIC(CCentralizedFTPSettings, CDialogEx)

CCentralizedFTPSettings::CCentralizedFTPSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CENTRALIZED_COMMUNICATION_DLG, pParent)
	, m_strSystemName(_T(""))
	, m_strSharedDirectory(_T(""))
	, m_strUserName(_T(""))
	, m_strPassword(_T(""))
{

}

CCentralizedFTPSettings::~CCentralizedFTPSettings()
{
}

void CCentralizedFTPSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SYSTEM_NAME, m_strSystemName);
	DDX_Text(pDX, IDC_EDIT_SHARED_DIRECTORY, m_strSharedDirectory);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
}


BEGIN_MESSAGE_MAP(CCentralizedFTPSettings, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CCentralizedFTPSettings::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDOK, &CCentralizedFTPSettings::OnBnClickedOk)
END_MESSAGE_MAP()


// CCentralizedFTPSettings message handlers


void CCentralizedFTPSettings::OnBnClickedButtonConnect()
{
	UpdateData(TRUE);

	Connect();
}

void CCentralizedFTPSettings::Connect()
{
	CString strRemoteName;
	strRemoteName.Format("\\\\%s\\%s", m_strSystemName, m_strSharedDirectory);
	theApp.m_strCentralizedFTPSharedDirectory = strRemoteName;

	NETRESOURCE resource;
	resource.dwType = RESOURCETYPE_DISK;
	resource.lpLocalName = 0;
	resource.lpRemoteName = (LPSTR)(LPCTSTR)strRemoteName;/*/* "\\\\\LAPTOP-EAV1SQG9\\Users\\Emage Vision India\\abc";*/
	resource.lpProvider = 0;
	DWORD conResult;

	DWORD result = WNetAddConnection2(&resource, m_strPassword, m_strUserName, CONNECT_UPDATE_PROFILE);

	if (result != NO_ERROR) {
		AfxMessageBox("Multiple Pc Not Connected Please Check Lan Connection and Try Again ");
	}
}


BOOL CCentralizedFTPSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strComm;
	strComm.Format("Centralized Comm");

	m_strSystemName = "SYSTEM NAME";
	m_strSystemName = GetRegistryEntry(READ_REGPATH + strComm, "System Name", m_strSystemName);
	PutRegistryEntry(WRITE_REGPATH + strComm, "System Name", m_strSystemName);

	m_strSharedDirectory = "C:\\";
	m_strSharedDirectory = GetRegistryEntry(READ_REGPATH + strComm, "Shared Directory", m_strSharedDirectory);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Shared Directory", m_strSharedDirectory);

	m_strUserName = "";
	m_strUserName = GetRegistryEntry(READ_REGPATH + strComm, "User Name", m_strUserName);
	PutRegistryEntry(WRITE_REGPATH + strComm, "User Name", m_strUserName);
	
	m_strPassword = "****";
	m_strPassword = GetRegistryEntry(READ_REGPATH + strComm, "Password", m_strPassword);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Password", m_strPassword);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CCentralizedFTPSettings::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString strComm;
	strComm.Format("Centralized Comm");

	PutRegistryEntry(WRITE_REGPATH + strComm, "System Name", m_strSystemName);

	PutRegistryEntry(WRITE_REGPATH + strComm, "Shared Directory", m_strSharedDirectory);

	PutRegistryEntry(WRITE_REGPATH + strComm, "User Name", m_strUserName);

	PutRegistryEntry(WRITE_REGPATH + strComm, "Password", m_strPassword);

	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
