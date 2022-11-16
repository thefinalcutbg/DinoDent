#include "ImplantPresenter.h"
#include "View/Interfaces/IImplantView.h"
#include "Model/Dental/Procedure.h"
#include "Model/Dental/ToothUtils.h"


ImplantPresenter::ImplantPresenter(const std::vector<Tooth*>& selectedTeeth)
	:
	AbstractSubPresenter(ProcedureType::implant),
	selectedTeeth(selectedTeeth), view(nullptr)
{
	if (selectedTeeth.size() == 1)
		m_diagnosis = getDiagnosis(*selectedTeeth.at(0));

}


void ImplantPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	bool noTeethSelected = !selectedTeeth.size();
	common_view->set_hidden(noTeethSelected);
	view->set_hidden(noTeethSelected);
	if (noTeethSelected) return;

	AbstractSubPresenter::setProcedureTemplate(m);
	
	auto data = view->getData();
	view->setData(data);
}

void ImplantPresenter::setView(IImplantView* view)
{
	this->view = view;
}


std::string ImplantPresenter::getDiagnosis(const Tooth& tooth)
{
	if (tooth.extraction.exists() && !tooth.temporary.exists())
	{
		return "Adontia " + std::to_string(ToothUtils::getToothNumber(tooth.index, false));
	}

	return std::string();
}

std::vector<Procedure> ImplantPresenter::getProcedures()
{
	std::vector<Procedure> procedures;
	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth) {
		procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
		procedures.back().result = view->getData();
		procedures.back().tooth = tooth->index;
	}

	return procedures;
}