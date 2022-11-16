#include "EndoPresenter.h"


EndoPresenter::EndoPresenter(const std::vector<Tooth*>& selectedTeeth) :
	AbstractSubPresenter(ProcedureType::endo),
	selectedTeeth(selectedTeeth)
{
	if (selectedTeeth.size() == 1)
		m_diagnosis = getDiagnosis(*selectedTeeth.at(0));
}

void EndoPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	bool noTeethSelected = !selectedTeeth.size();

	common_view->set_hidden(noTeethSelected);
	if (noTeethSelected)return;

	AbstractSubPresenter::setProcedureTemplate(m);
}


std::vector<Procedure> EndoPresenter::getProcedures()
{
	std::vector<Procedure> procedures;
	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth){
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
		tooth.pulpitis.getDiagnosisString(),
		tooth.lesion.getDiagnosisString(),
		"Инфектиран коренов пълнеж",
		tooth.fracture.getDiagnosisString()
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string();
}
