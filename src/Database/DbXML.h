#pragma once
#include <vector>
#include "Model/XML/AmbListXML.h"

namespace DbXML
{
	std::vector<AmbListXML> getAmbListXML(int month, int year, std::string RZICode, std::string LPK);
}

