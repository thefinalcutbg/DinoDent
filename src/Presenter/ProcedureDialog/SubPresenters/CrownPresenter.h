#pragma once
#include "AbstractSubPresenter.h"
#include "View/ProcedureDialog/ProcedureFields/ICrownView.h"
#include "Model/Validator/ProcedureValidators.h"
#include "Model/Tooth/ToothUtils.h"
#include "BridgeHelper.h"
class ToothContainer;


class CrownPresenter : public AbstractSubPresenter
{

	//a variable which holds the bridge data


	ICrownView* view;

	const ToothContainer& teeth;
	const std::vector<Tooth*>& selectedTeeth;

	BridgeRangeValidator range_validator;
	std::string m_ksmpOther;

	BridgeHelper bridgeLogic;

	bool m_bridgeSelected;

	std::string getDiagnosis(const Tooth& tooth);

public:
	CrownPresenter(const std::vector<Tooth*>& selectedTeeth, const ToothContainer& teeth);
	
	void setView(ICrownView* view);
	void rangeChanged(int begin, int end);
	void selectAsBridge(bool checked);

	// Inherited via ManipulationPresenter
	void setProcedureTemplate(const ProcedureTemplate& m) override;
	bool isValid() override;
	std::vector<Procedure> getProcedures() override;
};

