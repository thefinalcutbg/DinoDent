#include "ExtractionPresenter.h"


ExtractionPresenter::ExtractionPresenter(const std::vector<Tooth*>& selectedTeeth)	:
	TeethMPresenter(selectedTeeth)
{
	if (selectedTeeth.size() == 1)
	{
		diagnosis = autoDiagnosis(*selectedTeeth.at(0));
	}

}

std::string ExtractionPresenter::autoDiagnosis(const Tooth& tooth)
{
	std::array<bool, 8> existing
	{
		tooth.hyperdontic.exists(),
		tooth.implant.exists(),
		tooth.lesion.exists(),
		tooth.temporary.exists(),
		tooth.root.exists(),
		tooth.periodontitis.exists(),
		tooth.mobility.exists(),
		tooth.fracture.exists()
	};

	std::array<std::string, 8> diagnosis
	{
		"Свръхброен зъб",
		"Периимплантит",
		tooth.lesion.data.getDiagnosisName(),
		"Разклатен млечен зъб",
		tooth.root.data.getDiagnosisName(),
		"Пародонтозен зъб",
		"Подвижен зъб",
		tooth.fracture.data.getDiagnosisName()
	};

	for (int i = 0; i < 8; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};;
}
