#pragma once

#pragma once
#include "AbstractSubPresenter.h"

class ToothProcedurePresenter : public AbstractSubPresenter
{

protected:
	const std::vector<Tooth*>& selectedTeeth;

	//virtual std::string getDiagnosis();
	//virtual void modifyProcedure(Procedure& p) {}; //override in subclasses
	//this function is called, when the procedures are generated



public:

	ToothProcedurePresenter(const std::vector<Tooth*>& selectedTeeth, ProcedureType t = ProcedureType::any);
	
	void setAdditionalTemplateParameters() override {

		if (selectedTeeth.empty()) {
			view->setErrorMsg("Изберете поне един зъб");
			return;
		}

		view->setLayout(ICommonFields::General);
	}

	bool additionalValidation() override { return !selectedTeeth.empty(); }

	std::vector<Procedure> getProcedures() override;
};


class EndoPresenter : public ToothProcedurePresenter
{

	std::string getDiagnosis(const Tooth& tooth);

public:
	EndoPresenter(const std::vector<Tooth*>& selectedTeeth);
	std::vector<Procedure> getProcedures() override;
};


class ExtractionPresenter : public ToothProcedurePresenter
{
	std::string getDiagnosis(const Tooth& tooth);

public:
	ExtractionPresenter(const std::vector<Tooth*>& selectedTeeth);
	std::vector<Procedure> getProcedures() override;
};

class ImplantPresenter : public ToothProcedurePresenter
{
	std::string getDiagnosis(const Tooth& tooth);

public:
	ImplantPresenter(const std::vector<Tooth*>& selectedTeeth);
};