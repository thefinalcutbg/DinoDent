#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"
#include "View/ProcedureDialog/IProcedureDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Procedure/CustomProcedures.h"

//this implementation is a total mess and needs refactoring

ProcedureDialogPresenter::ProcedureDialogPresenter
(
	const std::vector<Tooth*>& selectedTeeth,
	const ToothContainer& teeth,
	const Date& ambListDate,
	const Date& patientTurns18,
	bool unfavourable,
	int specialty
)
	:
	procedures{},

	view(nullptr),
	common_fields(nullptr),
	current_m_presenter(nullptr),
	teeth(&teeth),
	selectedTeeth(selectedTeeth),

	date_validator(ambListDate, patientTurns18),

	_errorState(true),

	toothNonSpecific_presenter(this->selectedTeeth),
	obt_presenter(this->selectedTeeth),
	extr_presenter(this->selectedTeeth),
	endo_presenter(this->selectedTeeth),
	crown_presenter(this->selectedTeeth, teeth),
	impl_presenter(this->selectedTeeth),
	fiber_presenter(this->selectedTeeth, teeth),
	crownRemove_presenter(this->selectedTeeth),

	presenters_ptr
	{
		&general_presenter,
		&toothNonSpecific_presenter,
		&obt_presenter,
		&extr_presenter,
		&endo_presenter,
		&crown_presenter,
		&impl_presenter,
		&fiber_presenter,
		&crownRemove_presenter
	}
{


	//getting NZOK procedures:
	procedureList = MasterNZOK::instance().getM_Templates
	(
		ambListDate,
		specialty,
		ambListDate >= patientTurns18,
		unfavourable
	);

	//getting custom procedures:
	auto customProcedures = CustomProcedures::instance().getCustomProcedures();

	procedureList.insert(procedureList.end(), customProcedures.begin(), customProcedures.end());
}


void ProcedureDialogPresenter::setView(IProcedureDialog* view)
{
	this->view = view;
	view->setObturationPresenter(&obt_presenter);
	view->setCrownPresenter(&crown_presenter);
	view->setImplantPresenter(&impl_presenter);
	view->setFiberSplintPresenter(&fiber_presenter);

	for (auto p : presenters_ptr)
	{
		p->setCommonFieldsView(view->commonFields());
	}

	view->loadManipulationList(procedureList);

	view->commonFields()->dateEdit()->setInputValidator(&date_validator);

	date_validator.validateInput(Date::currentDate()) ?
		view->commonFields()->dateEdit()->set_Date(Date::currentDate())
		:
		view->commonFields()->dateEdit()->set_Date(date_validator.getValidDate());

	

	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index, t->temporary.exists()));

	view->setSelectionLabel(selectedTeethNum);

}


void ProcedureDialogPresenter::indexChanged(int index)
{
	currentIndex = index;
	
	_errorState = true;

	if (currentIndex == -1)
	{
		view->showErrorMessage("Изберете манипулация");
		return;
	}

	auto& procedureTemplate = procedureList[currentIndex];

	if (procedureTemplate.type != ProcedureType::general && !selectedTeeth.size())
	{
		view->showErrorMessage(u8"Изберете поне един зъб!");
		return;
	}

	_errorState = false;

	view->setView(procedureTemplate.type);

	current_m_presenter = presenters_ptr[static_cast<int>(procedureTemplate.type)];
	current_m_presenter->setProcedureTemplate(procedureTemplate);

	date_validator.setProcedure(procedureList[index].code, procedureList[index].nzok);
	view->commonFields()->dateEdit()->validateInput();
}


void ProcedureDialogPresenter::formAccepted()
{
	if (_errorState || !current_m_presenter->isValid()) return;

	procedures = current_m_presenter->getProcedures();

	view->close();
}

std::vector<Procedure> ProcedureDialogPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);
	return procedures;
}