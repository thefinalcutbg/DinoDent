#pragma once
#include "AbstractSubPresenter.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"
#include "Model/Validators/ProcedureValidators.h"


class ObturationPresenter : public AbstractSubPresenter
{
	SurfaceValidator surf_validator;

	const std::vector<Tooth*>& selectedTeeth;

	std::string getDiagnosis(const Tooth& tooth);

	std::array<bool, 6> autoSurfaces(const Tooth& tooth);

	bool selectedForFirstTime = true;

public:
	ObturationPresenter(const std::vector<Tooth*>& selectedTeeth);

	void setAdditionalTemplateParameters() override;
	bool additionalValidation() override;
	virtual std::vector<Procedure> getProcedures() override;

};

