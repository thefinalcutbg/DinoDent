#include "AbstractSubPresenter.h"


void AbstractSubPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	common_view->setCurrentPresenter(this);

	m_template = m;

	common_view->priceEdit()->disable(m.nzok);
	common_view->manipulationEdit()->disable(m.nzok);

	common_view->manipulationEdit()->set_Text(m.name);
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
	common_view->manipulationEdit()->setInputValidator(&notEmpty_validator);

}

bool AbstractSubPresenter::isValid()
{
	std::array<AbstractUIElement*, 3>validatable
	{
		common_view->dateEdit(),
		common_view->manipulationEdit(),
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



Procedure AbstractSubPresenter::getProcedureCommonFields()
{
	Procedure result{
		  m_template,
		  Date{common_view->dateEdit()->getDate()},
		  common_view->manipulationEdit()->getText(),
		  common_view->diagnosisEdit()->getText(),
		  common_view->priceEdit()->get_Value(),
	};

	result.LPK = UserManager::currentUser().LPK;

	return result;
}
