#include "stdafx.h"

#include "Utils/StringUtils.h"
#include "ConfigParser.h"

using namespace MySetup;

void ConfigParser::ParseXmlSetupScreens(
	__inout CSetupData *setupData,
	__in Utils::CMsXmlNode *setupScreens)
{
	if (setupData == NULL || setupScreens == NULL) return;
	std::vector<Utils::CMsXmlNode*> children = setupScreens->GetChildren();
	for (Utils::CMsXmlNode *child : children)
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

		if (_stricmp(pszId, "splash") == 0)
			setupData->AddSetupScreen(SETUP_SCREEN_SPLASH);

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
		{
			ConfigParser::ParseXmlSetupScreens(setupData, child);
		}



		delete child;
	}
}