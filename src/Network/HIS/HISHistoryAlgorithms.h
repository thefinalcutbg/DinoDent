#pragma once
#include <TinyXML/tinyxml.h>
#include "Model/Dental/Procedure.h"
#include "Model/Dental/ToothContainer.h"

class TiXmlDocument;

namespace HISHistoryAlgorithms
{
	std::vector<Procedure> getProcedures(TiXmlDocument& doc);
	ToothContainer getToothStatus(TiXmlDocument& doc);
};