#include "PracticeManagerPresenter.h"

#include "Database/DbPractice.h"
#include "Database/Database.h"
#include "Database/DbDoctor.h"
#include "DbUpdates/Updater.h"

#include "View/ModalDialogBuilder.h"
#include "View/Widgets/PracticeSelectorView.h"

#include "View/Widgets/PracticeDialog.h"
#include "GlobalSettings.h"

#include "Presenter/DoctorDialogPresenter.h"

PracticeManagerPresenter::PracticeManagerPresenter() :
	practices{DbPractice::getPracticeList()}
{
}

void PracticeManagerPresenter::setView(PracticeSelectorView* view)
{
	this->view = view;
	
	std::vector<std::string> practiceNames;

	for (auto& p : practices) { practiceNames.push_back(p.name); }

	view->setPracticeList(practiceNames);
	view->setDbPath(GlobalSettings::getDbPath());
}

void PracticeManagerPresenter::addClicked()
{

    PracticeDialog practiceDialog;
	practiceDialog.exec();

    auto p_result = practiceDialog.getData();

    if(!p_result.has_value()) return;

    if(DbPractice::practiceExists(p_result->rzi)){
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
    setView(view);
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

	setView(view);


}

void PracticeManagerPresenter::dbChangePath()
{
	auto pathResult = GlobalSettings::setDbPath();

	if (pathResult.size()) {
		Db::setFilePath(pathResult);
	}

	Db::createIfNotExist();

	DbUpdater::updateDb();

	practices = DbPractice::getPracticeList();

	setView(view);
}


