#include "ImplantPresenter.h"

ImplantPresenter::ImplantPresenter(const std::vector<Tooth*>& selectedTeeth)
: TeethMPresenter(selectedTeeth)
{
	if (selectedTeeth.size() == 1)
	{
		diagnosis = autoDiagnosis(*selectedTeeth.at(0));
	};
}

std::string ImplantPresenter::autoDiagnosis(const Tooth& tooth)
{
	if (tooth.extraction.exists() && !tooth.temporary.exists())
	{
		return "Adontia " + std::to_string(ToothUtils::getToothNumber(tooth.index, false));
	}

	return std::string();
}

Result ImplantPresenter::getResult()
{
	return ImplData();
}
