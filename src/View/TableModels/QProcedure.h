#pragma once

#include <QString>
#include "Model/Dental/Procedure.h"

struct QProcedure
{
	QProcedure(const Procedure& p);
    QProcedure(const QString& notes): notes(notes) {}; //for print purposes
	QString date;
	QString diagnosis;
	QString tooth{};
	QString procedureName;
	QString nhifCode;
	QString ACHICode;
	FinancingSource fsource{ FinancingSource::Patient };
	//QString price;
	QString doctor;
	QString notes;
	Procedure::DatabaseSource db_source;
};

