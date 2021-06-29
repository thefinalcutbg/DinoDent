#include "ImplantPresenter.h"
#include "View/ProcedureDialog/Widgets/ImplantView/IImplantView.h"
#include "Model/Manipulation/Manipulation.h"
#include "Model/Tooth/ToothUtils.h"


ImplantPresenter::ImplantPresenter(const std::vector<Tooth*>& selectedTeeth)
: TeethMPresenter(selectedTeeth), view(nullptr)
{
	if (selectedTeeth.size() == 1)
	{
		diagnosis = autoDiagnosis(*selectedTeeth.at(0));
	};
}

void ImplantPresenter::setManipulationTemplate(const ManipulationTemplate& m)
{
	common_view->set_hidden(noTeethSelected);
	view->set_hidden(noTeethSelected);
	if (noTeethSelected) return;

	GeneralMPresenter::setManipulationTemplate(m);

	view->systemEdit()->set_Text(m.material);
}

void ImplantPresenter::setView(IImplantView* view)
{
	this->view = view;
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
	return view->getData();
}
