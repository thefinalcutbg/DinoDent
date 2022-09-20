#pragma once
#include "Model/Dental/Procedure.h"
#include "Model/Dental/Tooth.h"
#include "View/Interfaces/ICommonFields.h"
#include "Model/Validators/ProcedureValidators.h"
#include "Model/Validators/CommonValidators.h"

#include <vector>

class AbstractSubPresenter
{

protected:

	ICommonFields* common_view{ NULL };

	ProcedureType m_type;
	std::string m_diagnosis;
	bool m_nzok{0};
	int m_code{-1};
	double m_price{0};
	std::string m_name;

	NotEmptyValidator notEmpty_validator;

	std::string m_ksmp;
	//returns tooth 99, non-temporary;
	Procedure getProcedureCommonFields();

public:
	AbstractSubPresenter(ProcedureType t) : m_type(t) {};

	virtual void setCommonFieldsView(ICommonFields* view) { common_view = view;};

	void diagnosisTextChanged(std::string text) { m_diagnosis = text; };

	virtual void setProcedureTemplate(const ProcedureTemplate& m);
	
	virtual bool isValid();

	void ksmpButtonClicked();

	virtual std::vector<Procedure> getProcedures() = 0;
	
};

