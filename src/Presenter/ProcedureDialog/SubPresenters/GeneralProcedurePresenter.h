#pragma once
#include "AbstractSubPresenter.h"

class GeneralProcedurePresenter : public AbstractSubPresenter
{
public:

	GeneralProcedurePresenter() : AbstractSubPresenter(ProcedureType::general) {}

	std::vector<Procedure> getProcedures() override {
		return{ AbstractSubPresenter::getProcedureCommonFields() };
	}
};

