#pragma once
#include <TinyXML/tinyxml.h>
#include "Model/Dental/Procedure.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/HisSnapshot.h"
#include "Model/Dental/AmbList.h"
#include "Model/Patient.h"
#include "Model/Dental/HISDental.h"

class TiXmlDocument;

namespace HISHistoryAlgorithms
{
	std::vector<Procedure> getProcedures(TiXmlDocument& doc);
	std::vector<HisSnapshot> getDentalHistory(TiXmlDocument& doc);

	//expects element with array of nhis:tooth elements:
	ToothContainer getToothStatus(const TiXmlElement& node);
	HISToothContainer getHisToothContainer(const TiXmlElement& node);
	
};