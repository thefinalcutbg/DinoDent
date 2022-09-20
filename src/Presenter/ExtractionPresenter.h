#pragma once
#include "AbstractSubPresenter.h"

class ExtractionPresenter : public AbstractSubPresenter
{
	const std::vector<Tooth*>& selectedTeeth;
	std::string getDiagnosis(const Tooth& tooth);

public:
	ExtractionPresenter(const std::vector<Tooth*>& selectedTeeth);
	void setProcedureTemplate(const ProcedureTemplate& m) override;
	std::vector<Procedure> getProcedures() override;
};

