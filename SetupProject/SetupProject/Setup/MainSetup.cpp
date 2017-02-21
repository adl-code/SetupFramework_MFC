#include "stdafx.h"
#include "Setup/SetupData.h"
#include "Setup/MainSetup.h"
#include "Setup/ConfigParser.h"

#include "Resource.h"
#include "UI/SplashDlg.h"
#include "UI/EulaDlg.h"
#include "UI/ConfigDlg.h"
#include "UI/MainSetupDlg.h"

#include "Utils/ResourceLoader.h"
#include "Utils/HttpDownloader.h"

bool RunMainSetup(
	__inout CSetupData *setupData,
	__in CSetupObserver *setupObserver);

void MySetup::InitSetup(
	__inout CSetupData *setupData)
{	
	// Parse config file
	DWORD configStringLen;
	const char *configString = Utils::LoadUtf8Resource(NULL, _T("SETUP"), _T("CONFIG"), configStringLen);
	if (configString && configStringLen)
	{
		std::string configData;
		configData.append(configString, configStringLen);
		Utils::CMsXmlDoc *configDoc = Utils::CMsXmlDoc::ParseFromUtf8String(configData.c_str());
		if (configDoc)
		{
			Utils::CMsXmlNode *rootNode = configDoc->GetRoot();
			if (rootNode && _tcsicmp(rootNode->GetName().c_str(), _T("setup_config")) == 0)
			{
				// Parse the root node
				_tstring schemeName;
				rootNode->GetAttribute(_T("scheme"), schemeName);
				std::vector<Utils::CMsXmlNode*> children = rootNode->GetChildren();
				bool schemeFound = false;
				for (Utils::CMsXmlNode *child : children)
				{					
					if (!schemeFound &&
						_tcsicmp(child->GetName().c_str(), _T("scheme")) == 0)
					{						
						_tstring schemeId;
						if (schemeName.empty())							
							schemeFound = true; // Use the first scheme found if no scheme is specified
						else if (child->GetAttribute(_T("id"), schemeId) &&
							_tcsicmp(schemeId.c_str(), schemeName.c_str()) == 0)							
							schemeFound = true; // Scheme name matches the expected scheme

						if (schemeFound)
							ConfigParser::ParseXmlSetupScheme(setupData, child);
					}
					
					delete child;					
				}
			}
			if (rootNode) delete rootNode;
			delete configDoc;
		}
	}

	setupData->SetHideBorder(true);
	setupData->SetTopMost(true);
}


int MySetup::RunSetup(
	__inout CSetupData *setupData)
{
	if (setupData == NULL) return SETUP_ERROR;

	// Loop through all setup steps
	int iStep = 0;
	int prevStep = -1;
	std::vector<int> setupScreens = setupData->GetSetupScreens();
	int screenCnt = (int)setupScreens.size();

	while (iStep < screenCnt)
	{
		CBaseSetupDlg *setupDlg = NULL;
		std::string screenId;
		CSetupObserver *setupObserver = NULL;

		switch (setupScreens[iStep])
		{
		case SETUP_SCREEN_SPLASH:
			// Display splash screen
			screenId = SCREEN_ID_SPLASH;
			setupDlg = static_cast<CBaseSetupDlg*>(new CSplashDlg());
			break;
		case SETUP_SCREEN_CONFIG:
			screenId = SETUP_SCREEN_ID_CONFIG;
			setupDlg = static_cast<CBaseSetupDlg*>(new CConfigDlg());
			break;
		case SETUP_SCREEN_EULA:
			// Display EULA
			screenId = SETUP_SCREEN_ID_EULA;
			setupDlg = static_cast<CBaseSetupDlg*>(new CEulaDlg());
			break;
		case SETUP_SCREEN_MAIN:
			screenId = SETUP_SCREEN_ID_MAIN;			
			{
				CMainSetupDlg *mainDlg = new CMainSetupDlg();
				setupObserver = static_cast<CSetupObserver*>(mainDlg);
				setupDlg = static_cast<CBaseSetupDlg*>(mainDlg);
			}			
			break;
		default:
			// Unprocessed steps
			return SETUP_CANCEL;
		}

		if (setupDlg == NULL) return SETUP_ERROR;
				
		setupData->SetCouldBeBack(prevStep != SETUP_SCREEN_SPLASH && setupScreens[iStep] != SETUP_SCREEN_MAIN);
		int result = SETUP_OK;
		if (setupObserver)
		{
			if (!RunMainSetup(setupData, setupObserver))
				result = SETUP_ERROR;
		}
		
		if (result == SETUP_OK)
			result = setupDlg->DoSetup(setupData, screenId.c_str());
		delete setupDlg;

		switch (result)
		{
		case SETUP_OK:
			iStep++;
			break;
		case SETUP_BACK:
			iStep--;
			break;
		default:
			return result;
		}

		if (iStep > 0)
			prevStep = setupScreens[iStep - 1];
		else
			prevStep = -1;
	}
	return SETUP_OK;
}

typedef struct
{
	CSetupData *setupData;
	CSetupObserver *setupObserver;
} SETUP_THREAD_DATA;


void SetupThread(SETUP_THREAD_DATA *pData)
{
	Sleep(300);
	Utils::CStringLoader *stringLoader = pData->setupData->GetStringLoader();
	std::string langID = pData->setupData->GetLanguageID();
	LPCSTR pLangID = langID.c_str();

	pData->setupObserver->UpdateStatus(stringLoader->LoadString(SID_DOWNLOADING_INSTALLER, pLangID).c_str());

	for (int i = 0; i < 10; i++)
	{
		pData->setupObserver->UpdateProgress((i + 1) * 10);	
		Sleep(1000);
	}
}

DWORD WINAPI MainSetupThread(LPVOID lParam)
{
	if (lParam)
	{
		SETUP_THREAD_DATA *pData = (SETUP_THREAD_DATA*)lParam;
		SetupThread(pData);
		delete pData;
	}
	return 0;
}

bool RunMainSetup(
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
