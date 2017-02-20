#pragma once

#include "Utils/StringLoader.h"
#include <string>
#include <map>
#include <algorithm>

// Setup screens
#define SETUP_SCREEN_MAIN		0		// Main setup
#define SETUP_SCREEN_SPLASH		1		// Splash screen
#define SETUP_SCREEN_CONFIG		2		// Configurations
#define SETUP_SCREEN_EULA		3		// End User License Agreement
#define SETUP_SCREEN_FINAL		100		// Final screen

// Setup result
#define SETUP_NEXT			0
#define SETUP_OK			0
#define SETUP_CANCEL		1
#define SETUP_BACK			2

#define SETUP_ERROR			0x1000

#define MAX_LANG_ID			16

namespace MySetup
{
	typedef std::map<std::string, std::string> CPropertyMap;

	typedef struct
	{
		HMODULE hModule;
		_tstring resType;
		_tstring resName;		

		bool isEmpty()
		{
			return resName.empty() && resType.empty();
		}
	} RESOURCE_ENTRY;

	class CSetupData
	{
	public:
		CSetupData();
		~CSetupData();
	private:
		// Class data
		CPropertyMap m_SetupConfig;
		std::map<std::string, CPropertyMap> m_ScreenConfig;
		
		bool m_ShouldHideBorder; // Hide setup screen's border
		bool m_ShouldBeTopMost; // Set all setup screens to topmost
		bool m_CoudBeBack; // Check whether we could be back to the previous setup step
		

		std::string m_LanguageID; // ID of language being used for GUI

		std::vector<int> m_SetupScreens; // List of setup screens
		bool m_ShouldShowFinalScreen; // Show final screen indicating that setup succeeded

		Utils::CStringLoader *m_StringLoader;

		// URL for online installer
		_tstring m_DownloadUrlX86;
		_tstring m_DownloadUrlAmd64;

		
		// Offline installer resource
		RESOURCE_ENTRY m_OfflineInstallerX86;
		_tstring m_OfflineInstallerCompressionX86;

		RESOURCE_ENTRY m_OfflineInstallerAmd64;		
		_tstring m_OfflineInstallerCompressionAmd64;

		bool m_ShouldManuallyVerifyServer;

		std::vector<RESOURCE_ENTRY> m_OnlineInstallerTrustedCERTs;
	public:
		// Set user configuration
		void SetUserConfig(
			__in const char *configName,
			__in_opt const char *configValue = NULL);

		// Get user configuration
		// Return true if the configuration has been set, false otherwise
		bool GetUserConfig(
			__in const char *configName,
			__out std::string &configValue);
		
		// Get/set properties
		void SetHideBorder(bool hideBorder);
		bool ShouldHideBorder();

		void SetTopMost(bool topmost);
		bool ShouldBeTopMost();

		void SetCouldBeBack(bool couldBeBack);
		bool CouldBeBack();

		// Add a setup screen
		void AddSetupScreen(int screenID);

		void SetShowFinalScreen(bool showFinalScreen);

		// Get/set screen config
		void SetScreenConfig(
			__in const char *screenID,
			__in const char *configName,
			__in const char *configValue);

		bool GetScreenConfig(
			__in const char *screenID,
			__in const char *configName,
			__out std::string &configValue);

		void InitStringLoader(
			__in HMODULE resModule,
			__in LPCTSTR resType,
			__in LPCTSTR resName);

		std::vector<int> GetSetupScreens();

		// String resource methods
		Utils::CStringLoader *GetStringLoader();
		std::string GetLanguageID();

		// Installer configuration
		void SetDownloadUrlX86(__in LPCTSTR pszUrl);
		_tstring GetDownloadUrlX86();

		void SetDownloadUrlAmd64(__in LPCTSTR pszUrl);
		_tstring GetDownloadUrlAmd64();


		void SetOfflineInstallerX86(const RESOURCE_ENTRY &res);
		RESOURCE_ENTRY GetOfflineInstallerX86();

		void SetOfflineInstallerAmd64(const RESOURCE_ENTRY &res);
		RESOURCE_ENTRY GetOfflineInstallerAmd64();
			

		void SetOfflineInstallerCompressionX86(LPCTSTR compression);
		LPCTSTR GetOfflineInstallerCompressionX86();

		void SetOfflineInstallerCompressionAmd64(LPCTSTR compression);
		LPCTSTR GetOfflineInstallerCompressionAmd64();

		void SetManuallyVerifyServer(bool manually);
		bool ShouldManuallyVerifyServer();

		void AddOnlineInstallerTrustedCERT(const RESOURCE_ENTRY &res);

		std::vector<RESOURCE_ENTRY> GetOnlineInstallerTrustedCERTs();
	};
}