#pragma once
#include <TinyXML/tinyxml.h>
#include "Model/Dental/Procedure.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/HisSnapshot.h"
#include "Model/Dental/AmbList.h"
#include "Model/Patient.h"

class TiXmlDocument;

namespace HISHistoryAlgorithms
{
	std::vector<Procedure> getProcedures(TiXmlDocument& doc);
	ToothContainer getToothStatus(TiXmlDocument& doc);
	std::vector<HisSnapshot> getDentalHistory(TiXmlDocument& doc);
	std::tuple<AmbList, Patient> parseList(TiXmlDocument& doc);
};