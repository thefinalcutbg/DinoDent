#include "ToothProcedurePresenter.h"

ToothProcedurePresenter::ToothProcedurePresenter(const std::vector<Tooth*>& selectedTeeth, ProcedureType t)
	:
	AbstractSubPresenter(t),
	selectedTeeth(selectedTeeth)
{

}


std::vector<Procedure> ToothProcedurePresenter::getProcedures()
{
	std::vector<Procedure> procedures;
	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth)
	{
		procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
		procedures.back().tooth = tooth->index;
		procedures.back().temp = tooth->temporary.exists();
	}

	return procedures;

}


EndoPresenter::EndoPresenter(const std::vector<Tooth*>& selectedTeeth) :
	ToothProcedurePresenter(selectedTeeth, ProcedureType::endo)
{
	if (selectedTeeth.size() == 1)
		m_diagnosis = getDiagnosis(*selectedTeeth.at(0));
}

std::vector<Procedure> EndoPresenter::getProcedures()
{
	std::vector<Procedure> procedures;
	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth) {
		procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
		procedures.back().tooth = tooth->index;
		procedures.back().temp = tooth->temporary.exists();
	}

	return procedures;
}

std::string EndoPresenter::getDiagnosis(const Tooth& tooth)
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
		"Pulpitis chronica ulcerosa",
		"Periodontitis chronica granulomatosa localisata",
		"Инфектиран коренов пълнеж",
		"Фрактура"
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string();
}



ExtractionPresenter::ExtractionPresenter(const std::vector<Tooth*>& selectedTeeth) :
	ToothProcedurePresenter(selectedTeeth, ProcedureType::extraction)
{
	if (selectedTeeth.size() == 1)
		m_diagnosis = getDiagnosis(*selectedTeeth.at(0));
}

std::vector<Procedure> ExtractionPresenter::getProcedures()
{
	std::vector<Procedure> procedures;
	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth) {
		procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
		procedures.back().tooth = tooth->index;
		procedures.back().temp = tooth->temporary.exists();
	}

	return procedures;
}

std::string ExtractionPresenter::getDiagnosis(const Tooth& tooth)
{
	std::array<bool, 9> existing
	{
		tooth.hyperdontic.exists(),
		tooth.implant.exists(),
		tooth.lesion.exists(),
		tooth.temporary.exists(),
		tooth.root.exists(),
		tooth.periodontitis.exists(),
		tooth.mobility.exists(),
		tooth.fracture.exists(),
		tooth.pulpitis.exists()
	};

	std::array<std::string, 9> diagnosis
	{
		"Свръхброен зъб",
		"Периимплантит",
		"Periodontitis chronica granulomatosa localisata",
		"Разклатен млечен зъб",
		"Radix relicva",
		"Пародонтозен зъб",
		"Подвижен зъб",
		"Фрактура",
		"Pulpitis chronica ulcerosa" 
	};

	for (int i = 0; i < 9; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
}


ImplantPresenter::ImplantPresenter(const std::vector<Tooth*>& selectedTeeth) :
	ToothProcedurePresenter(selectedTeeth, ProcedureType::extraction)
{
	if (selectedTeeth.size() == 1)
		m_diagnosis = getDiagnosis(*selectedTeeth.at(0));
}

std::string ImplantPresenter::getDiagnosis(const Tooth& tooth)
{
	 if (tooth.extraction) return "Andontia partialis";

	 return std::string{};
}