#include "TeethMPresenter.h"

TeethMPresenter::TeethMPresenter
(const std::vector<Tooth*>& selectedTeeth)
	:
	selectedTeeth(&selectedTeeth),
	noTeethSelected(!selectedTeeth.size())
{

}

void TeethMPresenter::setManipulationTemplate(const ManipulationTemplate& m)
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

std::vector<Manipulation> TeethMPresenter::getManipulations()
{
	std::vector<Manipulation> m;
	m.reserve(selectedTeeth->size());

	for (int i = 0; i < selectedTeeth->size(); i++)
	{
		m.emplace_back
		(
			Manipulation
			{
				 m_template,
				 Date{common_view->dateEdit()->getText()},
				 common_view->manipulationEdit()->getText(),
				 common_view->diagnosisEdit()->getText(),
				 common_view->priceEdit()->get_Value(),
				 getResult()
			}
		);

		m[i].tooth = selectedTeeth->at(i)->index;
	}

	return m;
}
