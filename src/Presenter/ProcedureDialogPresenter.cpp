#include "ProcedureDialogPresenter.h"

#include "Model/Dental/ToothUtils.h"
#include "View/ModalDialogBuilder.h"

#include "View/Widgets/ProcedureDialog.h"
#include "Model/Date.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/AmbList.h"
#include "Model/User.h"

ProcedureDialogPresenter::ProcedureDialogPresenter
(
	const std::vector<const Tooth*>& selectedTeeth,
    const Date& procedureDate,
    const std::optional<NhifData>& nhifData
)
	:
    selectedTeeth(selectedTeeth),
    nhifData(nhifData),
    procedureDate(procedureDate),
    procedure_creator(selectedTeeth),
    view(nullptr)
{
    if(!User::hasNhifContract() || !nhifData) return;

    date_validator = ProcedureDateValidator(nhifData->patientTurns18);

    if (
		procedureDate.month == Date::currentMonth() &&
		procedureDate.year == Date::currentYear() &&
		procedureDate <= Date::currentDate()
    ) {

        this->procedureDate = Date::currentDate();
	}
}

void ProcedureDialogPresenter::procedureDateChanged(const Date& date)
{
	bool needsRefresh =
        nhifData &&
        date < nhifData->patientTurns18 != procedureDate < nhifData->patientTurns18 &&
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

    auto dateField = view->procedureInput()->dateEdit();

    dateField->setInputValidator(&date_validator);

    dateField->set_Date(procedureDate);

    if(!User::practice().generateMonthlySheets()){
        view->procedureInput()->hideDate();
    }

	if (User::settings().showProcedurePrices) {
		view->procedureList()->showPrices();
	}

    bool treatmentPlanMode = !nhifData.has_value();

    if(treatmentPlanMode){
        view->procedureInput()->setTreatmentPlanMode();
    }

	refreshNhifList();
	
	//setting the label
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
		selectedTeethNum.emplace_back(ToothUtils::getToothNumber(t->index(), t->hasStatus(Dental::Temporary)));

	view->setSelectionLabel(selectedTeethNum);

    setCode(ProcedureCode{}, false);
}

void ProcedureDialogPresenter::setCode(ProcedureCode code, bool nhif)
{
	if (code.nhifCode()) {
		date_validator.setProcedure(code.nhifCode(), nhif);
		view->procedureInput()->dateEdit()->validateInput();
	}

    procedure_creator.setProcedureCode(code, nhif);
}

void ProcedureDialogPresenter::formAccepted()
{

	result = procedure_creator.getProcedures();

	if (result.empty()) return;

	view->close();
}

void ProcedureDialogPresenter::refreshNhifList()
{

    if (!User::hasNhifContract() || !nhifData) return;

	auto nhifProcedures = NhifProcedures::getNhifProcedures(
		procedureDate,
		User::doctor().specialty,
        procedureDate >= nhifData->patientTurns18,
        nhifData->pregnancyAllowed,
        nhifData->specType
	);

	std::vector<std::tuple<ProcedureCode, double, double>> codePricePair;
	codePricePair.reserve(nhifProcedures.size());

	for (auto& code : nhifProcedures) {
		
		auto [patientPrice, nhifPrice] = NhifProcedures::getPrices(
			code.nhifCode(),
			procedureDate,
            procedureDate >= nhifData->patientTurns18,
			User::doctor().specialty,
            nhifData->specType
		);

		codePricePair.push_back(std::make_tuple(code, patientPrice, nhifPrice));
	}

	list_presenter.setNhifProcedures(codePricePair);

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
