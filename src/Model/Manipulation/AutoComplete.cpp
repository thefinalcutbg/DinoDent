#include "AutoComplete.h"

ToothUtils AutoComplete:: utils{};

std::tuple<int, int> AutoComplete::getInitialBridgeRange(const std::vector<Tooth*>& selectedTeeth)
{
	if (!selectedTeeth.size())
	{
		return std::tuple<int, int>(0, 1);
	}

	int begin = 0;
	int end = 0;

	if (selectedTeeth.size() == 1) //if only 1 tooth is selected, the bridge length is 2
	{
		begin = selectedTeeth[0]->index;

		if (begin != 15 && begin != 31)
		{
			end = begin + 1;
		}
		else
		{
			end = begin - 1;
			std::swap(end, begin);
		}
		return std::tuple<int, int>{begin, end};
	}
	
	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //no matter if the first and last teeth are on different jaws
	
	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return std::tuple<int, int>(begin, end);
}

std::string AutoComplete::bridgeRangeString(int begin, int end, const std::array<Tooth, 32>& teeth)
{
	return std::string
	{	
		" " +
		std::to_string(utils.getToothNumber(begin, teeth[begin].temporary.exists())) +
		" - " +
		std::to_string(utils.getToothNumber(end, teeth[end].temporary.exists()))
	};
}

std::array<bool, 6> AutoComplete::getSurfaces(const Tooth& tooth)
{
	std::array<bool, 6> surfaces{ 0,0,0,0,0,0 };

	if (tooth.caries)
	{
		for (int i = 0; i < 6; i++)
		{
			if (tooth.c_surf[i].exists())
			{
				surfaces[i] = true;
			}
		}
	}
	else if (tooth.root.exists())
	{
		surfaces = { 1, 1, 1, 1, 1, 1 };
	}
	
	if (tooth.endo.exists())
	{
		if (utils.getToothType(tooth.index) == ToothType::Frontal)
		{
			surfaces[static_cast<int>(Surface::Lingual)] = true;
		}
		else surfaces[static_cast<int>(Surface::Occlusal)] = true;
	}
	
	if (tooth.fracture.exists())
	{
		if (utils.getToothType(tooth.index) == ToothType::Frontal)
		{
			surfaces[static_cast<int>(Surface::Occlusal)] = true;
		}
		else surfaces[static_cast<int>(Surface::Lingual)] = true;
	}
	else if (tooth.obturation)
	{
		for (int i = 0; i < 6; i++)
		{
			if (tooth.o_surf[i].exists())
			{
				surfaces[i] = true;
			}
		}
	}

	return surfaces;
}

std::string AutoComplete::getExtrDiag(const Tooth& tooth)
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
		tooth.lesion.getDiagnosis(),
		"Разклатен млечен зъб",
		tooth.root.getDiagnosis(),
		"Пародонтозен зъб",
		"Подвижен зъб",
		tooth.fracture.getDiagnosis()
	};

	for (int i = 0; i < 8; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
}

std::string AutoComplete::getObturDiag(const Tooth& tooth)
{

	bool secondaryCaries = 0;
	std::string cariesDiagnosis;

	if (tooth.caries)
	{
		for (int i = 0; i < 6; i++)		//checking if somewhere obturation is present also, returning secondary caries
		{
			auto& c = tooth.c_surf[i];
			auto& o = tooth.o_surf[i];
			if (c.exists() && o.exists())
			{
				secondaryCaries = true;
			}
		}

		for (int i = 0; i < 6; i++)		//getting the diagnosis of the first caries found
		{
			auto& c = tooth.c_surf[i];

			if (c.exists()) {
				cariesDiagnosis = c.getDiagnosis();
			}
		}
	}

	std::array<bool, 7> existing
	{
			secondaryCaries,
			tooth.caries,
			tooth.endo.exists(),
			tooth.pulpitis.exists() && tooth.lesion.exists(),
			tooth.fracture.exists(),
			tooth.root.exists(),
			tooth.obturation
	};

	std::array<std::string, 7> diagnosis
	{
		"Вторичен кариес",
		cariesDiagnosis,
		"Status post devital.",
		"Преендодонтско изграждане",
		tooth.fracture.getDiagnosis(),
		tooth.root.getDiagnosis(),
		"Дефектна обтурация"
	};

	for (int i = 0; i < 7; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};

}

std::string AutoComplete::getEndoDiag(const Tooth& tooth)
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

std::string AutoComplete::getCrownDiag(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.endo.exists(),
		tooth.fracture.exists(),
		tooth.obturation,
		tooth.implant.exists()
	};

	std::array<std::string, 4> diagnosis
	{
		"Status post devitalisationem",
		tooth.fracture.getDiagnosis(),
		"Екстензивна ресторация",
		"Протезиране върху имплант"
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
	//return "Протезиране по естетични съображения";
}

std::string AutoComplete::getBridgeDiag(int begin, int end, const std::array<Tooth, 32>& teeth)
{
	
	std::vector<const Tooth*> teethRange;

	for (int i = begin; i < end+1; i++)
	{
		teethRange.push_back(&teeth.at(i));
	}

	std::vector<int> missingTeethIdx;

		for (const Tooth *tooth : teethRange)
		{
			if (tooth->extraction.exists())
			{
				missingTeethIdx.emplace_back(utils.getToothNumber(tooth->index, false));
			}
		}
	

	if (missingTeethIdx.size() == teethRange.size())
	{
		return "Всички зъби са извадени, wtf?"; //all selected teeth are extracted, wtf user?
	}

	if (!missingTeethIdx.size())
	{
		return "Стабилизация с блок корони";
	}

	diagnosisStream.str("");
	diagnosisStream << "Adontia partialis ";

	for (int i = 0; i < missingTeethIdx.size(); i++)
	{

		diagnosisStream << missingTeethIdx.at(i);

		if (i != missingTeethIdx.size() - 1)
		{
			diagnosisStream << ", ";
		}
	}

	return diagnosisStream.str();
}



