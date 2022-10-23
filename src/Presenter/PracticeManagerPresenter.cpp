#include "PracticeManagerPresenter.h"
#include "Database/DbPractice.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/PracticeDialogPresenter.h"
#include "Path.h"
#include "Database/Database.h"
#include "Updates/Updater.h"

PracticeManagerPresenter::PracticeManagerPresenter() :
	practices{DbPractice::getPracticeList()}
{
}

void PracticeManagerPresenter::setView(IPracticeSelectorView* view)
{
	this->view = view;
	
	std::vector<std::string> practiceNames;

	for (auto& p : practices) { practiceNames.push_back(p.name); }

	view->setPracticeList(practiceNames);
	view->setDbPath(Path::getDbPath());
}

void PracticeManagerPresenter::addClicked()
{
	view->closeDialog();

	PracticeDialogPresenter p;

	auto result = p.open();

	if (result.has_value()) {
		DbPractice::insertPractice(result->practice);
		DbPractice::setDoctorsPracticeList(result->doctorsList, result->practice.rziCode);
	}

	practices = DbPractice::getPracticeList();

	ModalDialogBuilder::openDialog(this);
	
}

void PracticeManagerPresenter::removeClicked(int idx)
{
	if (idx < 0 && idx >= practices.size()) return;

	if (
		!ModalDialogBuilder::askDialog(
			u8"Сигурни ли сте, че искате да премахнете тази практика?"
		)
	) return;

	auto passGuess = ModalDialogBuilder::inputDialog(
		u8"Въведете парола на практиката",
		u8"Премахване на практика",
		true
	);


	if(passGuess.empty()) return;

	if (passGuess != practices[idx].pass)
	{
		ModalDialogBuilder::showError(u8"Грешна парола!");
		return;
	}

	DbPractice::deletePractice(practices[idx].rzi);

	practices = DbPractice::getPracticeList();

	setView(view);


}

void PracticeManagerPresenter::editClicked(int idx)
{
	if (idx < 0 && idx >= practices.size()) return;

	auto passGuess = ModalDialogBuilder::inputDialog(
		u8"Въведете парола на практиката",
		u8"Редактиране на практика",
		true
	);

	if (passGuess.empty()) return;

	if (passGuess != practices[idx].pass)
	{
		ModalDialogBuilder::showError(u8"Грешна парола!");
		return;
	}

	view->closeDialog();

	PracticeDialogPresenter p(practices[idx].rzi);

	auto result = p.open();

	if (result.has_value()) {
		DbPractice::updatePractice(result->practice, practices[idx].rzi);
		DbPractice::setDoctorsPracticeList(result->doctorsList, result->practice.rziCode);
	}

	practices = DbPractice::getPracticeList();

	ModalDialogBuilder::openDialog(this);

}

void PracticeManagerPresenter::dbChangePath()
{
	auto pathResult = Path::setDbPath();

	if (pathResult.empty()) return;

	Db::setFilePath(pathResult);

	Db::createIfNotExist();

	DbUpdater::updateDb();

	practices = DbPractice::getPracticeList();

	setView(view);
}


