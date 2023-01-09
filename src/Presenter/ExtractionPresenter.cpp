#include "ExtractionPresenter.h"


ExtractionPresenter::ExtractionPresenter(const std::vector<Tooth*>& selectedTeeth)	:
	AbstractSubPresenter(ProcedureType::extraction),
	selectedTeeth(selectedTeeth)
{
	if (selectedTeeth.size() == 1)
		m_diagnosis = getDiagnosis(*selectedTeeth.at(0));
}

void ExtractionPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	bool noTeethSelected = !selectedTeeth.size();

	common_view->set_hidden(noTeethSelected);
	if (noTeethSelected)return;

	AbstractSubPresenter::setProcedureTemplate(m);
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
		tooth.lesion.getDiagnosisString(),
		"Разклатен млечен зъб",
		tooth.root.getDiagnosisString(),
		"Пародонтозен зъб",
		"Подвижен зъб",
		tooth.fracture.getDiagnosisString(),
		tooth.pulpitis.getDiagnosisString()
	};

	for (int i = 0; i < 9; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};;
}
