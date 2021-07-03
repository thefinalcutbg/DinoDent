#pragma once
#include "TeethMPresenter.h"
#include "View/ProcedureDialog/Widgets/CrownView/ICrownView.h"
#include "Model/Validator/ProcedureValidators.h"
#include "Model/Tooth/ToothUtils.h"

class CrownPresenter : public TeethMPresenter
{
	ICrownView* view;

	const std::array<Tooth, 32>* teeth;

	BridgeRangeValidator range_validator;

	bool bridge;
	std::string bridgeDiagnosis;
	std::string bridgeRangeName;
	double bridgePrice;

	std::string getBridgeDiagnosis(int begin, int end, const std::array<Tooth, 32>& teeth);
	std::string getBridgeRangeName(int begin, int end, const std::array<Tooth, 32>& teeth);
	std::tuple<int, int> getInitialBridgeRange(const std::vector<Tooth*>& selectedTeeth);


	std::string autoDiagnosis(const Tooth& tooth) override;
	Result getResult() override;

public:
	CrownPresenter(const std::vector<Tooth*>& selectedTeeth, const std::array<Tooth, 32>& teeth);
	
	void setView(ICrownView* view);
	void rangeChanged(int begin, int end);
	void rangeWidgetChecked(bool checked);

	// Inherited via ManipulationPresenter
	void setManipulationTemplate(const ProcedureTemplate& m) override;
	bool isValid() override;
	std::vector<Procedure> getManipulations() override;
};

