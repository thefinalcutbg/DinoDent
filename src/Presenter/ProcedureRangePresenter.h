#pragma once

#include "AbstractSubPresenter.h"

#include "Model/Dental/Tooth.h"
#include "View/Interfaces/AbstractRangeEdit.h"
#include "Model/Validators/ProcedureValidators.h"

class ToothContainer;

class ProcedureRangePresenter : public AbstractSubPresenter
{

	BridgeRangeValidator range_validator;

	const std::vector<Tooth*>& selectedTeeth;

	int begin;
	int end;

	void rangeChanged(int begin, int end) override;

public:
	ProcedureRangePresenter(const std::vector<Tooth*>& selectedTeeth, ProcedureType type);
	
	void setAdditionalTemplateParameters() override;

	bool additionalValidation() override;
	std::vector<Procedure> getProcedures() override;;
};