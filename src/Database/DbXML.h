#pragma once
#include "AbstractORM.h"
#include <vector>
#include "Model/AmbListXML.h"

class DbXML : public AbstractORM
{
	std::vector<AmbListXML> getAmbListXML(Date from, Date to, std::string RZICode, std::string LPK);
};

