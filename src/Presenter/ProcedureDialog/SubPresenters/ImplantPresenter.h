#pragma once
#include "TeethMPresenter.h"
#include "Model/Tooth/ToothUtils.h"

class ImplantPresenter : public TeethMPresenter
{
	virtual std::string autoDiagnosis(const Tooth& tooth) override;
	virtual Result getResult() override;

public:

	ImplantPresenter(const std::vector<Tooth*>& selectedTeeth);
};

