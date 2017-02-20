#include "stdafx.h"
#include <algorithm>

#include "Setup/SetupData.h"

using namespace MySetup;

MySetup::CSetupData::CSetupData()
: m_CoudBeBack(false)
, m_ShouldShowFinalScreen(false)
, m_ShouldHideBorder(false)
, m_ShouldBeTopMost(false)
, m_StringLoader(NULL)
, m_OnlineInstaller(false)
, m_OfflineInstallerResTypeAmd64(NULL)
, m_OfflineInstallerResTypeX86(NULL)
, m_OfflineInstallerResNameAmd64(NULL)
, m_OfflineInstallerResNameX86(NULL)
{
	// Add the main setup screen first
	m_SetupScreens.push_back(SETUP_SCREEN_MAIN);
}

MySetup::CSetupData::~CSetupData()
{
	if (m_StringLoader)
		delete m_StringLoader;
	m_StringLoader = NULL;
}

void MySetup::CSetupData::SetUserConfig(
	__in const char *configName,
	__in_opt const char *configValue /*= NULL*/)
{
	if (configName == NULL) return;

	std::string cfg = Utils::MakeLowerA(configName);
	m_SetupConfig[cfg] = configValue ? configValue : "";
}

bool MySetup::CSetupData::GetUserConfig(
	__in const char *configName,
	__out std::string &configValue)
{
	if (configName == NULL) return false;

	std::string cfg = Utils::MakeLowerA(configName);

	CPropertyMap::iterator itor = m_SetupConfig.find(cfg);
	return itor == m_SetupConfig.end() ? false : (configValue = itor->second, true);
}

void MySetup::CSetupData::SetHideBorder(bool hideBorder)
{
	m_ShouldHideBorder = hideBorder;
}

bool MySetup::CSetupData::ShouldHideBorder()
{
	return m_ShouldHideBorder;
}

void MySetup::CSetupData::SetTopMost(bool topmost)
{
	m_ShouldBeTopMost = topmost;
}

bool MySetup::CSetupData::ShouldBeTopMost()
{
	return m_ShouldBeTopMost;
}

void MySetup::CSetupData::SetCouldBeBack(bool couldBeBack)
{
	m_CoudBeBack = couldBeBack;
}

bool MySetup::CSetupData::CouldBeBack()
{
	return m_CoudBeBack;
}

void MySetup::CSetupData::AddSetupScreen(int screenID)
{
	m_SetupScreens.insert(m_SetupScreens.end() - (m_ShouldShowFinalScreen ? 2 : 1), screenID);
}

void MySetup::CSetupData::SetShowFinalScreen(bool showFinalScreen)
{
	if (showFinalScreen && !m_ShouldShowFinalScreen)
		m_SetupScreens.push_back(SETUP_SCREEN_FINAL);

	if (!showFinalScreen && m_ShouldShowFinalScreen)
		m_SetupScreens.pop_back();
	m_ShouldShowFinalScreen = showFinalScreen;
	
}

// Get/set screen config
void MySetup::CSetupData::SetScreenConfig(
	__in const char *screenID,
	__in const char *configName,
	__in const char *configValue)
{
	if (screenID == NULL || configName == NULL) return;

	std::string screen = Utils::MakeLowerA(screenID);

	std::string cfg = Utils::MakeLowerA(configName);

	std::string val = configValue ? configValue : "";

	std::map<std::string, CPropertyMap>::iterator itor = m_ScreenConfig.find(screen);
	if (itor == m_ScreenConfig.end())
	{
		CPropertyMap props;
		props[cfg] = val;
		m_ScreenConfig[screen] = props;
	}
	else
	{
		itor->second[cfg] = val;
	}
}

bool MySetup::CSetupData::GetScreenConfig(
	__in const char *screenID,
	__in const char *configName,
	__out std::string &configValue)
{
	if (screenID == NULL || configName == NULL) return false;

	std::string screen = Utils::MakeLowerA(screenID);

	std::string cfg = Utils::MakeLowerA(configName);

	std::map<std::string, CPropertyMap>::iterator itor = m_ScreenConfig.find(screen);
	if (itor == m_ScreenConfig.end())
		return false;

	CPropertyMap::iterator mapItor = itor->second.find(cfg);
	if (mapItor == itor->second.end())
		return false;

	configValue = mapItor->second;
	return true;
}

void MySetup::CSetupData::InitStringLoader(
	__in HMODULE resModule,
	__in LPCTSTR resType,
	__in LPCTSTR resName)
{
	if (resType == NULL || resName == NULL) return;
	if (m_StringLoader)
		delete m_StringLoader;
	m_StringLoader = new Utils::CStringLoader(resModule, resType, resName);
}

std::vector<int> MySetup::CSetupData::GetSetupScreens()
{
	return m_SetupScreens;
}

Utils::CStringLoader * MySetup::CSetupData::GetStringLoader()
{
	return m_StringLoader;
}

std::string MySetup::CSetupData::GetLanguageID()
{
	return m_LanguageID;
}

void MySetup::CSetupData::SetOnlineInstallerMode(bool onlineInstaller)
{
	m_OnlineInstaller = onlineInstaller;
}

bool MySetup::CSetupData::IsOnlineInstaller()
{
	return m_OnlineInstaller;
}

void MySetup::CSetupData::SetDownloadUrlX86(__in LPCTSTR pszUrl)
{
	if (pszUrl) m_DownloadUrlX86 = pszUrl;
}

_tstring MySetup::CSetupData::GetDownloadUrlX86()
{
	return m_DownloadUrlX86;
}

void MySetup::CSetupData::SetDownloadUrlAmd64(__in LPCTSTR pszUrl)
{
	if (pszUrl) m_DownloadUrlAmd64 = pszUrl;
}

_tstring MySetup::CSetupData::GetDownloadUrlAmd64()
{
	return m_DownloadUrlAmd64;
}

void MySetup::CSetupData::SetOfflineInstallerResTypeX86(LPCTSTR resType)
{
	m_OfflineInstallerResTypeX86 = resType;
}

LPCTSTR MySetup::CSetupData::GetOfflineInstallerResTypeX86()
{
	return m_OfflineInstallerResTypeX86;
}

void MySetup::CSetupData::SetOfflineInstallerResTypeAmd64(LPCTSTR resType)
{
	m_OfflineInstallerResTypeAmd64 = resType;
}

LPCTSTR MySetup::CSetupData::GetOfflineInstallerResTypeAmd64()
{
	return m_OfflineInstallerResTypeAmd64;
}

void MySetup::CSetupData::SetOfflineInstallerResNameX86(LPCTSTR resName)
{
	m_OfflineInstallerResNameX86 = resName;
}

LPCTSTR MySetup::CSetupData::GetOfflineIntallerResNameX86()
{
	return m_OfflineInstallerResNameX86;
}

void MySetup::CSetupData::SetOfflineInstallerResNameAmd64(LPCTSTR resName)
{
	m_OfflineInstallerResNameAmd64 = resName;
}

LPCTSTR MySetup::CSetupData::GetOfflineIntallerResNameAmd64()
{
	return m_OfflineInstallerResNameAmd64;
}

