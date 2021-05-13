#include "EndoPresenter.h"


EndoPresenter::EndoPresenter(const std::vector<Tooth*>& selectedTeeth) :
	TeethMPresenter(selectedTeeth)
{
	if (selectedTeeth.size() == 1)
	{
		diagnosis = autoDiagnosis(*selectedTeeth.at(0));
	}

}

std::string EndoPresenter::autoDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.pulpitis.exists(),
		tooth.lesion.exists(),
		tooth.endo.exists(),
		tooth.fracture.exists()
	};

	std::array<std::string, 4> diagnosis
	{
		tooth.pulpitis.getDiagnosis(),
		tooth.lesion.getDiagnosis(),
		"Инфектиран коренов пълнеж",
		tooth.fracture.getDiagnosis()
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string();
}
