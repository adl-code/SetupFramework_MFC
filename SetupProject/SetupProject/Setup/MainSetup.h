#pragma once
#include "Setup/SetupData.h"

namespace MySetup
{
	void InitSetup(
		__inout CSetupData* setupData);
	
	int RunSetup(
		__inout CSetupData *setupData);
}