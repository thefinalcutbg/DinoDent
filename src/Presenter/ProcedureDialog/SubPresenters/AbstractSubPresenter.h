#pragma once
#include "Model/Procedure/Procedure.h"
#include "Model/Tooth/Tooth.h"
#include "View/ProcedureDialog/CommonFields/ICommonFields.h"
#include "Model/Validator/ProcedureValidators.h"
#include "Model/Validator/CommonValidators.h"

#include <vector>

class AbstractSubPresenter
{

protected:
	ICommonFields* common_view{ NULL };

	std::string m_diagnosis;
	ProcedureTemplate m_template;
	NotEmptyValidator notEmpty_validator;

	Procedure getProcedureCommonFields();

public:
	
	void setCommonFieldsView(ICommonFields* view) { common_view = view;};

	void diagnosisTextChanged(std::string text) { m_diagnosis = text; };

	virtual void setProcedureTemplate(const ProcedureTemplate& m);

	virtual bool isValid();

	virtual std::vector<Procedure> getProcedures() = 0;
	
};

