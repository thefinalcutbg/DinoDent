#include "ImplantPresenter.h"
#include "View/uiComponents/qt_derived/Widgets/IImplantView.h"
#include "Model/Procedure/Procedure.h"
#include "Model/Tooth/ToothUtils.h"


ImplantPresenter::ImplantPresenter(const std::vector<Tooth*>& selectedTeeth)
: TeethMPresenter(selectedTeeth), view(nullptr)
{
	if (selectedTeeth.size() == 1)
	{
		diagnosis = autoDiagnosis(*selectedTeeth.at(0));
	};
}

void ImplantPresenter::setManipulationTemplate(const ProcedureTemplate& m)
{
	common_view->set_hidden(noTeethSelected);
	view->set_hidden(noTeethSelected);
	if (noTeethSelected) return;

	GeneralMPresenter::setManipulationTemplate(m);

	auto data = view->getData();
	data.system = m.material;

	view->setData(data);
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
