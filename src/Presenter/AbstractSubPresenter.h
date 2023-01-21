#pragma once
#include "Model/Dental/Procedure.h"
#include "Model/Dental/Tooth.h"
#include "View/Interfaces/ICommonFields.h"
#include "Model/Validators/ProcedureValidators.h"
#include "Model/Validators/CommonValidators.h"
#include "View/Interfaces/IProcedureDialog.h"

#include <vector>

class AbstractSubPresenter
{

protected:

	ICommonFields* view{ nullptr };

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

	void setView(ICommonFields* view) {	this->view =  view; };

	void diagnosisTextChanged(std::string text) { m_diagnosis = text; };

	void setProcedureTemplate(const ProcedureTemplate& m);
	virtual void setAdditionalTemplateParameters() = 0;

	bool isValid();
	virtual bool additionalValidation() = 0;

	void ksmpButtonClicked();

	virtual void rangeChanged(int beigin, int end) {};
	virtual void bridgeChecked(bool checked) {};


	virtual std::vector<Procedure> getProcedures() = 0;
	
};

