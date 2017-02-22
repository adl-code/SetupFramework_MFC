#pragma once
#include "Setup/SetupData.h"

using namespace MySetup;

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

	typedef int(*SETUP_STAGE)(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);
	
	DWORD CalculatePercentage(DWORD currentStage, DWORD totalStages, double stagePercentage);
	
	// Download the online installer
	int SetupStageDownloadInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);

	// Extract the offline installer
	int SetupStageExtractInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);

	// Invoke the installer
	int SetupStageInvokeInstaller(DWORD currentStage, DWORD totalStages, SETUP_THREAD_DATA *pData);


}
