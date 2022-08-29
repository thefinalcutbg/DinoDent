#pragma once
#include "AbstractSubPresenter.h"
#include "View/Interfaces/IObturationView.h"
#include "Model/Validators/ProcedureValidators.h"


class ObturationPresenter : public AbstractSubPresenter
{
	IObturationView* view;
	SurfaceValidator surf_validator;

	const std::vector<Tooth*>& selectedTeeth;

	std::string getDiagnosis(const Tooth& tooth);

	std::array<bool, 6> autoSurfaces(const Tooth& tooth);

public:
	ObturationPresenter(const std::vector<Tooth*>& selectedTeeth);

	void setView(IObturationView* view);

	void setProcedureTemplate(const ProcedureTemplate& m) override;
	bool isValid() override;
	virtual std::vector<Procedure> getProcedures() override;

};

