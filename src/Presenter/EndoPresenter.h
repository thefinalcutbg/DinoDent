#pragma once
#include "AbstractSubPresenter.h"

class EndoPresenter : public AbstractSubPresenter
{
	const std::vector<Tooth*>& selectedTeeth;
	std::string getDiagnosis(const Tooth& tooth);

public:
	EndoPresenter(const std::vector<Tooth*>& selectedTeeth);
	void setProcedureTemplate(const ProcedureTemplate& m) override;
	std::vector<Procedure> getProcedures() override;
};

