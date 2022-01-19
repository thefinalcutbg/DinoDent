#pragma once
#include "AbstractSubPresenter.h"

class GeneralProcedurePresenter : public AbstractSubPresenter
{
public:
	std::vector<Procedure> getProcedures() override {
		return{ AbstractSubPresenter::getProcedureCommonFields() };
	}
};

