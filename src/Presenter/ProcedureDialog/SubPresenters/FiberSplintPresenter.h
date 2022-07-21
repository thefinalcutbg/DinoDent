#pragma once
#include "AbstractSubPresenter.h"
#include "View/ProcedureDialog/ProcedureFields/IFiberSplintView.h"
#include "Model/Validator/ProcedureValidators.h"
#include "Model/Tooth/ToothUtils.h"

class FiberSplintPresenter : public AbstractSubPresenter
{
	IFiberSplintView* view;

	const ToothContainer& teeth;
	const std::vector<Tooth*>& selectedTeeth;

	std::string splintRangeName;
	std::string m_procedureRangeString;

	BridgeRangeValidator range_validator;

	std::string getSplintRangeName(int begin, int end, const ToothContainer& teeth);
	std::tuple<int, int> getInitialSplintRange(const std::vector<Tooth*>& selectedTeeth);

	std::string getDiagnosis();

public:
	FiberSplintPresenter(const std::vector<Tooth*>& selectedTeeth, const ToothContainer& teeth);

	void setView(IFiberSplintView* view);
	void rangeChanged(int begin, int end);

	void setProcedureTemplate(const ProcedureTemplate& m) override;
	bool isValid() override;
	std::vector<Procedure> getProcedures() override;
};

