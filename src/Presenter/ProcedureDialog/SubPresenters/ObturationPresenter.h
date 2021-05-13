#pragma once
#include "TeethMPresenter.h"
#include "View/ProcedureDialog/Widgets/ObturationView/IObturationView.h"
#include "Model/Validator/ManipulationValidators.h"
#include "Model/Tooth/ToothUtils.h"

class ObturationPresenter : public TeethMPresenter
{
	IObturationView* view;
	SurfaceValidator surf_validator;

	ToothUtils utils;

	virtual std::string autoDiagnosis(const Tooth& tooth) override;
	virtual Result getResult() override;


	std::array<bool, 6> autoSurfaces(const Tooth& tooth);

public:
	ObturationPresenter(const std::vector<Tooth*>& selectedTeeth);

	void setView(IObturationView* view);

	// Inherited via ManipulationPresenter
	virtual void setManipulationTemplate(const ManipulationTemplate& m) override;
	virtual bool isValid() override;
	
};

