#pragma once

#include "Setup/SetupData.h"
#include "Utils/MsXmlUtils.h"

namespace MySetup
{
	namespace ConfigParser
	{
		void ParseXmlSetupScreens(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *setupScreens);

		void ParseXmlSetupScheme(
			__inout CSetupData *setupData,
			__in Utils::CMsXmlNode *setupScheme);
	}
}