#include "stdafx.h"

#include "Utils/StringUtils.h"
#include "ConfigParser.h"

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
	if (setupText->GetAttribute(_T("res_type"), textResType) &&
		setupText->GetAttribute(_T("res_name"), textResName))
		setupData->InitStringLoader(NULL, textResType.c_str(), textResName.c_str());
}

void ConfigParser::ParseXmlSetupOfflineInstaller(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *offlineInstaller)
{
	if (setupData == NULL || offlineInstaller == NULL) return;

	// Parse offline installers
	RESOURCE_ENTRY genericRes, x86Res, amd64Res;	
	_tstring genericCompression, x86Compression, amd64Compression;

	for (Utils::CMsXmlNode *child : offlineInstaller->GetChildren())
	{
		_tstring childName = child->GetName().c_str();
		LPCTSTR pChildName = childName.c_str();

		if (_tcsicmp(pChildName, _T("installer")) == 0)
		{
			// Parse the installer entry
			_tstring resName;
			_tstring resType;
			_tstring platform;
			_tstring compression;
			if (child->GetAttribute(_T("res_name"), resName) &&
				child->GetAttribute(_T("res_type"), resType))
			{
				OsPlatform osPlatform = PlatformUnknown;
				if (child->GetAttribute(_T("platform"), platform))
					osPlatform = ConfigParser::ParsePlatform(platform.c_str());
				child->GetAttribute(_T("compression"), compression);

				switch (osPlatform)
				{
				case PlatformAmd64:
					amd64Res.resType = resType;
					amd64Res.resName = resName;
					amd64Compression = compression;
					break;
				case PlatformX86:
					x86Res.resType = resType;
					x86Res.resName = resName;
					x86Compression = compression;
					break;
				default:
					genericRes.resName = resName;
					genericRes.resType = resType;
					genericCompression = compression;
					break;
				}
			}		
		}

		delete child;
	}

	// If there is no installer for the specific platforms, use the generic installer instead
	if (amd64Res.isEmpty())
	{
		setupData->SetOfflineInstallerAmd64(genericRes);
		setupData->SetOfflineInstallerCompressionAmd64(genericCompression.c_str());
	}
	else
	{
		setupData->SetOfflineInstallerAmd64(amd64Res);
		setupData->SetOfflineInstallerCompressionAmd64(amd64Compression.c_str());
	}

	if (x86Res.isEmpty())
	{
		setupData->SetOfflineInstallerX86(genericRes);
		setupData->SetOfflineInstallerCompressionX86(genericCompression.c_str());
	}
	else
	{
		setupData->SetOfflineInstallerX86(x86Res);
		setupData->SetOfflineInstallerCompressionX86(x86Compression.c_str());
	}
		
}

void ConfigParser::ParseXmlSetupOnlineInstaller(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *onlineInstaller)
{
	if (setupData == NULL || onlineInstaller == NULL) return;

	// Parse online installers
	_tstring genericUrl;
	_tstring x86Url;
	_tstring amd64Url;

	std::vector<RESOURCE_ENTRY> trustedCERTs;

	for (Utils::CMsXmlNode *child : onlineInstaller->GetChildren())
	{
		_tstring childName = child->GetName().c_str();
		LPCTSTR pChildName = childName.c_str();

		if (_tcsicmp(pChildName, _T("installer")) == 0)
		{
			// Parse the installer entry
			_tstring url;
			_tstring platform;
			_tstring compression;

			if (child->GetAttribute(_T("url"), url))
			{
				OsPlatform osPlatform = PlatformUnknown;
				if (child->GetAttribute(_T("platform"), platform))
					osPlatform = ConfigParser::ParsePlatform(platform.c_str());
				child->GetAttribute(_T("compression"), compression);

				switch (osPlatform)
				{
				case PlatformAmd64:
					amd64Url = url;
					break;
				case PlatformX86:
					x86Url = url;
					break;
				default:
					genericUrl = url;
					break;
				}
			}
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
	if (x86Url.empty())
		setupData->SetDownloadUrlX86(genericUrl.c_str());
	else
		setupData->SetDownloadUrlX86(x86Url.c_str());

	if (amd64Url.empty())
		setupData->SetDownloadUrlAmd64(genericUrl.c_str());
	else
		setupData->SetDownloadUrlAmd64(amd64Url.c_str());

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
