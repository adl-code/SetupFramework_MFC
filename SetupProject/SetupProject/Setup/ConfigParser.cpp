#include "stdafx.h"

#include "Utils/StringUtils.h"
#include "Setup/ConfigParser.h"
#include "Setup/ConfigData.h"

using namespace MySetup;

void ConfigParser::ParseXmlSetupScreens(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *setupScreens)
{
	if (setupData == NULL || setupScreens == NULL) return;

	for (Utils::CMsXmlNode *child : setupScreens->GetChildren())
	{
		_tstring screenId;
				
		if (_tcsicmp(child->GetName().c_str(), _T("screen")) ||
			!child->GetAttribute(_T("id"), screenId) || screenId.empty())
		{
			delete child;
			continue;
		}

		std::string theId = TSTRING_TO_UTF8(screenId);
		LPCSTR pszId = theId.c_str();
		for (auto prop: child->GetAttributes())
		{
			setupData->SetScreenConfig(
				theId.c_str(),
				TSTRING_TO_UTF8(prop.first).c_str(),
				TSTRING_TO_UTF8(prop.second).c_str());
		}

		if (_stricmp(pszId, SCREEN_ID_SPLASH) == 0)
			setupData->AddSetupScreen(SETUP_SCREEN_SPLASH);
		else if (_stricmp(pszId, SETUP_SCREEN_ID_EULA) == 0)
			setupData->AddSetupScreen(SETUP_SCREEN_EULA);
		else if (_stricmp(pszId, SETUP_SCREEN_ID_CONFIG) == 0)
			setupData->AddSetupScreen(SETUP_SCREEN_CONFIG);
					


		delete child;
	}
}

void ConfigParser::ParseXmlSetupText(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *setupText)
{
	if (setupData == NULL || setupText == NULL) return;

	// Load text resource
	_tstring textResType;
	_tstring textResName;
	_tstring langID;
	if (setupText->GetAttribute(_T("res_type"), textResType) &&
		setupText->GetAttribute(_T("res_name"), textResName))
		setupData->InitStringLoader(NULL, textResType.c_str(), textResName.c_str());

	if (setupText->GetAttribute(_T("lang_id"), langID))
		setupData->SetLanguageID(TSTRING_TO_UTF8(langID).c_str());
}

void ConfigParser::ParseXmlSetupOfflineInstaller(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *offlineInstaller)
{
	if (setupData == NULL || offlineInstaller == NULL) return;

	// Parse offline installers	
	CPropertyMap genericInstaller, x86Installer, amd64Installer;

	for (Utils::CMsXmlNode *child : offlineInstaller->GetChildren())
	{
		_tstring childName = child->GetName().c_str();
		LPCTSTR pChildName = childName.c_str();

		if (_tcsicmp(pChildName, _T("installer")) == 0)
		{
			// Parse the installer entry			
			_tstring platform;
			
			OsPlatform osPlatform = PlatformUnknown;
			if (child->GetAttribute(_T("platform"), platform))
				osPlatform = ConfigParser::ParsePlatform(platform.c_str());

			CPropertyMap *props;
			switch (osPlatform)
			{
			case PlatformAmd64:
				props = &amd64Installer;
				break;
			case PlatformX86:
				props = &x86Installer;
				break;
			default:
				props = &genericInstaller;
				break;
			}
			for (auto item : child->GetAttributes())
			{
				(*props)[TSTRING_TO_UTF8(item.first)] = TSTRING_TO_UTF8(item.second);
			}
		}

		delete child;
	}

	// If there is no installer for the specific platforms, use the generic installer instead
	if (amd64Installer.find("res_name") == amd64Installer.end() ||
		amd64Installer.find("res_type") == amd64Installer.end())
		amd64Installer = genericInstaller;
	
	if (x86Installer.find("res_name") == x86Installer.end() ||
		x86Installer.find("res_type") == x86Installer.end())
		x86Installer = genericInstaller;
		
	setupData->SetSetupConfig(SETUP_CFG_OFFLINE_INSTALLER_AMD64, amd64Installer);
	setupData->SetSetupConfig(SETUP_CFG_OFFLINE_INSTALLER_X86, x86Installer);
}

void ConfigParser::ParseXmlSetupOnlineInstaller(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *onlineInstaller)
{
	if (setupData == NULL || onlineInstaller == NULL) return;

	// Parse online installers
	CPropertyMap genericInstaller, x86Installer, amd64Installer;

	std::vector<RESOURCE_ENTRY> trustedCERTs;

	for (Utils::CMsXmlNode *child : onlineInstaller->GetChildren())
	{
		_tstring childName = child->GetName().c_str();
		LPCTSTR pChildName = childName.c_str();

		if (_tcsicmp(pChildName, _T("installer")) == 0)
		{
			// Parse the installer entry
			CPropertyMap *props;
			_tstring platform;

			OsPlatform osPlatform = PlatformUnknown;
			if (child->GetAttribute(_T("platform"), platform))
				osPlatform = ConfigParser::ParsePlatform(platform.c_str());			
			
			switch (osPlatform)
			{
			case PlatformAmd64:
				props = &amd64Installer;
				break;
			case PlatformX86:
				props = &x86Installer;
				break;
			default:
				props = &genericInstaller;
				break;
			}
			for (auto i : child->GetAttributes())
				(*props)[TSTRING_TO_UTF8(i.first)] = TSTRING_TO_UTF8(i.second);
			
		}
		else if (_tcsicmp(pChildName, _T("trusted_certs")) == 0)
		{
			// Parse the trusted CERT entry
			for (Utils::CMsXmlNode *certEntry : child->GetChildren())
			{
				_tstring certResName, certResType;
				if (_tcsicmp(certEntry->GetName().c_str(), _T("cert")) == 0 &&
					certEntry->GetAttribute(_T("res_name"), certResName) &&
					certEntry->GetAttribute(_T("res_type"), certResType))
				{
					RESOURCE_ENTRY certResource = { NULL, certResType, certResName };
					trustedCERTs.push_back(certResource);
				}
				delete certEntry;
			}
		}

		delete child;
	}

	// If there is no installer for the specific platforms, use the generic installer instead
	if (x86Installer.find("url") == x86Installer.end())
		x86Installer = genericInstaller;
	
	if (amd64Installer.find("url") == amd64Installer.end())
		amd64Installer = genericInstaller;

	setupData->SetSetupConfig(SETUP_CFG_ONLINE_INSTALLER_AMD64, amd64Installer);
	setupData->SetSetupConfig(SETUP_CFG_ONLINE_INSTALLER_X86, x86Installer);

	_tstring configValue;
	bool manualVerification = false;
	// Should we manually validate server?
	if (onlineInstaller->GetAttribute(_T("manually_verify_server"), configValue) &&
		Utils::ParseBoolConfig(TSTRING_TO_UTF8(configValue.c_str()).c_str(), manualVerification))
	{
		setupData->SetManuallyVerifyServer(manualVerification);
		if (manualVerification)
		{
			for (RESOURCE_ENTRY certRes : trustedCERTs)
				setupData->AddOnlineInstallerTrustedCERT(certRes);
		}
		
	}
}

void ConfigParser::ParseXmlSetupInstaller(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *installer)
{
	if (setupData == NULL || installer == NULL) return;

	for (Utils::CMsXmlNode* child : installer->GetChildren())
	{
		_tstring childName = child->GetName().c_str();
		LPCTSTR pChildName = childName.c_str();
		if (_tcsicmp(pChildName, _T("offline_installer")) == 0)
			ConfigParser::ParseXmlSetupOfflineInstaller(setupData, child);
		else if (_tcsicmp(pChildName, _T("online_installer")) == 0)
			ConfigParser::ParseXmlSetupOnlineInstaller(setupData, child);

		delete child;
	}
}

void ConfigParser::ParseXmlSetupScheme(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *setupScheme)

{
	if (setupData == NULL || setupScheme == NULL) return;

	// Parse setup screen
	std::vector<Utils::CMsXmlNode*> children = setupScheme->GetChildren();
	for (Utils::CMsXmlNode *child : children)
	{
		_tstring childName = child->GetName();
		LPCTSTR pChildName = childName.c_str();
		if (_tcsicmp(pChildName, _T("setup_screens")) == 0)
			ConfigParser::ParseXmlSetupScreens(setupData, child);
		else if (_tcsicmp(pChildName, _T("text")) == 0)
			ConfigParser::ParseXmlSetupText(setupData, child);
		else if (_tcsicmp(pChildName, _T("installer")) == 0)
			ConfigParser::ParseXmlSetupInstaller(setupData, child);


		delete child;
	}
}

MySetup::ConfigParser::OsPlatform MySetup::ConfigParser::ParsePlatform(__in LPCTSTR platformValue)
{
	if (platformValue == NULL) return PlatformUnknown;
	if (_tcsicmp(platformValue, _T("x86")) == 0)
		return PlatformX86;
	else if (_tcsicmp(platformValue, _T("x64")) == 0 ||
		_tcsicmp(platformValue, _T("amd64")) == 0 ||
		_tcsicmp(platformValue, _T("x86-64")) == 0 ||
		_tcsicmp(platformValue, _T("amd64")) == 0)
		return PlatformAmd64;

	return PlatformUnknown;
}
