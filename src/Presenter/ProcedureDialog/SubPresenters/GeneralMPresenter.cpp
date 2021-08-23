#include "GeneralMPresenter.h"

void GeneralMPresenter::setValidators()
{
	common_view->diagnosisEdit()->setInputValidator(&notEmpty_validator);
	common_view->manipulationEdit()->setInputValidator(&notEmpty_validator);
}

void GeneralMPresenter::setManipulationTemplate(const ProcedureTemplate& m)
{
	common_view->setCurrentPresenter(this);

	m_template = m;

	common_view->priceEdit()->disable(m.nzok);
	common_view->manipulationEdit()->disable(m.nzok);

	common_view->manipulationEdit()->set_Text(m.name);
	common_view->priceEdit()->set_Value(m.price);

	!m.diagnosis.empty() ?
		common_view->diagnosisEdit()->set_Text(m.diagnosis)
		:
		common_view->diagnosisEdit()->set_Text(diagnosis);

	setValidators();
}

bool GeneralMPresenter::isValid()
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


std::vector<Procedure> GeneralMPresenter::getProcedures()
{
	return std::vector<Procedure>
	{
		Procedure
		{ m_template,
		  Date{common_view->dateEdit()->getDate()},
		  common_view->manipulationEdit()->getText(),
		  common_view->diagnosisEdit()->getText(),
		  common_view->priceEdit()->get_Value(),
		  getResult()
		}
	};
}
