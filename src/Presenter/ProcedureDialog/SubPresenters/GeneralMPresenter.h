#pragma once
#include "Model/Procedure/Procedure.h"
#include "Model/Tooth/Tooth.h"
#include "View/ProcedureDialog/Widgets/CommonFields/ICommonFields.h"
#include "Model/Validator/ProcedureValidators.h"
#include "Model/Validator/CommonValidators.h"

#include <vector>

class GeneralMPresenter : public ICommonFieldsPresenter
{

protected:
	ICommonFields* common_view{ NULL };

	std::string diagnosis;
	ProcedureTemplate m_template;

	NotEmptyValidator notEmpty_validator;

	virtual std::string autoDiagnosis(const Tooth& tooth) { return std::string(); } ;
	virtual Result getResult() { return NoData(); }
	virtual void setValidators();

public:
	
	void setCommonFieldsView(ICommonFields* view) { common_view = view;};

	void diagnosisTextChanged(std::string text) override { diagnosis = text; };

	virtual void setManipulationTemplate(const ProcedureTemplate& m);

	virtual bool isValid();

	virtual std::vector<Procedure> getManipulations();
	
};

