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
	struct KsmpState { std::string code; bool enabled{ false }; };
	ICommonFields* common_view{ NULL };

	ProcedureType m_type;
	std::string m_diagnosis;
	bool m_nzok{0};
	int m_code{-1};
	double m_price{0};
	std::string m_name;

	NotEmptyValidator notEmpty_validator;

	KsmpState m_ksmp;
	//returns tooth 99, non-temporary;
	Procedure getProcedureCommonFields();

public:
	AbstractSubPresenter(ProcedureType t) : m_type(t) {};

	virtual void setCommonFieldsView(ICommonFields* view) { common_view = view;};

	void diagnosisTextChanged(std::string text) { m_diagnosis = text; };

	virtual void setProcedureTemplate(const ProcedureTemplate& m);
	
	virtual bool isValid();

	void ksmpButtonClicked();

	void ksmpToggled(bool toggled);

	virtual std::vector<Procedure> getProcedures() = 0;
	
};

