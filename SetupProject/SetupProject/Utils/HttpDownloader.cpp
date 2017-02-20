#include "stdafx.h"
#include "HttpDownloader.h"

#define DOWNLOAD_BUFFER_SIZE		(2 * 1024 * 1024)		// 2 MB

Utils::CHttpDownloader::CHttpDownloader(LPCTSTR pAgent /*= NULL*/)
{
	if (pAgent)
		m_Agent = pAgent;
	else
		m_Agent = _T("MyOwnInternetAgent");
}


Utils::CHttpDownloader::~CHttpDownloader()
{
}

Utils::HTTP_RESULT Utils::CHttpDownloader::Download(
	__in LPCTSTR pszUrl,
	__in_opt CDownloadObserver *observer,
	__in_opt void* observerParam /*= NULL*/,
	__out_opt DWORD *pHttpErrorCode /*= NULL*/,
	__out_opt _tstring *pHttpErrorString /*= NULL*/)
{
	if (pszUrl == NULL)
		return HTTP_INVALID_PARAMETERS;

	if (FAILED(InternetAttemptConnect(0)))
		return HTTP_CONNECTION_ERROR;

	DWORD proxyTypePriorities[] = {
		INTERNET_OPEN_TYPE_DIRECT,
		INTERNET_OPEN_TYPE_PRECONFIG,
		INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY,
	};
	
	URL_COMPONENTS url = {};
	url.dwStructSize = sizeof(URL_COMPONENTS);
	url.dwHostNameLength = 1;
	url.dwPasswordLength = 1;
	url.dwUserNameLength = 1;
	url.dwUrlPathLength = 1;
	
	if (!InternetCrackUrl(pszUrl, 0, 0, &url) ||
		(url.nScheme != INTERNET_SCHEME_HTTP && url.nScheme != INTERNET_SCHEME_HTTPS))		
		return HTTP_INVALID_PARAMETERS;
	
	HTTP_RESULT httpResult = HTTP_ERROR;
	LPCTSTR *ppAcceptTypes = NULL;
	std::vector<LPCTSTR> acceptTypes;

	for (size_t i = 0; i < m_AcceptTypes.size(); i++)
		acceptTypes.push_back(m_AcceptTypes[i].c_str());
	if (!acceptTypes.empty())
	{
		acceptTypes.push_back(NULL);
		ppAcceptTypes = &acceptTypes[0];
	}

	bool isHttps = url.nScheme == INTERNET_SCHEME_HTTPS;
	_tstring userName;
	_tstring password;
	_tstring hostName;
	_tstring httpObjectName;

	hostName.append(url.lpszHostName, url.dwHostNameLength);
	userName.append(url.lpszUserName, url.dwUserNameLength);
	password.append(url.lpszPassword, url.dwPasswordLength);
	httpObjectName.append(url.lpszUrlPath, url.dwUrlPathLength);

	for (DWORD proxyType : proxyTypePriorities)
	{
		httpResult = HTTP_ERROR;
		HINTERNET hInternet = InternetOpen(m_Agent.c_str(), proxyType, NULL, NULL, 0);
		if (hInternet == NULL)
			continue;
		
		HINTERNET hConnect = InternetConnect(hInternet, hostName.c_str(), url.nPort, userName.c_str(), password.c_str(), INTERNET_SERVICE_HTTP, 0, NULL);
		if (hConnect)
		{
			DWORD requestFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE;
			requestFlags |= isHttps ? INTERNET_FLAG_SECURE : 0;

			HINTERNET hRequest = HttpOpenRequest(hConnect, _T("GET"), httpObjectName.c_str(), NULL, NULL, ppAcceptTypes, requestFlags, NULL);
			if (hRequest)
			{
				if (HttpSendRequest(hRequest, NULL, 0, NULL, 0))
					// Request has been sent successfully, now try receiving response
					httpResult = ReceiveHttpResponse(hRequest, observer, observerParam, pHttpErrorCode, pHttpErrorString);
	
				InternetCloseHandle(hRequest);
			}
			else
			{
				TRACE("HttpOpenRequest %08X\n", GetLastError());
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hInternet);
		if (httpResult == HTTP_OK || httpResult == HTTP_USER_CANCELED)
			break;
	}

	return httpResult;
}

void Utils::CHttpDownloader::AddAcceptType(__in LPCTSTR acceptType)
{
	if (acceptType)
		m_AcceptTypes.push_back(acceptType);
}


Utils::HTTP_RESULT Utils::CHttpDownloader::ReceiveHttpResponse(
	__in HINTERNET hRequest,
	__in_opt CDownloadObserver *observer,
	__in_opt void* observerParam,
	__out_opt DWORD *pHttpErrorCode,
	__out_opt _tstring *pHttpErrorString)
{
	DWORD contentLen;
	DWORD dataSize;
	HTTP_RESULT httpResult = HTTP_OK;

	// Get response status code and status text
	if (!GetResponseStatus(hRequest, pHttpErrorCode, pHttpErrorString))
		return HTTP_ERROR;
	
	// Get content length
	dataSize = sizeof(contentLen);
	if (HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentLen, &dataSize, NULL))
	{
		if (observer && !observer->OnDownloadStarting(contentLen, observerParam))
			return HTTP_USER_CANCELED;
	}
	else
		return HTTP_ERROR;

	DWORD remain = contentLen;
	LPBYTE buffer = NULL;
	if (remain > 0)
	{
		buffer = new BYTE[DOWNLOAD_BUFFER_SIZE];
		if (buffer == NULL) return HTTP_ERROR;
	}

	while (remain)
	{
		DWORD sizeToRead = DOWNLOAD_BUFFER_SIZE;
		DWORD actualBytesRead;

		if (sizeToRead > remain) sizeToRead = remain;

		if (InternetReadFile(
			hRequest,
			buffer,
			sizeToRead,
			&actualBytesRead))
		{
			remain -= actualBytesRead;
			if (observer && !observer->OnDownloaded(buffer, actualBytesRead, observerParam))
			{
				httpResult = HTTP_ERROR;
				break;
			}
		}
		else
		{
			DWORD err = GetLastError();
			TRACE("InternetReadFile error 0x%08X (%d)\n", err, err);
			httpResult = HTTP_ERROR;
			break;
		}
	}
	if (buffer) delete[] buffer;

	if (httpResult == HTTP_OK && observer)
		observer->OnDownloadCompleted(observerParam);
	return httpResult;
}

bool Utils::CHttpDownloader::GetResponseStatus(
	__in HINTERNET hRequest,
	__out_opt DWORD *pHttpErrorCode,
	__out_opt _tstring *pHttpErrorString)
{
	DWORD dataSize;
	if (pHttpErrorCode)
	{
		dataSize = sizeof(DWORD);
		// Get response code
		if (!HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, pHttpErrorCode, &dataSize, NULL))
		{
			TRACE("HttpQueryInfo error %d\n", GetLastError());
			return false;
		}
	}

	if (pHttpErrorString)
	{
		dataSize = 0;
		HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_TEXT, NULL, &dataSize, NULL);
		if (dataSize == 0) return HTTP_ERROR;

		TCHAR *pStatusText = new TCHAR[dataSize / sizeof(TCHAR) + 1];
		if (pStatusText == NULL) return HTTP_ERROR;
		memset(pStatusText, 0, dataSize + sizeof(TCHAR));

		if (HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_TEXT, pStatusText, &dataSize, NULL))
		{
			*pHttpErrorString = pStatusText;
			delete[] pStatusText;
		}
		else
		{
			delete[] pStatusText;
			return false;
		}
	}

	return true;
}

