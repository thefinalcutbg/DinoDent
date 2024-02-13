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
    pregnancyAllowed(pregnancyAllowed),
    procedure_creator(selectedTeeth),
    view(nullptr),
    date_validator(patientTurns18)
{}


void ProcedureDialogPresenter::setView(IProcedureDialog* view)
{
	this->view = view;

	view->setProcedureTemplates(procedureList);

	view->procedureInput()->dateEdit()->setInputValidator(&date_validator);

	view->procedureInput()->dateEdit()->set_Date(ambList.newProcedureDate());

	this->procedureDateChanged(ambList.newProcedureDate());
	
	procedure_creator.setView(view->procedureInput());

	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index(), t->hasStatus(Dental::Temporary)));

	view->setSelectionLabel(selectedTeethNum);

	indexChanged(-1);
}

void ProcedureDialogPresenter::procedureDateChanged(const Date& date)
{
	procedureList.clear();

	if (User::hasNhifContract())
	{
		auto nhifProcedures = NhifProcedures::getNhifProcedures(
			date,
			User::doctor().specialty,
			date >= patientTurns18,
			pregnancyAllowed,
			ambList.nhifData.specification
		);
		
		for (auto& p : nhifProcedures)
		{
			procedureList.push_back({ p, true });
		}
	}


	//getting custom procedures:
	auto customProcedures = ProcedureCode::getNonNhifProcedures();

	auto favourites = DbDoctor::getFavouriteProcedures(User::doctor().LPK);

	for (auto& p : customProcedures)
	{
		procedureList.push_back(
			ProcedureListElement{
				.code = p,
				.nhif = false,
				.favourite = static_cast<bool>(favourites.count(p.code()))
			}
		);

	}

	sortProcedures();

	view->setProcedureTemplates(procedureList);
	
}


void ProcedureDialogPresenter::indexChanged(int index)
{
	currentIndex = index;

	if (currentIndex == -1)
	{
		view->procedureInput()->setErrorMsg("Изберете манипулация");
		return;
	}

	auto& procedureCode = procedureList[currentIndex];

	date_validator.setProcedure(procedureList[index].code.oldCode(), procedureList[index].nhif);
	view->procedureInput()->dateEdit()->validateInput();


	
	procedure_creator.setProcedureCode(procedureCode.code, procedureCode.nhif);
	

}

void ProcedureDialogPresenter::favouriteClicked(int index)
{
	auto& fav = procedureList[index].favourite;

	fav = !fav;
	sortProcedures();

	favourites_changed = true;

	view->setProcedureTemplates(procedureList);
}


void ProcedureDialogPresenter::formAccepted()
{
	if (currentIndex == -1 || !procedure_creator.isValid()) return;

	procedures = procedure_creator.getProcedures();

	view->close();
}

void ProcedureDialogPresenter::sortProcedures()
{
	std::sort(procedureList.begin(), procedureList.end(),
	
	[](const ProcedureListElement& a, const ProcedureListElement& b)
	{
		if (a.nhif > b.nhif) return true;

		if (a.nhif == b.nhif && a.favourite > b.favourite) {
			return true;
		}

		if (a.nhif == b.nhif && a.favourite == b.favourite) {
			return a.code.oldCode() < b.code.oldCode();
		}

		return false;

	});
}


std::vector<Procedure> ProcedureDialogPresenter::openDialog()
{
    ModalDialogBuilder::openDialog(*this);
	return procedures;
}

ProcedureDialogPresenter::~ProcedureDialogPresenter()
{
	if (!favourites_changed) return;

	//updating in DB
	std::vector<std::string> favCodes;

	for (auto& p : procedureList) {
		if (p.favourite) {
			favCodes.push_back(p.code.code());
		}
	}

	DbDoctor::updateFavouriteProcedures(favCodes, User::doctor().LPK);
}
