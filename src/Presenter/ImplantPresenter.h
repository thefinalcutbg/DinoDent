#pragma once
#include "AbstractSubPresenter.h"

class ProcedureTemplate;
class IImplantView;

class ImplantPresenter : public AbstractSubPresenter
{
	IImplantView* view;

	const std::vector<Tooth*>& selectedTeeth;

	std::string getDiagnosis(const Tooth& tooth);

public:
	ImplantPresenter(const std::vector<Tooth*>& selectedTeeth);

	void setProcedureTemplate(const ProcedureTemplate& m);
	
	void setView(IImplantView* view);

	std::vector<Procedure> getProcedures() override;
};

