#include "stdafx.h"

#include "Setup/SetupRunner.h"
#include "Setup/ConfigData.h"

#include "Utils/ResourceLoader.h"

#define SETUP_CFG_CURRENT_INSTALLER	"current_installer"
#define SETUP_CFG_INSTALLER_PATH "installer_path"

void SetupRunner::SetupThread(SETUP_THREAD_DATA *pData)
{
	std::vector<SETUP_STAGE> setupStages;
#if defined ONLINE_INSTALLER
	setupStages.push_back(&SetupStageDownloadInstaller);
#endif

#if defined OFFLINE_INSTALLER
	setupStages.push_back(&SetupStageExtractInstaller);
#endif

	setupStages.push_back(&SetupStageInvokeInstaller);

	DWORD totalStages = (DWORD)setupStages.size();
	int result = SETUP_OK;
	for (DWORD i = 0; i < totalStages; i++)
	{
		result = setupStages[i](i + 1, totalStages, pData);		
		if (result == SETUP_OK)
		{
			// Update stage progress
			pData->setupObserver->UpdateProgress(CalculatePercentage(i + 1, totalStages, 100));		
		}
		else
		{
			break;
		}
	}

	// Cleanup
	std::string installerPath;
	if (pData->setupData->GetSetupConfig(SETUP_CFG_CURRENT_INSTALLER, SETUP_CFG_INSTALLER_PATH, installerPath) &&
		!installerPath.empty())
	{
		DeleteFile(UTF8_TO_TSTRING(installerPath).c_str());
	}
	

	pData->setupObserver->OnFinish(result);

}

DWORD WINAPI SetupRunner::MainSetupThread(LPVOID lParam)
{
	if (lParam)
	{
		SETUP_THREAD_DATA *pData = (SETUP_THREAD_DATA*)lParam;
		Sleep(100);
		SetupThread(pData);
		delete pData;
	}
	return 0;
}

bool SetupRunner::RunMainSetup(
	__inout CSetupData *setupData,
	__in CSetupObserver *setupObserver)
{
	if (setupData == NULL || setupObserver == NULL) return false;
	SETUP_THREAD_DATA *pData = new SETUP_THREAD_DATA();
	if (pData == NULL) return false;

	pData->setupData = setupData;
	pData->setupObserver = setupObserver;
	HANDLE hThread = CreateThread(
		NULL,
		0,
		&MainSetupThread,
		pData,
		0,
		NULL);

	if (hThread == NULL) return false;

	CloseHandle(hThread);
	return true;
}

bool SetupRunner::Is64BitOs()
{
#if defined(_M_X64) || defined(__amd64__)
	return true;
#else
	BOOL wow64Process = FALSE;
	if (!IsWow64Process(GetCurrentProcess(), &wow64Process)) return false;
	return (wow64Process == TRUE);
#endif

}

DWORD SetupRunner::CalculatePercentage(DWORD currentStage, DWORD totalStages, double stagePercentage)
{
	if (currentStage == 0 || currentStage > totalStages || totalStages == 0 || stagePercentage > 100) return 0;

	return (DWORD)(100.0f * (currentStage - 1) / totalStages + stagePercentage / totalStages);
}

// Download the online installer
int SetupRunner::SetupStageDownloadInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData)
{
	pData->setupObserver->UpdateStatus(pData->setupData->GetString(SID_DOWNLOADING_INSTALLER).c_str());

	return SETUP_OK;
}

// Extract the offline installer
int SetupRunner::SetupStageExtractInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData)
{
	std::string resName, resType;

	// Initialize status
	pData->setupObserver->UpdateStatus(pData->setupData->GetString(SID_EXTRACTING_INSTALLER).c_str());

	std::string installerSection = Is64BitOs() ? SETUP_CFG_OFFLINE_INSTALLER_AMD64 : SETUP_CFG_OFFLINE_INSTALLER_AMD64;

	if (!pData->setupData->GetSetupConfig(installerSection.c_str(), "res_name", resName) ||
		!pData->setupData->GetSetupConfig(installerSection.c_str(), "res_type", resType))
	{
		pData->setupData->LogA(LogError, "No resource configuration for %s found.\n", installerSection.c_str());
		return SETUP_ERROR;
	}

	DWORD resSize = 0;
	LPBYTE resData = (LPBYTE)Utils::LoadResource(NULL, UTF8_TO_TSTRING(resType).c_str(), UTF8_TO_TSTRING(resName).c_str(), &resSize);
	if (resSize == 0 || resData == NULL)
	{
		pData->setupData->LogA(LogError, "No resource entry for %s found.", installerSection.c_str());
		return SETUP_ERROR;
	}

	// Write resource data to offline file
	TCHAR tempDir[MAX_PATH];
	TCHAR installerPath[MAX_PATH];
	if (!GetTempPath(MAX_PATH, tempDir) ||
		!GetTempFileName(tempDir, _T("ist"), 0, installerPath))
	{
		pData->setupData->LogA(LogError, "Failed to generate path name for installer.");
		return SETUP_ERROR;
	}

	HANDLE hFile = CreateFile(installerPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DeleteFile(installerPath);
		pData->setupData->LogA(LogError, "Failed to open temporary file \"%s\" to write setup data.%s", TSTRING_TO_UTF8(installerPath).c_str());
		return SETUP_ERROR;
	}
	
	pData->setupData->SetSetupConfig(SETUP_CFG_CURRENT_INSTALLER, SETUP_CFG_INSTALLER_PATH, TSTRING_TO_UTF8(installerPath).c_str());

	DWORD remain = resSize;
	DWORD offset = 0;
	DWORD sizeToWrite;
	DWORD written;
	DWORD blockSize = 0;
#if defined __DEBUG || defined DEBUG
	blockSize = 1000;
#else
	
#endif	
	while (remain)
	{		
		if (pData->setupData->ShouldPause(5)) continue;

		if (pData->setupData->ShouldStop(5))
		{
			CloseHandle(hFile);
			return SETUP_CANCEL;
		}
		
		if (blockSize)
			sizeToWrite = blockSize > remain ? remain : blockSize;
		else
			sizeToWrite = remain;
		if (WriteFile(hFile, resData + offset, sizeToWrite, &written, NULL))
		{			
			offset += written;
			remain -= written;
			
			double percentage = (double)offset * 100.0f / resSize;				
			pData->setupObserver->UpdateProgress(CalculatePercentage(currentStage, totalStages, percentage));
#if defined DEBUG || defined _DEBUG
			pData->setupData->LogA(LogDebug, "Writing offline installer %d%%", (int)percentage);
#endif
		}
		else
		{
			pData->setupData->LogA(LogError, "Failed to write installer to disk. Error: %08X", GetLastError());
			CloseHandle(hFile);
			return SETUP_ERROR;
		}
	}

	CloseHandle(hFile);
	
	return SETUP_OK;
}

// Invoke the installer
int SetupRunner::SetupStageInvokeInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData)
{
	UNREFERENCED_PARAMETER(currentStage);
	UNREFERENCED_PARAMETER(totalStages);

	std::string configValue;
	if (!pData->setupData->GetSetupConfig(SETUP_CFG_CURRENT_INSTALLER, SETUP_CFG_INSTALLER_PATH, configValue))
		return SETUP_ERROR;

	_tstring installerPath = UTF8_TO_TSTRING(configValue);

	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};

	si.cb = sizeof(si);

	int result = SETUP_ERROR;
	if (CreateProcess(installerPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) // Too much NULL would kill you :D
	{
		// We would hang here until the child process finishes its jobs
		WaitForSingleObject(pi.hProcess, INFINITE);
		DWORD exitCode;
		GetExitCodeProcess(pi.hProcess, &exitCode);
		result = exitCode == 0 ? SETUP_OK : SETUP_ERROR;
	}
	
	DeleteFile(installerPath.c_str());
	return result;
}



