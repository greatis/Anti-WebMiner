// SSLCon.cpp: implementation of the CSslConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafxs.h"
#include "SSLCon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
String GetResultHeaders(HINTERNET hHttp)
{
	DWORD dwSize = 2048;
	LPVOID lpOutBuffer = NULL;
	String result;
	TCHAR buff[2048]; //="Location";

	// HTTP_QUERY_CUSTOM,
	if(HttpQueryInfo(hHttp,HTTP_QUERY_RAW_HEADERS_CRLF,
		(LPVOID)buff,&dwSize,NULL))
	{
		if (GetLastError()==ERROR_HTTP_HEADER_NOT_FOUND)
        {
            return result;
        }
        else
        {
         result=buff;
        }
    }

   return result;
}

CSslConnection::CSslConnection()
{
	m_hInternet = NULL;
	m_hRequest = NULL;
	m_certStoreType = certStoreMY;
	m_hStore = NULL;
	m_hSession = NULL;
	m_pContext = NULL;
	m_wPort = 443;
	m_strAgentName = "";	
	m_secureFlags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|
					INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
}

CSslConnection::~CSslConnection()
{
	ClearHandles();
}


bool CSslConnection::ConnectToHttpsServer(String &strVerb)
{
	try {
		m_hInternet = InternetOpen(m_strAgentName.t_str(), INTERNET_OPEN_TYPE_PRECONFIG ,
				NULL, NULL, 0);
		if (!m_hInternet) {
			m_strLastError = "Cannot open internet";
			m_lastErrorCode = GetLastError();
			return false;
		}
			
		m_hSession = InternetConnect(m_hInternet, 
			m_strServerName.t_str(),
			m_wPort,
			m_strUserName.t_str(),
			m_strPassword.t_str(),
			INTERNET_SERVICE_HTTP,
			0,
			0);
		if (!m_hSession) {			
			m_strLastError = "Cannot connect to internet";
			m_lastErrorCode = GetLastError();
			ClearHandles();
			return false;
		}
		m_hRequest = HttpOpenRequest(m_hSession, 
				strVerb.t_str(),
				m_strObjectName.t_str(),
				NULL,
				NULL,
				NULL,
				m_secureFlags /*| INTERNET_FLAG_NO_AUTO_REDIRECT*/,
				m_ReqID);
// /*| INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID|INTERNET_FLAG_IGNORE_CERT_DATE_INVALID*/
                                
		if (!m_hRequest) {
			m_strLastError = "Cannot perform http request";
			m_lastErrorCode = GetLastError();
			ClearHandles();
			return false;
		}


		m_ReqID++;
	}
	catch(...) {
		m_strLastError = "Memory Exception occured";
		m_lastErrorCode = GetLastError();
		return false;
	}
	return true;
}

bool CSslConnection::SendHttpsRequest()
{
	try {
		for (int tries = 0; tries < 20; ++tries) {
			int result =  HttpSendRequest(m_hRequest, m_strHeaders.t_str(), m_strHeaders.Length(), NULL, 0);
		       	if (result)
				return true;
                                							
			int lastErr = GetLastError();
			if (lastErr == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED) {
				//if (!SetClientCert()) {

					m_strLastError = "Cannot perform http request, client authentication needed but couldnt detect required client certificate";
					m_lastErrorCode = GetLastError();
					return false;
				// }
			}
			else if (lastErr == ERROR_INTERNET_INVALID_CA) {
				m_strLastError = "Cannot perform http request, client authentication needed, invalid client certificate is used";
				m_lastErrorCode = GetLastError();
				return false;
			}
			else {
				m_strLastError = "Cannot perform http request";
				m_lastErrorCode = GetLastError();
				return false;
			}
		} 
	}
	catch(...) {
		m_strLastError = "Memory Exception occured";
		m_lastErrorCode = GetLastError();
		return false;
	}
	return false;
}

bool CSslConnection::PostHttps(TCHAR *cpHeaders, DWORD iHeadlen, TCHAR *cpPostData, DWORD iPostLen)
{
	try {
		for (int tries = 0; tries < 20; ++tries) {
			int result =  HttpSendRequest(m_hRequest, cpHeaders, iHeadlen, cpPostData, iPostLen);
			if (result) 				
				return true;							
			int lastErr = GetLastError();
			if (lastErr == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED) {
				//if (!SetClientCert()) {

					m_strLastError = "Cannot perform http request, client authentication needed but couldnt detect required client certificate";
					m_lastErrorCode = GetLastError();
					return false;
				// }
			}
			else if (lastErr == ERROR_INTERNET_INVALID_CA) {
				m_strLastError = "Cannot perform http request, client authentication needed, invalid client certificate is used";
				m_lastErrorCode = GetLastError();
				return false;
			}
			else {
				m_strLastError = "Cannot perform http request";
				m_lastErrorCode = GetLastError();
				return false;
			}
		} 
	}
	catch(...) {
		m_strLastError = "Memory Exception occured";
		m_lastErrorCode = GetLastError();
		return false;
	}
	return false;
}

void CSslConnection::ClearHandles()
{
	if (m_hInternet) {
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
		
	if (m_hSession) {
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
	}

	if (m_pContext) {
	 //	CertFreeCertificateContext(m_pContext);
		m_pContext = NULL;
	}
	if (m_hStore) {
	//	CertCloseStore(m_hStore, CERT_CLOSE_STORE_FORCE_FLAG);
		m_hStore = NULL;
	}
}

bool CSslConnection::SetClientCert()
{
	TCHAR *lpszStoreName;
	switch (m_certStoreType) {
	case certStoreMY:
		lpszStoreName = L"MY";
		break;
	case certStoreCA:
		lpszStoreName = L"CA";
		break;
	case certStoreROOT:
		lpszStoreName = L"ROOT";
		break;
	case certStoreSPC:
		lpszStoreName = L"SPC";
		break;
	}

// 	m_hStore = CertOpenSystemStore(NULL, lpszStoreName);
	if (!m_hStore) {
		m_strLastError = "Cannot open system store ";
		m_strLastError += lpszStoreName;
		m_lastErrorCode = GetLastError();
		ClearHandles();
		return false;
	}
	
	m_pContext = FindCertWithOUNITName();

	if (!m_pContext) {
		m_strLastError = "Cannot find the required certificate";
		m_lastErrorCode = GetLastError();
		ClearHandles();
		return false;
	}
	
	// INTERNET_OPTION_CLIENT_CERT_CONTEXT is 84
	int res = InternetSetOption(m_hRequest, 
						       84 /*	INTERNET_OPTION_CLIENT_CERT_CONTEXT */, 
							(void *) m_pContext, sizeof(CERT_CONTEXT));
	if (!res) {
		m_strLastError = "Cannot set certificate context";
		m_lastErrorCode = GetLastError();
		ClearHandles();
		return false;
	}

	return true;
}

PCCERT_CONTEXT CSslConnection::FindCertWithOUNITName()
{
	//This function performs a certificate contex search
	//by the organizational unit name of the issuer
	//Take this function as a sample for your possible different search functions
	PCCERT_CONTEXT pCertContext = NULL;		
	CERT_RDN certRDN;

	certRDN.cRDNAttr = 1;
	certRDN.rgRDNAttr = new CERT_RDN_ATTR;
	certRDN.rgRDNAttr->pszObjId = szOID_ORGANIZATIONAL_UNIT_NAME;
	certRDN.rgRDNAttr->dwValueType = CERT_RDN_ANY_TYPE;
	certRDN.rgRDNAttr->Value.pbData = (BYTE *) m_strOName.t_str();
	certRDN.rgRDNAttr->Value.cbData = m_strOName.Length()*sizeof(TCHAR);
      /*
	pCertContext = CertFindCertificateInStore(m_hStore,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		0, CERT_FIND_ISSUER_ATTR, &certRDN, NULL);

        */
	delete certRDN.rgRDNAttr;
	return pCertContext;
}

String CSslConnection::GetRequestResult()
{
	DWORD dwNumberOfBytesRead;
	char sz[1024];
	String strResult;
	int result;
		DWORD dwSize=0;
		memset(sz, 0, 1024);

//          String ss= GetResultHeaders( m_hRequest);
	do {
		 // HttpQueryInfo(m_hRequest,  HTTP_QUERY_RAW_HEADERS_CRLF ,sz,&dwSize,8,)

		result = InternetReadFile(m_hRequest, sz, 1023, &dwNumberOfBytesRead);
		sz[dwNumberOfBytesRead] = '\0';
	  //	int x = _tcslen(sz);
		strResult = strResult+String(sz);
		memset(sz, 0, 1024);

	} while(result && dwNumberOfBytesRead != 0);
	return strResult;
}


