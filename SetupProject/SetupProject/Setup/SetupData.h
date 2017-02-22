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

enum LogLevel
{	
	LogDebug = 0,
	LogInfo,
	LogWarning,
	LogError,
};

namespace MySetup
{
	typedef std::map<std::string, std::string> CPropertyMap;

	class CSetupObserver
	{
	public:
		virtual ~CSetupObserver() {};
	public:
		virtual void UpdateStatus(
			__in LPCTSTR statusText) = 0;

		virtual void UpdateProgress(
			DWORD percentage) = 0;

		virtual void OnError(
			__in LPCTSTR errorText) = 0;

		virtual void OnFinish(int setupResult) = 0;

	};

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
		CPropertyMap m_UserConfig;

		
		std::map<std::string, CPropertyMap> m_SetupConfig;

		std::map<std::string, CPropertyMap> m_ScreenConfig;
		
		bool m_CoudBeBack; // Check whether we could be back to the previous setup step
		

		std::string m_LanguageID; // ID of language being used for GUI

		std::vector<int> m_SetupScreens; // List of setup screens
		bool m_ShouldShowFinalScreen; // Show final screen indicating that setup succeeded

		Utils::CStringLoader *m_StringLoader;
				
		// Data for manual server verification
		bool m_ShouldManuallyVerifyServer;
		std::vector<RESOURCE_ENTRY> m_OnlineInstallerTrustedCERTs;

		HANDLE m_hStopEvent;
		HANDLE m_hPauseEvent;

		UINT m_LogLevel;
	public:
		// Set user configuration
		void SetUserConfig(
			__in const char *configName,
			__in_opt const char *configValue = NULL);

		// Get user configuration
		// Return true if the configuration has been set, false otherwise
		bool GetUserConfig(
			__in const char *configName,
			__out std::string &configValue) const;
		
		// Get/set properties
	
		void SetCouldBeBack(bool couldBeBack);
		bool CouldBeBack() const;

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
			__out std::string &configValue) const;


		// Get/set setup config
		void SetSetupConfig(
			__in const char *section,
			__in const char *configName,
			__in const char *configValue);

		void SetSetupConfig(
			__in const char *section,
			__in const CPropertyMap& config);

		bool GetSetupConfig(
			__in const char *section,
			__in const char *configName,
			__out std::string &configValue) const;

		bool GetSetupConfig(
			__in const char *section,
			__out CPropertyMap& config) const;
		

		void InitStringLoader(
			__in HMODULE resModule,
			__in LPCTSTR resType,
			__in LPCTSTR resName);

		std::vector<int> GetSetupScreens() const;

		// String resource methods
		const Utils::CStringLoader *GetStringLoader();
		std::string GetLanguageID() const;

		void SetManuallyVerifyServer(bool manually);
		bool ShouldManuallyVerifyServer() const;

		void AddOnlineInstallerTrustedCERT(const RESOURCE_ENTRY &res);

		std::vector<RESOURCE_ENTRY> GetOnlineInstallerTrustedCERTs() const;
				
		void Stop();
		bool ShouldStop(DWORD timeout /* milliseconds */) const;

		void LogA(UINT logLevel, const char *fmt, ...);

		void SetLogLevel(UINT logLevel);
		UINT GetLogLevel() const;

		void PauseSetup();
		void ResumeSetup();
		bool ShouldPause(DWORD timeout /* milliseconds */) const;

		// Shortcut to its own internal string loader
		_tstring GetString(__in const char *textID, __in const char *langID = NULL) const;
		_tstring GetString(__in std::string &textID, __in const char *langID = NULL) const;
	};
}