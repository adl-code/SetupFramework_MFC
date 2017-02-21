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
, m_ShouldManuallyVerifyServer(false)
{
	// Add the main setup screen first
	m_SetupScreens.push_back(SETUP_SCREEN_MAIN);

	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

MySetup::CSetupData::~CSetupData()
{
	if (m_StringLoader) delete m_StringLoader;
	
	if (m_hStopEvent) CloseHandle(m_hStopEvent);
}

void MySetup::CSetupData::SetUserConfig(
	__in const char *configName,
	__in_opt const char *configValue /*= NULL*/)
{
	if (configName == NULL) return;

	std::string cfg = Utils::MakeLowerA(configName);
	m_UserConfig[cfg] = configValue ? configValue : "";
}

bool MySetup::CSetupData::GetUserConfig(
	__in const char *configName,
	__out std::string &configValue)
{
	if (configName == NULL) return false;

	std::string cfg = Utils::MakeLowerA(configName);

	CPropertyMap::iterator itor = m_UserConfig.find(cfg);
	return itor == m_UserConfig.end() ? false : (configValue = itor->second, true);
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

// Get/set setup config
void MySetup::CSetupData::SetSetupConfig(
	__in const char *section,
	__in const char *configName,
	__in const char *configValue)
{
	if (section == NULL || configName == NULL) return;

	std::string sectionName = Utils::MakeLowerA(section);

	std::string cfg = Utils::MakeLowerA(configName);

	std::string val = configValue ? configValue : "";

	std::map<std::string, CPropertyMap>::iterator itor = m_SetupConfig.find(sectionName);
	if (itor == m_SetupConfig.end())
	{
		CPropertyMap props;
		props[cfg] = val;
		m_SetupConfig[sectionName] = props;
	}
	else
	{
		itor->second[cfg] = val;
	}
}

void MySetup::CSetupData::SetSetupConfig(
	__in const char *section,
	__in const CPropertyMap& config)
{
	if (section == NULL) return;

	std::string sectionName = Utils::MakeLowerA(section);
	std::map<std::string, CPropertyMap>::iterator itor = m_SetupConfig.find(sectionName);
	if (itor == m_SetupConfig.end())
		m_SetupConfig[sectionName] = config;	
	else
	{
		for (auto i : config)
			itor->second[i.first] = i.second;
	}
}

bool MySetup::CSetupData::GetSetupConfig(
	__in const char *section,
	__in const char *configName,
	__out std::string &configValue)
{
	if (section == NULL || configName == NULL) return false;

	std::string screen = Utils::MakeLowerA(section);

	std::string cfg = Utils::MakeLowerA(configName);

	std::map<std::string, CPropertyMap>::iterator itor = m_SetupConfig.find(screen);
	if (itor == m_SetupConfig.end())
		return false;

	CPropertyMap::iterator mapItor = itor->second.find(cfg);
	if (mapItor == itor->second.end())
		return false;

	configValue = mapItor->second;
	return true;
}

bool MySetup::CSetupData::GetSetupConfig(
	__in const char *section,
	__out CPropertyMap& config)
{
	if (section == NULL) return false;

	std::string sectionName = Utils::MakeLowerA(section);
	std::map<std::string, CPropertyMap>::iterator itor = m_SetupConfig.find(sectionName);
	if (itor == m_SetupConfig.end())
		return false;

	for (auto i : itor->second)
		config[i.first] = i.second;
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

void MySetup::CSetupData::SetManuallyVerifyServer(bool manually)
{
	m_ShouldManuallyVerifyServer = manually;
}

bool MySetup::CSetupData::ShouldManuallyVerifyServer()
{
	return m_ShouldManuallyVerifyServer;
}

void MySetup::CSetupData::AddOnlineInstallerTrustedCERT(const RESOURCE_ENTRY &res)
{
	m_OnlineInstallerTrustedCERTs.push_back(res);
}

std::vector<RESOURCE_ENTRY> MySetup::CSetupData::GetOnlineInstallerTrustedCERTs()
{
	return m_OnlineInstallerTrustedCERTs;
}

void MySetup::CSetupData::StopInstall()
{
	if (m_hStopEvent) SetEvent(m_hStopEvent);
}

bool MySetup::CSetupData::ShouldStop(DWORD timeout /* milliseconds */)
{
	if (m_hStopEvent == NULL) return true;

	return WaitForSingleObject(m_hStopEvent, timeout) != WAIT_TIMEOUT;
}

