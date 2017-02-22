#pragma once
#include <vector>
#include "Utils/StringUtils.h"
#include <WinInet.h>

#pragma comment (lib, "WinInet.lib")

namespace Utils
{
	class CHttpDownloadObserver
	{
	public:
		virtual ~CHttpDownloadObserver()
		{

		}

		DWORD contentLen;
		DWORD dataSize;
		virtual bool OnDownloadStarting(
			__in DWORD dataSize,
			__in_opt void *param
			) = 0;

		// Return true to continue downloading
		// Return false to cancel the download
		virtual bool OnDownloaded(
			__in_bcount(bufferSize) void *buffer, // Buffer containing data just downloaded
			__in DWORD bufferSize, // Number of bytes just downloaded
			__in_opt void* param) = 0;

		virtual void OnDownloadCompleted(
			__in_opt void *param) = 0;

		virtual bool ShouldPause(DWORD timeout /*milliseconds*/, __in_opt void *param) = 0;
		virtual bool ShouldStop(DWORD timeout /*milliseconds*/, __in_opt void *param) = 0;
	};

	enum HTTP_RESULT
	{
		HTTP_OK	= 0,
		HTTP_USER_CANCELED = -1,		
		HTTP_ERROR = 1,	// HTTP server returns error code
		HTTP_CONNECTION_ERROR,
		HTTP_INVALID_PARAMETERS,
	};

	class CHttpDownloader
	{
	public:
		CHttpDownloader(LPCTSTR pAgent = NULL);
		~CHttpDownloader();
	private:
		_tstring m_Agent;
		std::vector<_tstring> m_AcceptTypes;

	public:
		HTTP_RESULT Download(
			__in LPCTSTR pszUrl,
			__in_opt CHttpDownloadObserver *observer,
			__in_opt void* observerParam = NULL,
			__out_opt DWORD *pHttpErrorCode = NULL,
			__out_opt _tstring *pHttpErrorString = NULL);

		void AddAcceptType(__in LPCTSTR acceptType);
	private:
		HTTP_RESULT ReceiveHttpResponse(
			__in HINTERNET hRequest,
			__in_opt CHttpDownloadObserver *observer,
			__in_opt void* observerParam,
			__out_opt DWORD *pHttpErrorCode,
			__out_opt _tstring *pHttpErrorString);

		bool GetResponseStatus(
			__in HINTERNET hRequest,
			__out_opt DWORD *pHttpErrorCode,
			__out_opt _tstring *pHttpErrorString);

		bool HasUserCanceled(
			__in_opt CHttpDownloadObserver *observer,
			__in_opt void *observerParam);
	};


}
