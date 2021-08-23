#pragma once

#include <QString>
#include "Model/Procedure/Procedure.h"

struct QProcedure
{
	QProcedure(const Procedure& p);
	QString date;
	QString diagnosis;
	int tooth{ -1 };
	QString procedureName;
	int code{ -1 };
	bool nzok{ false };
	QString price;
	QString doctor;
};

