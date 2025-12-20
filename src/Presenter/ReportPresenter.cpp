#include "ReportPresenter.h"

#include <fstream>

#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"

#include "Model/Validators/AmbListValidator.h"
#include "Model/User.h"
#include "Model/xml.h"
#include "Model/FreeFunctions.h"
#include "Model/Dental/NhifProcedures.h"

#include "Presenter/TabPresenter.h"

#include "View/Widgets/ReportView.h"
#include "View/ModalDialogBuilder.h"

#include "Printer/Print.h"

void ReportPresenter::updateProgressBar()
{
	double percent = (static_cast<double>(m_currentIndex) / lists.size()) * 100;

	view->setPercent(percent);
}

void ReportPresenter::checkAmbList(const AmbList& list, const Patient& patient)
{

    bool isValid = true;

    std::string amblistName = list.nrn.size() ?
    list.nrn : " на " + patient.firstLastName();


	if (list.nrn.empty()) {

		isValid = false;

		m_hasErrors = true;

        view->appendSheet(list.rowid, amblistName, "Не е изпратен към НЗИС");
	}

	if (list.nrn.size() && !list.his_updated) {

		isValid = false;

		m_hasErrors = true;

        view->appendSheet(list.rowid, amblistName, "Корекциите не са отразени в НЗИС");
	}

	if (nraCheck && patient.insuranceStatus.has_value() && !patient.foreigner) {

		switch (patient.insuranceStatus->status) {
	
			case Insured::Yes: break;

            case Insured::NoData:
                view->appendSheet(list.rowid, amblistName, "В НАП не е открит здравноосигурителен статус за този пациент");
				isValid = false;
				m_hasErrors = true;
				break;

			case Insured::No:
                view->appendSheet(list.rowid, amblistName, "Пациентът не е здравноосигурен");
				isValid = false;
				m_hasErrors = true;
				break;
		}
	}

    AmbListValidator v(list, patient);

    if (!v.ambListIsValid()) {

        m_hasErrors = true;

        auto errorMsg = v.getErrorMsg();

        //avoid exceeded limit duplication messages
        auto strBegin = std::string("За дата");

        if(errorMsg.substr(0, strBegin.size()) == strBegin){

            if(exceededDailyLimitSet.contains(errorMsg)) return;

            view->appendText(errorMsg);
            exceededDailyLimitSet.insert(errorMsg);

            return;

        } else {
            isValid = false;
            view->appendSheet(list.rowid, amblistName, v.getErrorMsg());
        }
    }


	if (!isValid) {

		RowInstance instance{ TabType::AmbList };
		instance.patientRowId = patient.rowid;
		instance.rowID = list.rowid;

		errorSheets.insert(std::make_pair(list.rowid, instance));
	}

}

void ReportPresenter::reset()
{
	//reseting
	m_report.reset();
	m_hasErrors = false;
	m_currentIndex = -1;
	errorSheets.clear();
    exceededDailyLimitSet.clear();
	view->setPercent(0);
	view->enableReportButtons(false);
	view->showStopButton(false);
	view->clearText();

}

void ReportPresenter::sendToPis()
{
	auto dialogText = m_hasErrors ?
		"Открити са грешки в отчета. "
		"Сигурни ли сте, че искате да го изпратите към ПИС?"
		:
		"Желаете ли да изпратите отчета към ПИС?";


	if (!ModalDialogBuilder::askDialog(dialogText)) {
		return;
	}


	sendFileService.sendAmbReport(
		m_report.value(),
		User::doctor().LPK
	);


}

void ReportPresenter::checkNext()
{
	if (m_currentIndex >= lists.size()) {
		finish();
		return;
	}

	auto& list = lists[m_currentIndex];

	auto& patient = patients[list.patient_rowid];

    std::vector<BulkRequester::RequestType> requests;

    if(nraCheck && !patient.insuranceStatus){
        requests.push_back(BulkRequester::NraStatus);

    } else if (patient.foreigner){
        patient.insuranceStatus.emplace();
    }

	if (pisCheck && !patient.PISHistory.has_value())
	{
        requests.push_back(BulkRequester::NhifProcedures);
	}

    if(requests.size()){
        bulk_requester.setCallback([this](const BulkRequester::Result& r){ resultRecieved(r);});
        bulk_requester.sendRequest(patient, requests);
        return;
    }

	checkAmbList(list, patient);

	m_currentIndex++;
	updateProgressBar();
	checkNext();
}

void ReportPresenter::saveToXML()
{
	if (m_hasErrors && 
		!ModalDialogBuilder::askDialog(
		"Отчетът е генериран с грешки. "
		"Сигурни ли сте, че искате да го запазите?")
	) 
	{
		return;
	}

	auto fileName = "STOM_"
		+ User::practice().rziCode + "_"
		+ User::doctor().LPK + "_"
		+ std::to_string(User::doctor().specialtyAsInt()) + "_"
		+ Date(1, month, year).toXMLReportFileName()
		+ "_01.xml";

	ModalDialogBuilder::saveFile(m_report.value(), fileName);
}

void ReportPresenter::setDate(int month, int year)
{
	this->month = month; 
	this->year = year;
	patients.clear();
	reset();
}

void ReportPresenter::generateReport(bool checkPis, bool checkNra)
{

	if (!User::practice().nhif_contract) {
		ModalDialogBuilder::showError("Моля попълнете данните от договора с НЗОК от настройки"); return;
	}

	if (User::doctor().specialty == NhifSpecialty::None) {
		ModalDialogBuilder::showError("Лекарят няма въведена специалност по НЗОК"); return;
	}

	//getting amblists and patients:

	pisCheck = checkPis;
	nraCheck = checkNra;

	errorSheets.clear();
	view->clearText();
	view->enableReportButtons(false);
	view->setPercent(0);

    exceededDailyLimitSet.clear();
	m_hasErrors = false;
	m_currentIndex = 0;

	lists = DbAmbList::getMonthlyNhifSheets(month, year);

	for (auto& l : lists)
	{
		auto& patientRowId = l.patient_rowid;

		if (!patients.count(patientRowId))
		{
			patients[patientRowId] = DbPatient::get(patientRowId);
		}
	}

	if (lists.empty()) {
		view->appendText("Не са открити амбулаторни листове за съответния месец");
		return;
	}

	//checking individual lists


	view->showStopButton(true);
	checkNext();

	return;
}

void ReportPresenter::generateSpecification()
{

	static std::vector<std::string> specOptions{
			"Изцяло или частично заплащана от НЗОК",
			"Изцяло заплащана от НЗОК",
			"За лица с психични заболявания под обща анестезия"
	};

	int spec = ModalDialogBuilder::openButtonDialog(specOptions, "Изберете спецификация");

	if (spec == -1) return;

	auto spec_report = NhifSpecReport(User::doctor(), Date(1, month, year), static_cast<NhifSpecificationType>(spec));

    for (auto& list : lists) for (auto& procedure : list.procedures) {
		spec_report.addProcedure(
			procedure,
			patients[list.patient_rowid].isAdult(procedure.date),
			list.nhifData.specification
		);
	}

	Print::saveNhifSpecReport(spec_report);
}

void ReportPresenter::finish()
{

	std::string errors;

	//checking max minutes allowed from NHIF

	int sumMinutes{ 0 };
	double sumPrice{ 0 };

	for (auto& list : lists) for (auto& procedure : list.procedures)
	{
		sumMinutes += NhifProcedures::getDuration(procedure.code.nhifCode());

		auto procedurePrice = NhifProcedures::getNhifPrice(
			procedure.code.nhifCode(),
			procedure.date,
			User::doctor().specialty,
			patients[list.patient_rowid].isAdult(procedure.date),
			list.nhifData.specification
		);

		if (list.nhifData.isUnfavourable) {
			procedurePrice *= 1.3;
		}

		sumPrice += procedurePrice;
	}
	

	int maxMinutesAllowed = Date::getWorkdaysOfMonth(month, year) * 360;

	if (maxMinutesAllowed < sumMinutes) {
		errors.append("Надвишени лимит минути по НЗОК (" + std::to_string(sumMinutes)
			+ " от максимално позволени " + std::to_string(maxMinutesAllowed) + ")\n");
	}

	if (!errors.empty()) {
		view->appendText(errors);
		m_hasErrors = true;
	}


	auto priceSuffix = year >= 2026 || (year == 2025 && month == 12) ? " €" : " лв.";
	
	view->appendText(
		"Минути дейност: " + std::to_string(sumMinutes) + " мин.\n"
		"Максимално позволени: " + std::to_string(maxMinutesAllowed) + " мин.\n"
		"Очаквана сума : " + FreeFn::formatDouble(sumPrice) + priceSuffix
	);

	m_hasErrors ?
		view->appendText("Отчетът е генериран с грешки!")
		:
		view->appendText("Отчетът е генериран успешно!");

	m_currentIndex = -1;

	m_report = XML::getReport(lists, patients);
	
	view->showStopButton(false);
	view->enableReportButtons(true);
	
}

void ReportPresenter::resultRecieved(const BulkRequester::Result &r)
{
    if (m_currentIndex == -1) {
        return;
    }

    if (pisCheck && !r.pisDentalActivities.has_value()) {
        reset();
        ModalDialogBuilder::showError("Неуспешна връзка с ПИС");
        return;
    } else if (pisCheck){
        patients[lists[m_currentIndex].patient_rowid].PISHistory = r.pisDentalActivities.value();
    }

    if(nraCheck){
        if(!r.nraStatus){
            ModalDialogBuilder::showError("Неуспешна връзка с НАП");
            reset();
            return;
        }

        patients[lists[m_currentIndex].patient_rowid].insuranceStatus = r.nraStatus;
    }

    checkNext();
}

void ReportPresenter::setView(ReportView* view)
{
	this->view = view;
}

void ReportPresenter::linkClicked(long long sheetRowid)
{
	if (!errorSheets.count(sheetRowid)) return;

	auto& ambRow = errorSheets.at(sheetRowid);

	TabPresenter::get().open(ambRow, true);
}
