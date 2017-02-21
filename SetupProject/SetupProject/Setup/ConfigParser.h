#pragma once

#include "Setup/SetupData.h"
#include "Utils/MsXmlUtils.h"

#define SCREEN_ID_SPLASH "splash"
#define SETUP_SCREEN_ID_EULA "eula"
#define SETUP_SCREEN_ID_CONFIG "config"
#define SETUP_SCREEN_ID_MAIN "main"


#define SETUP_CFG_ONLINE_INSTALLER_X86 "online_installer_x86"
#define SETUP_CFG_ONLINE_INSTALLER_AMD64 "online_installer_amd64"
#define SETUP_CFG_OFFLINE_INSTALLER_X86	"offline_installer_x86"
#define SETUP_CFG_OFFLINE_INSTALLER_AMD64 "offline_installer_amd64"

#define SETUP_CFG_RESOURCE_NAME	"res_name"
#define SETUP_CFG_RESOURCE_TYPE "res_type"

#define SID_DOWNLOADING_INSTALLER "downloading_installer"
#define SID_EXTRACTING_INSTALLER "extracting_installer"

namespace MySetup
{
	namespace ConfigParser
	{
		enum OsPlatform
		{
			PlatformUnknown = 0,
			PlatformX86,
			PlatformAmd64,
		};
		void ParseXmlSetupScreens(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *setupScreens);

		void ParseXmlSetupText(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *setupText);

		void ParseXmlSetupOfflineInstaller(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *offlineInstaller);

		void ParseXmlSetupOnlineInstaller(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *onlineInstaller);

		void ParseXmlSetupInstaller(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *installer);

		void ParseXmlSetupScheme(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *setupScheme);

		OsPlatform ParsePlatform(__in LPCTSTR platformValue);
	}
}