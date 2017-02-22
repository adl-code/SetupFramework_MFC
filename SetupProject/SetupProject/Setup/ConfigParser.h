#pragma once

#include "Setup/SetupData.h"
#include "Utils/MsXmlUtils.h"

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