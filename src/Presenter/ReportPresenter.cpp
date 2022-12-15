#include "ReportPresenter.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"
#include "Model/Validators/AmbListValidator.h"
#include "Model/xml.h"
#include <fstream>
#include "Model/FreeFunctions.h"
#include "Model/Dental/NhifProcedures.h"
#include "View/ModalDialogBuilder.h"

//returns empty string if valid
std::string consecutionCheck(const std::vector<AmbList>& lists) {

	std::unordered_set<int> uniqueSheetNumbers;

	std::string errors;

	for (int i = 0; i < lists.size(); i++)
	{
		//check list num uniqeness
		if (uniqueSheetNumbers.count(lists[i].number))
		{
			errors +=
				"Дублирана номерация на амбулаторен лист "
				+ std::to_string(lists[i].number) + "\n";

			uniqueSheetNumbers.insert(lists[i].number);

			continue;
		}

		uniqueSheetNumbers.insert(lists[i].number);

		if (i && lists[i].getDate() < lists[i - 1].getDate()) {

			errors +=
				"Несъответствие на датите и поредните номера между амбулаторни листи "
				+ std::to_string(lists[i - 1].number) +
				" и " + std::to_string(lists[i].number) + "\n";
		}

		if (i && lists[i].number != lists[i - 1].number + 1) {

			errors += "Нарушена поредност на номерата. Липсваща номерация между амбулаторни листи "
				+ std::to_string(lists[i - 1].number) +
				" и " + std::to_string(lists[i].number) + "\n";
		}
	}

	return errors;

}

void ReportPresenter::updateProgressBar()
{
	double percent = (static_cast<double>(m_currentIndex) / lists.size()) * 100;

	view->setPercent(percent);
}

void ReportPresenter::checkAmbList(const AmbList& list, const Patient& patient)
{
	AmbListValidator v(list, patient);

	bool result = v.ambListIsValid();

	if (!result) {

		m_hasErrors = true;

		view->appendText(

			"Амбулаторен лист №"
			+ std::to_string(list.number) + ": "
			+ v.getErrorMsg()
			
		);

	}

	if (!nraCheck) return;

	switch (patient.insuranceStatus->status) {
	case Insured::Yes: break;

	case Insured::NoData: view->appendText(
		"За пациент с ЕГН/ЛНЧ "
		+ patient.id +
		" не са открити данни в НАП");
		result = false;
		m_hasErrors = true;
		break;

	case Insured::No:
		view->appendText("Пациент с ЕГН/ЛНЧ " + patient.id + " е неосигурен");
		m_hasErrors = true;
		break;
	}


}



void ReportPresenter::reset()
{
	//reseting
	m_report.reset();
	m_hasErrors = false;
	m_currentIndex = -1;
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

	sendFileService.sendAmbReport(
		m_report.value(),
		User::doctor().egn
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
			patient.type, patient.id,
			[=](auto& result, const std::vector<std::string>&) {if (this)this->setPISActivities(result);}

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

	if (!User::practice().nzok_contract) {
		ModalDialogBuilder::showError("Моля попълнете даннит от договора с НЗОК от настройки"); return;
	}

	if (User::doctor().specialty == NhifSpecialty::None) {
		ModalDialogBuilder::showError("Лекарят няма въведена специалност по НЗОК"); return;
	}

	//getting amblists and patients:

	pisCheck = checkPis;
	nraCheck = checkNra;

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
	//checking list number and date sequence - this will probably become obsolete with HIS

	auto errors = consecutionCheck(lists);

	//checking max minutes allowed from NHIF

	double expectedPrice{ 0 };
	int sumMinutes{ 0 };

	for (auto& list : lists) {
		for (auto& procedure : list.procedures)
		{

			expectedPrice += NhifProcedures::getNZOKPrice
			(
				procedure.code,
				procedure.date,
				User::doctor().specialty,
				patients[list.patient_rowid].isAdult(procedure.date),
				list.nhifData.specification
			);

			sumMinutes += NhifProcedures::getDuration(procedure.code);
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
