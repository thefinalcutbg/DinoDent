#pragma once
#include "TeethMPresenter.h"

class EndoPresenter : public TeethMPresenter
{
	virtual std::string autoDiagnosis(const Tooth& tooth) override;

public:
	EndoPresenter(const std::vector<Tooth*>& selectedTeeth);
};

