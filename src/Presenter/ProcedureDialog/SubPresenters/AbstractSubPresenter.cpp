#include "AbstractSubPresenter.h"
#include "Model/KSMP.h"

void AbstractSubPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	common_view->setCurrentPresenter(this);

	m_code = m.code;
	m_price = m.price;
	m_nzok = m.nzok;
	m_name = m.name;


	//common_view->priceEdit()->disable(m.nzok);
	common_view->procedureNameEdit()->disable(m.nzok);

	common_view->procedureNameEdit()->set_Text(m.name);
	common_view->priceEdit()->set_Value(m.price);

	//If the template doesn't have default diagnosis, 
	//each presenter class auto-generates one if possible.
	//The implementation-defined diagnosis can be generated either
	//in the constructor of the subclass, or in the overriden getProcedure func:

	!m.diagnosis.empty() ? 
		common_view->diagnosisEdit()->set_Text(m.diagnosis)
		:
		common_view->diagnosisEdit()->set_Text(m_diagnosis);

	common_view->diagnosisEdit()->setInputValidator(&notEmpty_validator);
	common_view->procedureNameEdit()->setInputValidator(&notEmpty_validator);

	common_view->allowKSMPDisable(m.nzok == false);

	if (m.ksmp.empty()) {

		m_ksmp.code = KSMP::getByType(m.type).at(0)->code;
		m_ksmp.enabled = false;
		
	}
	else
	{
		m_ksmp.code = m.ksmp;
		m_ksmp.enabled = true;
	}

	common_view->setKSMPCode(m_ksmp.code);
	common_view->enableKSMP(m_ksmp.enabled);



}

bool AbstractSubPresenter::isValid()
{
	std::array<AbstractUIElement*, 3>validatable
	{
		common_view->dateEdit(),
		common_view->procedureNameEdit(),
		common_view->diagnosisEdit()
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

	return true;
}

#include "View/ModalDialogBuilder.h"

void AbstractSubPresenter::ksmpButtonClicked()
{
	auto result = ModalDialogBuilder::ksmpDialog(KSMP::getByType(m_type), m_ksmp.code);
	
	if (!result.empty()) {
		m_ksmp.code = result;

		common_view->setKSMPCode(m_ksmp.code);
		common_view->procedureNameEdit()->set_Text(KSMP::getName(result));
	}
}

void AbstractSubPresenter::ksmpToggled(bool toggled)
{
	m_ksmp.enabled = toggled;
	common_view->setKSMPCode(m_ksmp.code);
	common_view->enableKSMP(m_ksmp.enabled);

	common_view->procedureNameEdit()->set_Text(KSMP::getName(m_ksmp.code));
}


Procedure AbstractSubPresenter::getProcedureCommonFields()
{
	Procedure result;

	result.code = m_code;
	result.type = m_type;
	result.nzok = m_nzok;
	result.name = common_view->procedureNameEdit()->getText();
	result.LPK = User::doctor().LPK;
	result.ksmp = m_ksmp.enabled ? m_ksmp.code : "";
	result.date = common_view->dateEdit()->getDate();
	result.diagnosis = common_view->diagnosisEdit()->getText();
	result.price = common_view->priceEdit()->get_Value();


	return result;
}
