#include "ProcedureDialogPresenter.h"

#include "View/ModalDialogBuilder.h"

#include "View/Widgets/ProcedureDialog.h"
#include "Model/Date.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/AmbList.h"
#include "Model/User.h"

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
{
	procedureDate = ambList.getDate();

	if (User::hasNhifContract() &&
		procedureDate.month == Date::currentMonth() &&
		procedureDate.year == Date::currentYear() &&
		procedureDate <= Date::currentDate()
	) {

		procedureDate = Date::currentDate();
	}
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

void ProcedureDialogPresenter::setView(ProcedureDialog* view)
{
	this->view = view;

	procedure_creator.setView(view->procedureInput());

	view->procedureList()->setPresenter(&list_presenter);

	view->procedureInput()->dateEdit()->setInputValidator(&date_validator);

	view->procedureInput()->dateEdit()->set_Date(procedureDate);

	refreshNhifList();
	
	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index(), t->hasStatus(Dental::Temporary)));

	view->setSelectionLabel(selectedTeethNum);

	setCode(ProcedureCode{}, false, 0);
}

void ProcedureDialogPresenter::setCode(ProcedureCode code, bool nhif, double price)
{
	if (code.nhifCode()) {
		date_validator.setProcedure(code.nhifCode(), nhif);
		view->procedureInput()->dateEdit()->validateInput();
	}

	procedure_creator.setProcedureCode(code, nhif, price);
}

void ProcedureDialogPresenter::formAccepted()
{

	result = procedure_creator.getProcedures();

	if (result.empty()) return;

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

	std::vector<std::pair<ProcedureCode, double>> codePricePair;
	codePricePair.reserve(nhifProcedures.size());

	for (auto& code : nhifProcedures) {
		
		auto patientPrice = NhifProcedures::getPrices(
			code.nhifCode(),
			procedureDate,
			procedureDate >= patientTurns18,
			User::doctor().specialty,
			ambList.nhifData.specification
		).first;

		codePricePair.push_back(std::make_pair(code, patientPrice));
	}

	list_presenter.setNhifProcedures(codePricePair);
	q
	view->procedureList()->refresh();

}


std::vector<Procedure> ProcedureDialogPresenter::openDialog()
{
	ProcedureDialog d(*this);
	d.exec();

	return result;
}

ProcedureDialogPresenter::~ProcedureDialogPresenter()
{
}
