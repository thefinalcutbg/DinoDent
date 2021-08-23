#pragma once

#include "GeneralMPresenter.h"

class TeethMPresenter : public GeneralMPresenter
{

protected:
	const std::vector<Tooth*>* const selectedTeeth;
	bool noTeethSelected;
public:
	TeethMPresenter(const std::vector<Tooth*>& selectedTeeth);

	virtual void setManipulationTemplate(const ProcedureTemplate& m) override;

	bool isValid() override;

	std::vector<Procedure> getProcedures() override;
	
};