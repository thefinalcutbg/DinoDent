#pragma once
#include "AbstractSubPresenter.h"
#include "Model/Validators/ProcedureValidators.h"

class EditorPresenter : public AbstractSubPresenter
{
	int m_tooth{ -1 };
	int m_temp{ false };
	Date m_date;

	SurfaceValidator surface_validator;
	BridgeRangeValidator range_validator;

public:

	EditorPresenter(const Procedure& p);

	void setAdditionalTemplateParameters() override;
	bool additionalValidation() override;

	//always returns one procedure
	virtual std::vector<Procedure> getProcedures();


};

