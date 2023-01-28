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

class NhifAnesthesiaPresenter : public AbstractSubPresenter
{
public:
	NhifAnesthesiaPresenter() : AbstractSubPresenter(ProcedureType::nhif_anesthesia) {}

	void setAdditionalTemplateParameters() override {

		view->setLayout(ICommonFields::Anesthesia);
	}

	bool additionalValidation() override { return true; }

	std::vector<Procedure> getProcedures() override {
		auto result = AbstractSubPresenter::getProcedureCommonFields();
		result.result = Anesthesia(view->getMinutes());
		return { result };
	};
};