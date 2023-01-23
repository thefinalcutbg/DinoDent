#include "AbstractSubPresenter.h"
#include "Model/Dental/KSMP.h"
#include "View/ModalDialogBuilder.h"

void AbstractSubPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	view->setCurrentPresenter(this);

	m_code = m.code;
	m_price = m.price;
	m_nzok = m.nhif;
	m_name = m.name;


	//common_view->priceEdit()->disable(m.nhif);
	view->procedureNameEdit()->disable(m.nhif);

	view->procedureNameEdit()->set_Text(m.name);
//	common_view->priceEdit()->set_Value(m.price);

	//If the template doesn't have default diagnosis, 
	//each presenter class auto-generates one if possible.
	//The implementation-defined diagnosis can be generated either
	//in the constructor of the subclass, or in the overriden getProcedure func:

	!m.diagnosis.empty() ? 
		view->diagnosisEdit()->set_Text(m.diagnosis)
		:
		view->diagnosisEdit()->set_Text(m_diagnosis);

	view->diagnosisEdit()->setInputValidator(&notEmpty_validator);
	view->procedureNameEdit()->setInputValidator(&notEmpty_validator);

	view->setNhifLayout(m.nhif);

	m.ksmp.empty() ? 
		m_ksmp = KSMP::getByType(m.type).at(0)->code
		:
		m_ksmp = m.ksmp;


	view->setKSMPCode(m_ksmp);

	setAdditionalTemplateParameters();

}

bool AbstractSubPresenter::isValid()
{
	std::array<AbstractUIElement*, 3>validatable
	{
		view->dateEdit(),
		view->procedureNameEdit(),
		view->diagnosisEdit()
	};

	for (AbstractUIElement* e : validatable)
	{
		e->validateInput();
		if (!e->isValid())
		{
			e->setFocus();
			return false;
		}
	}

	if (!KSMP::isValid(view->getKSMPCode())) {
		ModalDialogBuilder::showMessage("Невалиден КСМП код");
		return false;
	}

	return additionalValidation();
}



void AbstractSubPresenter::ksmpButtonClicked()
{
	auto result = ModalDialogBuilder::ksmpDialog(KSMP::getByType(m_type), m_ksmp);
	
	if (!result.empty()) {
		m_ksmp = result;

		view->setKSMPCode(m_ksmp);
		view->procedureNameEdit()->set_Text(KSMP::getName(result));
	}
}

Procedure AbstractSubPresenter::getProcedureCommonFields()
{
	Procedure result;

	result.code = m_code;
	result.type = m_type;
	result.financingSource = view->getFinancingSource();
	result.name = view->procedureNameEdit()->getText();
	result.LPK = User::doctor().LPK;
	result.ksmp = m_ksmp;
	result.date = view->dateEdit()->getDate();
	result.diagnosis = view->diagnosisEdit()->getText();
	result.notes = view->getNotes();
	result.hyperdontic = view->onHyperdontic();
//	result.price = common_view->priceEdit()->get_Value();


	return result;
}


