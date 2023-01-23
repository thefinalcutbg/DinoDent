#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"
#include "View/Interfaces/IProcedureDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/AmbList.h"

//this implementation is a total mess and needs refactoring

ProcedureDialogPresenter::ProcedureDialogPresenter
(
	const AmbList& ambSheet,
	const std::vector<Tooth*>& selectedTeeth,
	const Date& patientTurns18,
	bool pregnancyAllowed
)
	:

	view(nullptr),
	current_m_presenter(nullptr),
	ambList(ambSheet),
	selectedTeeth(selectedTeeth),
	patientTurns18(patientTurns18),
	date_validator(patientTurns18),
	pregnancyAllowed(pregnancyAllowed),
	noProcedureSelected(true),

	toothNonSpecific_presenter(this->selectedTeeth),
	obt_presenter(this->selectedTeeth),
	extr_presenter(this->selectedTeeth),
	endo_presenter(this->selectedTeeth),
	crown_presenter(this->selectedTeeth, ambList.teeth),
	impl_presenter(this->selectedTeeth),
	fiber_presenter(this->selectedTeeth, ProcedureType::fibersplint),
	crownRemove_presenter(this->selectedTeeth, ProcedureType::removecrown),
	postRemove_presenter(this->selectedTeeth, ProcedureType::removepost),
	bridgeSplintRemove_presenter(this->selectedTeeth, ProcedureType::removebridgeOrSplint),

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
		&crownRemove_presenter,
		&postRemove_presenter,
		&bridgeSplintRemove_presenter
	}
{


}


void ProcedureDialogPresenter::setView(IProcedureDialog* view)
{
	this->view = view;

	view->setProcedureTemplates(procedureList);

	view->commonFields()->dateEdit()->setInputValidator(&date_validator);

	view->commonFields()->dateEdit()->set_Date(ambList.newProcedureDate());

	this->procedureDateChanged(ambList.newProcedureDate());
	
	for (auto& p : presenters_ptr) p->setView(view->commonFields());

	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index, t->temporary.exists()));

	view->setSelectionLabel(selectedTeethNum);

	indexChanged(-1);
}

void ProcedureDialogPresenter::procedureDateChanged(const Date& date)
{
	procedureList.clear();

	if (User::hasNzokContract())
	{
		//getting NZOK procedures:
		procedureList = NhifProcedures::getM_Templates
		(
			date,
			User::doctor().specialty,
			date >= patientTurns18,
			pregnancyAllowed,
			ambList.nhifData.specification
		);
	}


	//getting custom procedures:
	auto customProcedures = User::practice().priceList;

	procedureList.insert(procedureList.end(), customProcedures.begin(), customProcedures.end());

	view->setProcedureTemplates(procedureList);
	
}


void ProcedureDialogPresenter::indexChanged(int index)
{
	currentIndex = index;
	
	noProcedureSelected = true;

	if (currentIndex == -1)
	{
		view->commonFields()->setErrorMsg("Изберете манипулация");
		return;
	}

	auto& procedureTemplate = procedureList[currentIndex];

	noProcedureSelected = false;

	date_validator.setProcedure(procedureList[index].code, procedureList[index].nhif);
	view->commonFields()->dateEdit()->validateInput();

	current_m_presenter = presenters_ptr[static_cast<int>(procedureTemplate.type)];
	
	current_m_presenter->setProcedureTemplate(procedureTemplate);
	

}


void ProcedureDialogPresenter::formAccepted()
{
	if (noProcedureSelected || !current_m_presenter->isValid()) return;

	procedures = current_m_presenter->getProcedures();

	view->close();
}

std::vector<Procedure> ProcedureDialogPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);
	return procedures;
}