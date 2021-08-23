#include "TeethMPresenter.h"

TeethMPresenter::TeethMPresenter
(const std::vector<Tooth*>& selectedTeeth)
	:
	selectedTeeth(&selectedTeeth),
	noTeethSelected(!selectedTeeth.size())
{

}

void TeethMPresenter::setManipulationTemplate(const ProcedureTemplate& m)
{
	common_view->set_hidden(noTeethSelected);
	if(noTeethSelected) return;

	GeneralMPresenter::setManipulationTemplate(m);
}

bool TeethMPresenter::isValid()
{
	if (!GeneralMPresenter::isValid()) return false;

	if (noTeethSelected) return false;

	return true;
}

std::vector<Procedure> TeethMPresenter::getProcedures()
{
	std::vector<Procedure> m;
	m.reserve(selectedTeeth->size());

	for (int i = 0; i < selectedTeeth->size(); i++)
	{
		m.emplace_back
		(
			Procedure
			{
				 m_template,
				 Date{common_view->dateEdit()->getDate()},
				 common_view->manipulationEdit()->getText(),
				 common_view->diagnosisEdit()->getText(),
				 common_view->priceEdit()->get_Value(),
				 getResult(),
				 selectedTeeth->at(i)->index,
				 selectedTeeth->at(i)->temporary.exists()
			}
		);
	}

	return m;
}
