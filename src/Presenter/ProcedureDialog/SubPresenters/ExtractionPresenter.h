#pragma once
#include "TeethMPresenter.h"

class ExtractionPresenter : public TeethMPresenter
{
	virtual std::string autoDiagnosis(const Tooth& tooth) override;

public:
	ExtractionPresenter(const std::vector<Tooth*>& selectedTeeth);
};

