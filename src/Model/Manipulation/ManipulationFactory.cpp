#include "ManipulationFactory.h"

bool ManipulationFactory::checkDiagnosis(const Manipulation& m)
{
	return !m.diagnosis.empty();
}

Manipulation ManipulationFactory::tryAutoComplete(Tooth* tooth, Manipulation m)
{
	if (m.diagnosis.empty())
	{
		m.diagnosis = getDiagnosis(*tooth);
	}

	virtualAutoComplete(tooth, m);

	return m;
}

bool ManipulationFactory::checkManipulationValidity(const Manipulation& m)
{
	return checkDiagnosis(m);
}

ManipulationFactory::ManipulationFactory(std::string(*getDiagnosis)(const Tooth& tooth))
	:
	getDiagnosis(getDiagnosis)
{

}

ManipulationFactoryProducts ManipulationFactory::getManipulations(const std::vector<Tooth*>& selectedTeeth, Manipulation m)
{
	std::vector<Manipulation> newManipulations;
	newManipulations.reserve(selectedTeeth.size());

	std::vector<int> notAppliedToothNum;

	for (Tooth* t : selectedTeeth)
	{
		Manipulation newManipulation = tryAutoComplete(t, m);

		if (checkManipulationValidity(newManipulation))
		{
			newManipulation.tooth = t->index;
			newManipulations.push_back(newManipulation);
		}
		else
		{
			notAppliedToothNum.push_back(t->index);
		}
	}

	return { newManipulations, notAppliedToothNum };

}

bool ObturationFactory::checkSurfaces(const Manipulation& m)
{
	auto& surfaces = std::get<ObtData>(m.result).surfaces;

	for (int i = 0; i < 6; i++)
	{
		if (surfaces[i])
		{
			return true;
		}
	}

	return false;
}

ObturationFactory::ObturationFactory() : ManipulationFactory(AutoComplete::getObturDiag)
{}

bool ObturationFactory::checkManipulationValidity(const Manipulation& m)
{
	return (checkDiagnosis(m) && checkSurfaces(m));
}

void ObturationFactory::virtualAutoComplete(Tooth* tooth, Manipulation &m)
{
	auto& surfaces = std::get<ObtData>(m.result).surfaces;

	if (!checkSurfaces(m))
	{
		surfaces = AutoComplete::getSurfaces(*tooth);
	}
}
