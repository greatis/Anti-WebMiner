// SslCon: interface for the CSslConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINETSEC_H__91AD1B9B_5B03_457E_A6B6_D66BB03147B7__INCLUDED_)
#define AFX_WINETSEC_H__91AD1B9B_5B03_457E_A6B6_D66BB03147B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Wininet.h>
#include <wincrypt.h>

#pragma warning(disable:4786)

#include <string>
#include <vcl.h>
using namespace std;

enum CertStoreType {certStoreMY, certStoreCA, certStoreROOT, certStoreSPC};

class CSslConnection
{
public:
	CSslConnection();
	virtual ~CSslConnection();
public:
	bool ConnectToHttpsServer(String &strVerb);
	bool SendHttpsRequest();
	String GetRequestResult();
        bool PostHttps(TCHAR *cpHeaders, DWORD iHeadlen, TCHAR *cpPostData, DWORD iPostLen);
public: //accessors
	void SetAgentName(String &strAgentName) { m_strAgentName = strAgentName; }
	void SetCertStoreType(CertStoreType storeID) { m_certStoreType = storeID; }
	void SetServerName(String &strServerName) { m_strServerName = strServerName; }
	void SetObjectName(String &strObjectName) { m_strObjectName = strObjectName; }
      	void SetHeaders(String &sHeaders) { m_strHeaders = sHeaders; }
	void SetPort(INTERNET_PORT wPort = INTERNET_DEFAULT_HTTPS_PORT) { m_wPort = wPort; }
	void SetRequestID(int reqID) { m_ReqID = reqID; }
	void SetSecurityFlags(int flags) { m_secureFlags = flags; }
	//Search indicators
	void SetOrganizationName(String &strOName) { m_strOName = strOName;}
	String GetLastErrorString() { return m_strLastError; }
	int GetLastErrorCode() { return m_lastErrorCode; }
	String m_strLastError;
	void ClearHandles();
private:
	// examine the following function in order to perform different certificate
	// property searchs in stores. It detects the desired certificate with the organization name
	PCCERT_CONTEXT FindCertWithOUNITName();
	/////////////////////////////////////
	bool SetClientCert();
private:
	HINTERNET m_hInternet;
	HINTERNET m_hRequest;
	HINTERNET m_hSession;

	String m_strServerName;
	String m_strObjectName;
	INTERNET_PORT m_wPort;
	int m_secureFlags;

	HCERTSTORE m_hStore;
	PCCERT_CONTEXT m_pContext;
	CertStoreType m_certStoreType;
	String m_strUserName;
	String m_strPassword;
	String m_strAgentName;
	String m_strOName;
	int m_lastErrorCode;
	int m_ReqID;
        // Added Nar 4 2015
        String m_strHeaders;
};

#endif // !defined(AFX_WINETSEC_H__91AD1B9B_5B03_457E_A6B6_D66BB03147B7__INCLUDED_)

