#include "PracticeManagerPresenter.h"

#include "Database/DbPractice.h"
#include "Database/Database.h"
#include "Database/DbDoctor.h"
#include "DbUpdates/Updater.h"

#include "View/ModalDialogBuilder.h"
#include "View/Widgets/PracticeSelectorView.h"
#include "View/Widgets/DbSettingsDialog.h"
#include "View/Widgets/PracticeDialog.h"

#include "GlobalSettings.h"

#include "Presenter/DoctorDialogPresenter.h"
#include <QFileDialog>


PracticeManagerPresenter::PracticeManagerPresenter() :
	practices{ DbPractice::getPracticeList() }
{
}

void PracticeManagerPresenter::setView(PracticeSelectorView* view)
{
	this->view = view;
	
	setPracticeListToView();
	setDbPathToView();

}

void PracticeManagerPresenter::setDbPathToView()
{
	auto db_settings = GlobalSettings::getDbSettings();

	bool dbIsLocal = db_settings.mode == DbSettings::DbType::Sqlite;

	auto& dbPath = dbIsLocal ? db_settings.sqliteFilePath : db_settings.rqliteUrl;

	view->setDbPath(dbPath, dbIsLocal);
}

void PracticeManagerPresenter::setPracticeListToView()
{
	std::vector<std::string> practiceNames;

	for (auto& p : practices) { practiceNames.push_back(p.name); }

	view->setPracticeList(practiceNames);
}


void PracticeManagerPresenter::addClicked()
{

	PracticeDialog practiceDialog;
	practiceDialog.exec();

	auto p_result = practiceDialog.getData();

	if (!p_result.has_value()) return;

	if (DbPractice::practiceExists(p_result->rzi)) {
		ModalDialogBuilder::showMessage("Практика с такъв РЗИ номер вече съществува");
		return;
	}

	ModalDialogBuilder::showMessage("Въведете администратор на практиката");

	DoctorDialogPresenter doctorDialog;

	auto d_result = doctorDialog.open();

	if (!d_result.has_value()) return;

	Practice p;
	p.name = p_result->name;
	p.practice_address = p_result->address;
	p.firm_address = p_result->address;
	p.legal_entity = 2;
	p.rziCode = p_result->rzi;
	p.bulstat = "000000000";

	PracticeDoctor pd;
	pd.lpk = d_result->LPK;
	pd.admin = true;

	DbPractice::insertPractice(p);

	DbPractice::setDoctorsPracticeList({ pd }, p.rziCode);

	practices = DbPractice::getPracticeList();

	setPracticeListToView();
}

void PracticeManagerPresenter::removeClicked(int idx)
{
	if (idx < 0 && idx >= practices.size()) return;

	if (
		!ModalDialogBuilder::askDialog(
			"Сигурни ли сте, че искате да премахнете тази практика?"
		)
	) return;

	if (practices[idx].pass.size())
	{
		auto passGuess = ModalDialogBuilder::inputDialog(
			"Въведете парола на практиката",
			"Редактиране на практика",
			"",
			true
		);

		if (passGuess.empty()) return;

		if (passGuess != practices[idx].pass)
		{
			ModalDialogBuilder::showError("Грешна парола!");
			return;
		}
	}

	DbPractice::deletePractice(practices[idx].rzi);

	practices = DbPractice::getPracticeList();

	setPracticeListToView();


}

void PracticeManagerPresenter::dbChangePath()
{
	DbSettingsDialog d(GlobalSettings::getDbSettings());
	
	auto result = d.getResult();

	if (!result) return;

	Db::setSettings(result.value());

	if(!Db::testConnection()) return;

	GlobalSettings::setDbSettings(result.value());

	practices = DbPractice::getPracticeList();

	setDbPathToView();

	setPracticeListToView();
}

