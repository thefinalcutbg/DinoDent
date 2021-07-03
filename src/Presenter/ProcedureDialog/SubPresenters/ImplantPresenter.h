#pragma once
#include "TeethMPresenter.h"

class ProcedureTemplate;
class IImplantView;

class ImplantPresenter : public TeethMPresenter
{
	IImplantView* view;

	virtual std::string autoDiagnosis(const Tooth& tooth) override;
	virtual Result getResult() override;

public:
	void setManipulationTemplate(const ProcedureTemplate& m);
	ImplantPresenter(const std::vector<Tooth*>& selectedTeeth);
	void setView(IImplantView* view);
};

