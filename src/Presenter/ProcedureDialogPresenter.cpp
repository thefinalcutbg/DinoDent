#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"
#include "View/Interfaces/IProcedureDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/AmbList.h"
#include "Model/User.h"
#include "Database/DbDoctor.h"

ProcedureDialogPresenter::ProcedureDialogPresenter
(
	const AmbList& ambSheet,
	const std::vector<const Tooth*>& selectedTeeth,
	const Date& patientTurns18,
	bool pregnancyAllowed
)
	:
    selectedTeeth(selectedTeeth),
	ambList(ambSheet),
    patientTurns18(patientTurns18),
	procedureDate(ambList.newProcedureDate()),
    pregnancyAllowed(pregnancyAllowed),
    procedure_creator(selectedTeeth),
    view(nullptr),
    date_validator(patientTurns18)
{
	if (sectionIndex == -1) {
		sectionIndex = DbDoctor::getFavouriteProcedures(User::doctor().LPK).empty() ? 0 : 1;
	}
}


void ProcedureDialogPresenter::setView(IProcedureDialog* view)
{
	this->view = view;

	view->setProcedureSections(procedureList.getSectionList(), sectionIndex);

	view->procedureInput()->dateEdit()->setInputValidator(&date_validator);

	view->procedureInput()->dateEdit()->set_Date(procedureDate);

	refreshNhifList();
	
	procedure_creator.setView(view->procedureInput());

	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index(), t->hasStatus(Dental::Temporary)));

	view->setSelectionLabel(selectedTeethNum);

	setCode(ProcedureCode{}, false);
}

void ProcedureDialogPresenter::procedureDateChanged(const Date& date)
{
	bool needsRefresh = 
		date < patientTurns18 != procedureDate < patientTurns18 &&
		User::hasNhifContract();
	
	procedureDate = date;
	
	if (needsRefresh) {
		refreshNhifList();
	}
	
}

void ProcedureDialogPresenter::sectionChanged(int index)
{
	sectionIndex = index;
	view->setProcedureTemplates(procedureList.getList(index));
}


void ProcedureDialogPresenter::setCode(ProcedureCode code, bool nhif)
{

	if (!code.isValid()) {

		view->procedureInput()->setErrorMsg("Изберете манипулация");
		return;
	}

	if (code.oldCode()) {

		date_validator.setProcedure(code.oldCode(), nhif);
		view->procedureInput()->dateEdit()->validateInput();

	}

	procedure_creator.setProcedureCode(code, nhif);
	
}

void ProcedureDialogPresenter::favouriteClicked(const std::string& code)
{
	
	favourites_changed = true;

	procedureList.favClicked(sectionIndex, code);

	view->setProcedureTemplates(procedureList.getList(sectionIndex));

}


void ProcedureDialogPresenter::formAccepted()
{

	//if (currentIndex == -1 || !procedure_creator.isValid()) return;

	result = procedure_creator.getProcedures();

	view->close();
}

void ProcedureDialogPresenter::refreshNhifList()
{

	if (!User::hasNhifContract()) return;

	auto nhifProcedures = NhifProcedures::getNhifProcedures(
		procedureDate,
		User::doctor().specialty,
		procedureDate >= patientTurns18,
		pregnancyAllowed,
		ambList.nhifData.specification
	);

	procedureList.setNhifProcedures(nhifProcedures);

	//not focused on block
	if (sectionIndex < 2) {
		view->setProcedureTemplates(procedureList.getList(sectionIndex));
	}

}


std::vector<Procedure> ProcedureDialogPresenter::openDialog()
{
    ModalDialogBuilder::openDialog(*this);
	return result;
}

ProcedureDialogPresenter::~ProcedureDialogPresenter()
{
	if (!favourites_changed) return;

	//updating in DB
	std::vector<std::string> favCodes;
/*
	for (auto& p : procedureList) {
		if (p.favourite) {
			favCodes.push_back(p.code.code());
		}
	}
	*/
	DbDoctor::updateFavouriteProcedures(favCodes, User::doctor().LPK);
}
