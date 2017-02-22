#pragma once
#include "Setup/SetupData.h"

#include "Utils/HttpDownloader.h"

using namespace MySetup;
using namespace Utils;

namespace SetupRunner
{
	typedef struct
	{
		CSetupData *setupData;
		CSetupObserver *setupObserver;		
	} SETUP_THREAD_DATA;

	DWORD WINAPI MainSetupThread(LPVOID lParam);

	bool RunMainSetup(
		__inout CSetupData *setupData,
		__in CSetupObserver *setupObserver);

	void SetupThread(SETUP_THREAD_DATA *pData);

	bool Is64BitOs();

	_tstring GenInstallerTempPath(__in SETUP_THREAD_DATA *pData);

	typedef int(*SETUP_STAGE)(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);
	
	DWORD CalculatePercentage(DWORD currentStage, DWORD totalStages, double stagePercentage);
	
	// Download the online installer
	int SetupStageDownloadInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);

	// Extract the offline installer
	int SetupStageExtractInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);

	// Invoke the installer
	int SetupStageInvokeInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);

	LPTSTR GetCommandLineArg(
		__in const std::vector<_tstring> &argList,
		__out size_t &cmdLen);

	class CDownloadObserver :
		public CHttpDownloadObserver
	{
	public:
		CDownloadObserver(LPCTSTR destPath, SETUP_THREAD_DATA *pSetupData, DWORD currentState, DWORD totalStages);
		virtual ~CDownloadObserver();
	private:
		DWORD m_CurrentStage;
		DWORD m_TotalStages;
		DWORD m_TotalDownloadSize;
		DWORD m_TotalDownloaded;
		SETUP_THREAD_DATA *m_ThreadData;
		HANDLE m_DestFile;
	public:
	
		// Implement CHttpDownloadObserver interface
		virtual bool OnDownloadStarting(__in DWORD dataSize, __in_opt void *param);

		virtual bool OnDownloaded(__in_bcount(bufferSize) void *buffer, __in DWORD bufferSize, __in_opt void* param);

		virtual void OnDownloadCompleted(__in_opt void *param);

		virtual bool ShouldPause(DWORD timeout, __in_opt void *param);

		virtual bool ShouldStop(DWORD timeout, __in_opt void *param);
	};
}
