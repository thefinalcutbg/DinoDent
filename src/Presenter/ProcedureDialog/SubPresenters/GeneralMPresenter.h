#pragma once
#include "Model/Manipulation/Manipulation.h"
#include "Model/Tooth/Tooth.h"
#include "View/ProcedureDialog/Widgets/CommonFields/ICommonFields.h"
#include "Model/Validator/ManipulationValidators.h"

#include <vector>

class GeneralMPresenter : public ICommonFieldsPresenter
{

protected:
	ICommonFields* common_view{ NULL };

	std::string diagnosis;
	ManipulationTemplate m_template;

	EmptyFieldValidator notEmpty_validator;

	virtual std::string autoDiagnosis(const Tooth& tooth) { return std::string(); } ;
	virtual Result getResult() { return NoData(); }
	virtual void setValidators();

public:
	
	void setCommonFieldsView(ICommonFields* view) { common_view = view;};

	void diagnosisTextChanged(std::string text) override { diagnosis = text; };

	virtual void setManipulationTemplate(const ManipulationTemplate& m);

	virtual bool isValid();

	virtual std::vector<Manipulation> getManipulations();
	
};

