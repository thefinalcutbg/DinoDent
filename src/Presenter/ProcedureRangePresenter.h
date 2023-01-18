#pragma once

#include "AbstractSubPresenter.h"

#include "Model/Dental/Tooth.h"
#include "View/Interfaces/AbstractRangeEdit.h"

class ToothContainer;

class ProcedureRangePresenter : public AbstractSubPresenter
{

	AbstractRangeEdit* view{ nullptr };

	const std::vector<Tooth*>& selectedTeeth;

	int begin;
	int end;

public:
	ProcedureRangePresenter(const std::vector<Tooth*>& selectedTeeth, ProcedureType type);

	void setView(AbstractRangeEdit* view);

	std::vector<Procedure> getProcedures() override;;
};