#pragma once

#pragma once
#include "AbstractSubPresenter.h"

class ToothProcedurePresenter : public AbstractSubPresenter
{

	const std::vector<Tooth*>& selectedTeeth;

	//virtual std::string getDiagnosis();
	//virtual void modifyProcedure(Procedure& p) {}; //override in subclasses
	//this function is called, when the procedures are generated

public:

	ToothProcedurePresenter(const std::vector<Tooth*>& selectedTeeth);
	void setProcedureTemplate(const ProcedureTemplate& m) override;
	std::vector<Procedure> getProcedures() override;
};

