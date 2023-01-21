#pragma once
#include "AbstractSubPresenter.h"

class GeneralProcedurePresenter : public AbstractSubPresenter
{
public:

	GeneralProcedurePresenter() : AbstractSubPresenter(ProcedureType::general) {}

	void setAdditionalTemplateParameters() override {

		view->setLayout(ICommonFields::General);
	}

	bool additionalValidation() override { return true; }

	std::vector<Procedure> getProcedures() override {
		return{ AbstractSubPresenter::getProcedureCommonFields() };
	}
};

