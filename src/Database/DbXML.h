#pragma once
#include "AbstractORM.h"
#include <vector>
#include "Model/XML/AmbListXML.h"

class DbXML : public AbstractORM
{
public:
	std::vector<AmbListXML> getAmbListXML(int month, int year, std::string RZICode, std::string LPK);
};

