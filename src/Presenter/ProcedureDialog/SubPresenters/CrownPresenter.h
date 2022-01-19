#pragma once
#include "AbstractSubPresenter.h"
#include "View/ProcedureDialog/ProcedureFields/ICrownView.h"
#include "Model/Validator/ProcedureValidators.h"
#include "Model/Tooth/ToothUtils.h"

class ToothContainer;

class CrownPresenter : public AbstractSubPresenter
{
	ICrownView* view;

	const ToothContainer& teeth;
	const std::vector<Tooth*>& selectedTeeth;

	BridgeRangeValidator range_validator;

	std::string m_bridgeDiagnosis;
	std::string m_bridgeRangeString;
	bool m_bridgeSelected;
	double m_bridgePrice;

	std::string getBridgeDiagnosis(int begin, int end, const ToothContainer& teeth);
	std::string getBridgeRangeName(int begin, int end, const ToothContainer& teeth);
	std::tuple<int, int> getInitialBridgeRange(const std::vector<Tooth*>& selectedTeeth);


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

