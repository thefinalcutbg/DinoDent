#pragma once
#include "Model/Dental/ProcedureCode.h"

struct ProcedureListElement
{
	ProcedureCode code;
	bool nhif = false;
	bool favourite = false;
	double price = 0;
};
