#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"
#include "View/ProcedureDialog/IProcedureDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/AmbList.h"

//this implementation is a total mess and needs refactoring

ProcedureDialogPresenter::ProcedureDialogPresenter
(
	const AmbList& ambSheet,
	const std::vector<Tooth*>& selectedTeeth,
	const Date& patientTurns18
)
	:

	view(nullptr),
	common_fields(nullptr),
	current_m_presenter(nullptr),
	ambList(ambSheet),
	selectedTeeth(selectedTeeth),
	patientTurns18(patientTurns18),
	date_validator(patientTurns18),
	_errorState(true),

	toothNonSpecific_presenter(this->selectedTeeth),
	obt_presenter(this->selectedTeeth),
	extr_presenter(this->selectedTeeth),
	endo_presenter(this->selectedTeeth),
	crown_presenter(this->selectedTeeth, ambList.teeth),
	impl_presenter(this->selectedTeeth),
	fiber_presenter(this->selectedTeeth, ambList.teeth),
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

	view->setProcedureTemplates(procedureList);

	view->commonFields()->dateEdit()->setInputValidator(&date_validator);


	view->commonFields()->dateEdit()->set_Date(ambList.newProcedureDate());

	this->procedureDateChanged(ambList.newProcedureDate());
	

	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index, t->temporary.exists()));

	view->setSelectionLabel(selectedTeethNum);

}

void ProcedureDialogPresenter::procedureDateChanged(const Date& date)
{
	procedureList.clear();

	if (User::practice().nzok_contract && User::doctor().specialty != NhifSpecialty::None)
	{
		//getting NZOK procedures:
		procedureList = MasterNZOK::instance().getM_Templates
		(
			date,
			User::doctor().specialty,
			date >= patientTurns18,
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
	
	_errorState = true;

	if (currentIndex == -1)
	{
		view->showErrorMessage(u8"Изберете манипулация");
		return;
	}

	auto& procedureTemplate = procedureList[currentIndex];

	if (!selectedTeeth.size() &&
		procedureTemplate.type != ProcedureTemplateType::general &&
		procedureTemplate.type != ProcedureTemplateType::prosthodontic && 
		procedureTemplate.type != ProcedureTemplateType::fibersplint
		)
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