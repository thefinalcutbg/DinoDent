#include "ReportPresenter.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"
#include "Model/Validators/AmbListValidator.h"
#include "Model/xml.h"
#include <fstream>
#include "Model/FreeFunctions.h"
#include "Model/Dental/NhifProcedures.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/TabPresenter.h"

void ReportPresenter::updateProgressBar()
{
	double percent = (static_cast<double>(m_currentIndex) / lists.size()) * 100;

	view->setPercent(percent);
}

void ReportPresenter::checkAmbList(const AmbList& list, const Patient& patient)
{
	AmbListValidator v(list, patient);

	bool isValid = v.ambListIsValid();

	if (!isValid) {

		m_hasErrors = true;

		view->appendSheet(list.getNumber(), v.getErrorMsg());
	}

	if (list.nrn.empty()) {

		isValid = false;

		m_hasErrors = true;

		view->appendSheet(list.getNumber()," не е изпратен към НЗИС");
	}

	if (list.nrn.size() && !list.his_updated) {

		isValid = false;

		m_hasErrors = true;

		view->appendSheet(list.getNumber(), " Корекциите не са отразени в НЗИС");
	}


	if (nraCheck) {

		switch (patient.insuranceStatus->status) {
		case Insured::Yes: break;

		case Insured::NoData: view->appendText(
			"За пациент с ЕГН/ЛНЧ "
			+ patient.id +
			" не са открити данни в НАП");
			isValid = false;
			m_hasErrors = true;
			break;

		case Insured::No:
			view->appendText("Пациент с ЕГН/ЛНЧ " + patient.id + " е неосигурен");
			isValid = false;
			m_hasErrors = true;
			break;
		}
	}

	if (!isValid) {

		RowInstance instance{ TabType::AmbList };
		instance.patientRowId = patient.rowid;
		instance.rowID = list.rowid;

		errorSheets.insert(std::make_pair(list.getNumber(), instance));
	}

}



void ReportPresenter::reset()
{
	//reseting
	m_report.reset();
	m_hasErrors = false;
	m_currentIndex = -1;
	errorSheets.clear();
	view->setPercent(0);
	view->enableReportButtons(false);
	view->showStopButton(false);
	view->clearText();

}

void ReportPresenter::sendToPis()
{
	if (m_hasErrors &&
		!ModalDialogBuilder::askDialog(
			"Открити са грешки в отчета. "
			"Сигурни ли сте, че искате да го изпратите към ПИС?")
		)
	{
		return;
	}
	else
	{
		if(!ModalDialogBuilder::askDialog(
			"Желаете ли да изпратите отчета към ПИС?")
		) return;
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

	//sending request to PIS
	if (pisCheck && !patient.PISHistory.has_value())
	{
		bool success =
		activitiesService.sendRequest(
			patient.type, patient.id, true,
			[=](auto& result) {if (this)this->setPISActivities(result);}
		);

		if (!success) { reset();}

		return;
	}

	//sending request to NRA
	if (nraCheck && !patient.insuranceStatus.has_value())
	{
		bool success = 
			nraService.sendRequest(
			patient,
				[=](auto& result) { if (this)this->setInsuranceStatus(result);},
			true,
			list.getDate()
		);
		
		if (!success) { reset(); };

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

void ReportPresenter::setPISActivities(const std::optional<std::vector<Procedure>>& pisProcedures)
{
	if (!pisProcedures.has_value()) { 
		reset();
		return;
	}
	
	if (m_currentIndex == -1) {
		return;
	}

	patients[lists[m_currentIndex].patient_rowid].PISHistory = pisProcedures.value();
	checkNext();
	
}

void ReportPresenter::setInsuranceStatus(const std::optional<InsuranceStatus>& insuranceStatus)
{
	if (!insuranceStatus) {
		reset();
	}

	if (m_currentIndex == -1) {
		return;
	}

	patients[lists[m_currentIndex].patient_rowid].insuranceStatus = insuranceStatus;

	checkNext();
}

void ReportPresenter::setDate(int month, int year)
{
	this->month = month; 
	this->year = year;
	reset();
}

void ReportPresenter::generateReport(bool checkPis, bool checkNra)
{

	if (!User::practice().nhif_contract) {
		ModalDialogBuilder::showError("Моля попълнете даннит от договора с НЗОК от настройки"); return;
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
		view->appendText("Не са открити амбулаторни листи за съответния месец");
		return;
	}

	//checking individual lists


	view->showStopButton(true);
	checkNext();

	return;

	

}

void ReportPresenter::finish()
{

	std::string errors;

	//checking max minutes allowed from NHIF

	double expectedPrice{ 0 };
	int sumMinutes{ 0 };

	for (auto& list : lists) {
		for (auto& procedure : list.procedures)
		{

			expectedPrice += NhifProcedures::getNZOKPrice
			(
				procedure.code.oldCode(),
				procedure.date,
				User::doctor().specialty,
				patients[list.patient_rowid].isAdult(procedure.date),
				list.nhifData.specification
			);

			sumMinutes += NhifProcedures::getDuration(procedure.code.oldCode());
		}
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


	view->appendText(
		"Mинути дейност: " + std::to_string(sumMinutes) + "\n"
		"Максимално позволени: " + std::to_string(maxMinutesAllowed) + "\n"
		"Очаквана сума : " + FreeFn::formatDouble(expectedPrice) + " лв."
	);

	m_hasErrors ?
		view->appendText("Отчетът е генериран с грешки!")
		:
		view->appendText("Отчетът е генериран успешно!");

	m_currentIndex = -1;

	m_report = XML::getReport(lists, patients);
	patients.clear();
	view->showStopButton(false);
	view->enableReportButtons(true);
	
}

void ReportPresenter::setView(IReportView* view)
{
	this->view = view;
}

void ReportPresenter::linkClicked(const std::string& sheetNumber)
{
	if (!errorSheets.count(sheetNumber)) return;

	auto& ambRow = errorSheets.at(sheetNumber);

	TabPresenter::get().open(ambRow, true);
}
