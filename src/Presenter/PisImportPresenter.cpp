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

void PisImportPresenter::importDataPressed()
{
	
	view->clearConsole();
	view->disableButton(true);

	get_hash_serv.sendRequest(User::practice().rziCode,
		[&](const std::stack<std::string>& hashes) {

			if (hashes.empty()) {
				ModalDialogBuilder::showMessage(
					"Не са открити отчети за импортиране в базата данни"
				);
				view->disableButton(false);
				return;
			}

			m_hashes = hashes;

			view->setProgresSize(m_hashes.size());

			processHash();
		}
	);
}

void PisImportPresenter::processHash()
{
	if (m_hashes.empty()) {
		view->disableButton(false);
		view->logToConsole("Импортирането приключи!");
		return;
	}

	view->logToConsole("Изтегляне на амбулаторен отчет...");

	get_file_serv.sendRequest(m_hashes.top(), User::practice().rziCode,
		[&](const std::string& file) {

			if (file.empty()) {
				abort();
				return;
			}

			importToDb(file);
			m_hashes.pop();
			processHash();
		}
	);
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

		pair.second.patient_rowid = patient.rowid;

		std::string logMsg = "Импортиране на амбулаторен лист ";
		logMsg += pair.second.getNumber();
		view->logToConsole(logMsg);

		if (DbAmbList::importedPisSheetExists(pair.second, patient)) {
			view->logToConsole("Листът вече съществува!");
			continue;
		}

		DbAmbList::insert(pair.second, pair.second.patient_rowid);
	}
}

void PisImportPresenter::abort()
{
	view->setProgresSize(1);
	view->disableButton(false);
}

