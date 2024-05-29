#include "PisImportPresenter.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Network/PIS/PisReportParser.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"
#include "View/Widgets/PisImportView.h"

void PisImportPresenter::setView(PisImportView* view)
{
	this->view = view;
}

void PisImportPresenter::importData(int fromYear, int toYear)
{
	m_years = std::stack<int>{};
	m_hashes = std::stack<std::string>{};

	while (fromYear <= toYear) {
		m_years.push(fromYear);
		fromYear++;
	}

	view->disableButton(true);
	view->clearConsole();

	getNextYearHashes();
}

void PisImportPresenter::getNextYearHashes()
{
	if (m_years.empty()) {
		view->disableButton(false);
		view->logToConsole("Импортирането приключи!");
		return;
	}

	bool querySent = get_hash_serv.sendRequest(User::doctor().LPK, m_years.top(),
		[&](const std::stack<std::string>& hashes) { 

			if (hashes.empty()) {

				std::string msg = "Не са открити амбулаторни листове от ";
				msg += std::to_string(m_years.top());
				msg += " г.";

				view->logToConsole(msg);
				m_years.pop();

				getNextYearHashes();

				return;

			}
			
			m_hashes = hashes;

			view->setProgresSize(hashes.size());

			std::string msg = "Изтегляне на амбулаторни листове от ";
			msg += std::to_string(m_years.top());;
			msg += " г.";

			m_years.pop();

			view->logToConsole(msg);

			processHash();
		}
	);

	if (!querySent) {
		m_years = std::stack<int>();
		view->logToConsole("Импортирането бе прекъснато. Пробвайте отново.");
		view->disableButton(false);
	}
	
}

void PisImportPresenter::processHash()
{
	if (m_hashes.empty()) {
		getNextYearHashes();
		return;
	}

	bool querySent = get_file_serv.sendRequest(m_hashes.top(), User::doctor().LPK,
		[&](const std::string& file) {

			if (file.empty()) {
				m_years = std::stack<int>();
				view->logToConsole("Импортирането бе прекъснато. Пробвайте отново.");
				view->disableButton(false);
				return;
			}

			importToDb(file);
			m_hashes.pop();
			processHash();
		}
	);

	if (!querySent) {
		m_years = std::stack<int>();
		view->logToConsole("Импортирането бе прекъснато. Пробвайте отново.");
		view->disableButton(false);
	}
}

void PisImportPresenter::importToDb(const std::string& file)
{
	auto result = PisReportParser::parse(file);

	view->incrementProgress();

	for (auto& pair : result)
	{
		Patient patient = DbPatient::get(pair.first.id, pair.first.type);

		if (!patient.rowid) {
			patient = pair.first;
			patient.rowid = DbPatient::insert(patient);

			std::string logMsg = "Импортиране на пациент ";
			logMsg += pair.first.fullName();

			view->logToConsole(logMsg);
		}
		else
		{
			bool needsUpdating = false;

			if (patient.HIRBNo.empty()) {
				patient.HIRBNo = pair.first.HIRBNo;
				needsUpdating = true;
			}

			if (!patient.city.isValid()) {
				patient.city = pair.first.city;
				needsUpdating = true;
			}

			if (needsUpdating) {

				DbPatient::update(patient);

				std::string logMsg = "Актуализиране на пациент ";
				logMsg += pair.first.fullName();

				view->logToConsole(logMsg);
			}
		}

		pair.second.patient_rowid = patient.rowid;

		if (DbAmbList::importedPisSheetExists(pair.second, patient)) {
			continue;
		}

		std::string logMsg = "Импортиране на амбулаторен лист ";
		logMsg += pair.second.getNumber();
		view->logToConsole(logMsg);

		DbAmbList::insert(pair.second, pair.second.patient_rowid);
	}
}

void PisImportPresenter::abort()
{
	view->setProgresSize(1);
	view->disableButton(false);
}

void PisImportPresenter::loadXml()
{
	auto filesData = ModalDialogBuilder::openFiles();

	if (filesData.empty()) return;

	view->disableButton(true);

	view->setProgresSize(filesData.size());

	for (auto& data : filesData)
	{
		importToDb(data);
	}

	view->logToConsole("Импортирането приключи!");

	view->setProgresSize(12);

	view->disableButton(false);
}

