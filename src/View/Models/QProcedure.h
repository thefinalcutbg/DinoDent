#pragma once

#include <QString>
#include "Model/Dental/Procedure.h"

struct QProcedure
{
	QProcedure(const Procedure& p);
	QString date;
	QString diagnosis;
	int tooth{ -1 };
	QString procedureName;
	int code{ -1 };
	QString ksmp;
	Procedure::FinancingSource fsource{ Procedure::FinancingSource::Patient };
	//QString price;
	QString doctor;
	QString notes;
};

