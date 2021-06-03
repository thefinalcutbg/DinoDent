#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"



ProcedureDialogPresenter::ProcedureDialogPresenter
(
	const std::vector<ManipulationTemplate>& mList,
	const std::vector<Tooth*>& selectedTeeth,
	const std::array<Tooth, 32>& teeth,
	const Date& ambListDate
)
	:
	manipulationList{ mList },
	manipulations{},

	view(nullptr),
	common_fields(nullptr),
	current_m_presenter(nullptr),
	teeth(&teeth),
	selectedTeeth(selectedTeeth),
	errorState(true),

	any_teeth_presenter(this->selectedTeeth),
	obt_presenter(this->selectedTeeth),
	extr_presenter(this->selectedTeeth),
	endo_presenter(this->selectedTeeth),
	crown_presenter(this->selectedTeeth, teeth),
	impl_presenter(this->selectedTeeth),

	presenters_ptr
	{
		&general_presenter,
		&any_teeth_presenter,
		&obt_presenter,
		&extr_presenter,
		&endo_presenter,
		&crown_presenter,
		&impl_presenter
	}
{
	date_validator.setAmbListDate(ambListDate);
	
}


void ProcedureDialogPresenter::setView(IProcedureDialog* view)
{
	this->view = view;
	view->setObturationPresenter(&obt_presenter);
	view->setCrownPresenter(&crown_presenter);

	for (auto p : presenters_ptr)
	{
		p->setCommonFieldsView(view->commonFields());
	}

	view->loadManipulationList(manipulationList);

	//default validator set:
	auto date = Date::getCurrentDate();

	if (!date_validator.validate(date)) {
		date = date_validator.getMin();
	}

	view->commonFields()->dateEdit()->setFieldText(Date::toString(date));
	view->commonFields()->dateEdit()->set_Validator(&date_validator);

	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index, t->temporary.exists()));

	view->setSelectionLabel(selectedTeethNum);

}

/*
std::vector<Manipulation> ProcedureDialogPresenter::generateManipulations()
{

	auto& failed = product.failedByToothNumber;

	if (failed.size())
	{
		std::string message;
		message.reserve(72 + failed.size()*4);

		message.append("Манипулацията не можа да бъде автоматично генерирана за следните зъби: ");

		for (int i = 0; i < failed.size(); i++)
		{
			
			int& idx = failed[i];
			message.append(std::to_string(utils.getToothNumber(idx, teeth->at(idx).temporary.exists())));

			if (i != failed.size() - 1) 
				message.append(", ");
		}

		view->showErrorDialog(message);

}*/


void ProcedureDialogPresenter::indexChanged(int index)
{
	currentIndex = index;
	
	if (currentIndex == -1)
	{
		view->showErrorMessage("Изберете манипулация");
		errorState = true;
		return;
	}

	errorState = false;

	auto& mt = manipulationList[currentIndex];

	view->setView(mt.type);

	current_m_presenter = presenters_ptr[static_cast<int>(mt.type)];

	current_m_presenter->setManipulationTemplate(mt);
}


void ProcedureDialogPresenter::formAccepted()
{
	if (errorState) return;

	if (!current_m_presenter->isValid()) return;

	manipulations = current_m_presenter->getManipulations();

	view->close();
}

std::vector<Manipulation> ProcedureDialogPresenter::getManipulations()
{
	return manipulations;
}