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



void MySetup::InitSetup(
	__inout CSetupData *setupData)
{
	// Initialize string loader and string resource
	setupData->InitStringLoader(NULL, _T("SETUP"), _T("TEXT"));

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

	// Configure dialogs to display
	//setupData->AddSetupScreen(SETUP_SCREEN_SPLASH);		// Display splash
	setupData->AddSetupScreen(SETUP_SCREEN_CONFIG);		// Configuration window
	setupData->AddSetupScreen(SETUP_SCREEN_EULA);			// Then EULA
	
	setupData->SetHideBorder(true);
	setupData->SetTopMost(true);

	
	Utils::CHttpDownloader downloader;
	DWORD httpCode;
	_tstring httpMsg;
	LPCTSTR pszUrl = _T("http://192.168.1.33/version/ClientList.txt");
	downloader.AddAcceptType(_T("text/*"));
	downloader.Download(pszUrl, NULL, NULL, &httpCode, &httpMsg);
	
}


int MySetup::RunSetup(
	__inout CSetupData *setupData)
{
	if (setupData == NULL)
		return SETUP_ERROR;

	// Loop through all setup steps
	int iStep = 0;
	int prevStep = -1;
	std::vector<int> setupScreens = setupData->GetSetupScreens();
	int screenCnt = (int)setupScreens.size();

	while (iStep < screenCnt)
	{
		CBaseSetupDlg *setupDlg = NULL;
		std::string screenId;
		switch (setupScreens[iStep])
		{
		case SETUP_SCREEN_SPLASH:
			// Display splash screen			
			setupDlg = static_cast<CBaseSetupDlg*>(new CSplashDlg(_T("SPLASH_DLG")));
			break;
		case SETUP_SCREEN_CONFIG:
			setupDlg = static_cast<CBaseSetupDlg*>(new CConfigDlg(_T("CONFIG_DLG")));
			break;
		case SETUP_SCREEN_EULA:
			// Display EULA
			setupDlg = static_cast<CBaseSetupDlg*>(new CEulaDlg(_T("EULA_DLG")));
			break;
		case SETUP_SCREEN_MAIN:
			setupDlg = static_cast<CBaseSetupDlg*>(new CMainSetupDlg(_T("MAIN_SETUP_DLG")));
			break;
		default:
			// Unprocessed steps
			return SETUP_CANCEL;
		}

		if (setupDlg == NULL)
			return SETUP_ERROR;
		
		
		setupData->SetCouldBeBack(prevStep != SETUP_SCREEN_SPLASH && setupScreens[iStep] != SETUP_SCREEN_MAIN);

		int result = setupDlg->DoSetup(setupData);
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

