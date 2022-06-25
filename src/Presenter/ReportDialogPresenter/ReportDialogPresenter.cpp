#include "ReportDialogPresenter.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"
#include "Network/PISServ.h"
#include "Model/AmbListValidator.h"
#include "Network/PISServ.h"
#include "Model/XML/xml.h"
#include <fstream>
#include "Model/FreeFunctions.h"

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
				u8"Дублирана номерация на амбулаторен лист "
				+ std::to_string(lists[i].number) + "\n";

			uniqueSheetNumbers.insert(lists[i].number);

			continue;
		}

		uniqueSheetNumbers.insert(lists[i].number);

		if (i && lists[i].procedures[0].date < lists[i - 1].procedures[0].date) {

			errors +=
				u8"Несъответствие на датата на първите манипулации и поредните номера между амбулаторни листи "
				+ std::to_string(lists[i - 1].number) +
				u8" и " + std::to_string(lists[i].number) + "\n";
		}

		if (i && lists[i].number != lists[i - 1].number + 1) {

			errors += u8"Нарушена поредност на номерата. Липсваща номерация между амбулаторни листи "
				+ std::to_string(lists[i - 1].number) +
				u8" и " + std::to_string(lists[i].number) + "\n";
		}
	}

	return errors;

}


void ReportDialogPresenter::pisCheckNext()
{ 
	if (m_currentIndex >= lists.size()) {
		finish();
		return;
	}

	auto& list = lists[m_currentIndex];

	auto& patient = patients[list.patient_rowid];

	if(!patient.PISHistory.has_value())
	{
		bool success = 
			PIS::sendRequest(
			SOAP::dentalActivities(
				patient.id, patient.type),
				reply_handler
		);

		if (!success) {
			abort();
		}

		qDebug() << "checking patient " << patient.id.c_str();

		return;

	}

	if (!checkAmbList(list, patient)) {
		abort();
		return;
	}

	m_currentIndex++;
	updateProgressBar();
	pisCheckNext();

	
	
}

void ReportDialogPresenter::updateProgressBar()
{
	double percent = (static_cast<double>(m_currentIndex) / lists.size()) * 100;

	view->setPercent(percent);
}

bool ReportDialogPresenter::checkAmbList(const AmbList& list, const Patient& patient)
{
	AmbListValidator v(list, patient);

	bool result = v.ambListIsValid();

	if (!result) {

		std::string error{
			u8"Амбулаторен лист №"
			+ std::to_string(list.number) + ": "
			+ v.getErrorMsg()
		};

		view->appendText(error);

	}

	return result;
}

void ReportDialogPresenter::finish()
{
	auto errors = consecutionCheck(lists);

	double expectedPrice{0};
	int sumMinutes{ 0 };

	for (auto& list : lists) {
		for (auto& procedure : list.procedures) {

			expectedPrice += MasterNZOK::instance().getNZOKPrice
			(
				procedure.code,
				procedure.date,
				UserManager::currentUser().doctor.specialty,
				patients[list.patient_rowid].isAdult(procedure.date),
				false
			);

			sumMinutes += MasterNZOK::instance().getDuration(procedure.code);
		}
	}

	int maxMinutesAllowed = Date::getWorkdaysOfMonth(month, year) * 360;

	std::string result = u8"Отчетът е генериран успешно!\n"
			u8"Mинути дейност: " + std::to_string(sumMinutes) + "\n"
			u8"Максимално позволени: " + std::to_string(maxMinutesAllowed) + "\n"
			u8"Очаквана сума : " + formatDouble(expectedPrice) + u8" лв.";

	view->appendText(result);

	m_report = XML::getReport(lists, patients);
	view->enableReportButtons(true);
}

void ReportDialogPresenter::abort()
{
	ModalDialogBuilder::showError(u8"Нещо се случи");
	//reseting
	this->setDate(month, year);
}

void ReportDialogPresenter::sendToPis()
{

}

void ReportDialogPresenter::saveToXML()
{

	//getting the filename
	auto& user = UserManager::currentUser();

	auto fileName = "STOM_"
		+ user.practice.rziCode + "_"
		+ user.doctor.LPK + "_"
		+ std::to_string(user.doctor.specialty) + "_"
		+ Date(1, month, year).toXMLReportFileName()
		+ "_01.xml";

	//getting the filepath
	auto result = ModalDialogBuilder::getFileNamePath(fileName);

	if (!result.has_value()) return;

	auto& filepath = result.value();

	//saving
	std::ofstream out(result.value());
	out << m_report.value();
	out.close();

	ModalDialogBuilder::openExplorer(filepath);
}

void ReportDialogPresenter::setPISActivities(const std::optional<Procedures>& pisProcedures)
{
	if (!pisProcedures.has_value()) {
		
		abort();
		return;
	}

	patients[lists[m_currentIndex].patient_rowid].PISHistory = pisProcedures.value();

	qDebug() << "pis procedures recieved count: " << pisProcedures.value().size();

	pisCheckNext();
}

void ReportDialogPresenter::setDate(int month, int year)
{
	this->month = month; 
	this->year = year;
	m_report.reset();
	view->setPercent(0);
	view->enableReportButtons(false);
	view->clearText();
}

void ReportDialogPresenter::generateReport(bool checkPis)
{
	//getting amblists and patients:

	lists.clear();

	view->clearText();

	m_currentIndex = 0;

	auto rowIds = DbAmbList::getRowIdNhif(month, year);

	lists.reserve(rowIds.size());

	for (auto rowid : rowIds) {
		
		lists.emplace_back(DbAmbList::getListNhifProceduresOnly(rowid));

		auto patientRowId = lists.back().patient_rowid;
		
		if (!patients.count(patientRowId))
		{
			patients[patientRowId] = DbPatient::get(patientRowId);
		}
		
	}

	if (lists.empty()) {
		view->appendText(u8"Не са открити амбулаторни листи за съответния месец");
		return;
	}

	//checking list number and date sequence - this will probably become obsolete with HIS

	auto errors = consecutionCheck(lists);

	//checking max minutes allowed from NHIF

	double expectedPrice{ 0 };
	int sumMinutes{ 0 };

	for (auto& list : lists) {
		for (auto& procedure : list.procedures) {

			expectedPrice += MasterNZOK::instance().getNZOKPrice
			(
				procedure.code,
				procedure.date,
				UserManager::currentUser().doctor.specialty,
				patients[list.patient_rowid].isAdult(procedure.date),
				false
			);

			sumMinutes += MasterNZOK::instance().getDuration(procedure.code);
		}
	}

	int maxMinutesAllowed = Date::getWorkdaysOfMonth(month, year) * 360;

	if (maxMinutesAllowed < sumMinutes) {
		errors.append(u8"Надвишени лимит минути по НЗОК (" + std::to_string(sumMinutes)
			+ " от максимално позволени " + std::to_string(maxMinutesAllowed) + ")\n");
	}

	if (!errors.empty()) {
		view->appendText(errors);
		return;
	}

	//checking individual lists

	if (checkPis) {
		pisCheckNext();
		return;
	}
	
	for (auto list : lists) {
		bool valid = checkAmbList(list, patients[list.patient_rowid]);
		
		if (!valid) {
			abort();
			return;
		}

		m_currentIndex++;
		updateProgressBar();
	}
	
	finish();
}

void ReportDialogPresenter::setView(IReportDialog* view)
{
	this->view = view;
}
