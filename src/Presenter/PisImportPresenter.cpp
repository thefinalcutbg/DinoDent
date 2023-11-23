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

void PisImportPresenter::importData(int year)
{

	if (m_hashes.size()) {
		processHash();
		return;
	}

	bool begin = get_hash_serv.sendRequest(User::practice().rziCode, year,
		[&](const std::stack<std::string>& hashes) {

			if (hashes.empty()) {
				ModalDialogBuilder::showMessage(
					"Не са открити отчети за импортиране в базата данни"
				);

				abort();

				return;
			}



			m_hashes = hashes;

			view->setProgresSize(hashes.size());

			processHash();
		}
	);

	if (begin) {
		view->clearConsole();
		view->disableButton(true);
	}
}

void PisImportPresenter::processHash()
{
	if (m_hashes.empty()) {
		view->disableButton(false);
		view->logToConsole("Импортирането приключи!");
		return;
	}

	view->logToConsole("Изтегляне на амбулаторен отчет...");

	bool querySent = get_file_serv.sendRequest(m_hashes.top(), User::practice().rziCode,
		[&](const std::string& file) {

			if (file.empty()) {
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
		//	view->logToConsole("Листът вече съществува!");
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

