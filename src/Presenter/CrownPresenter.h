#pragma once
#include "AbstractSubPresenter.h"
#include "View/Interfaces/AbstractRangeEdit.h"
#include "Model/Validators/ProcedureValidators.h"
#include "Model/Dental/ToothUtils.h"

class ToothContainer;

class CrownPresenter : public AbstractSubPresenter
{
	const ToothContainer& teeth;
	const std::vector<Tooth*>& selectedTeeth;

	BridgeRangeValidator range_validator;
	std::string m_ksmpOther;

	bool m_bridgeSelected;

	int tooth_begin;
	int tooth_end;

	std::string bridgeDiagnosis;

	std::string getDiagnosis(const Tooth& tooth);
	std::string getBridgeDiagnosis();

	bool firstTimeSelected = true;


public:
	CrownPresenter(const std::vector<Tooth*>& selectedTeeth, const ToothContainer& teeth);
	

	void rangeChanged(int begin, int end) override;
	void bridgeChecked(bool checked) override;
	
	ConstructionRange getInitialRange();
	// Inherited via ManipulationPresenter
	void setAdditionalTemplateParameters() override;
	bool additionalValidation() override;
	std::vector<Procedure> getProcedures() override;
};

