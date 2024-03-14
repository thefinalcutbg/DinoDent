#include "PracticeManagerPresenter.h"
#include "Database/DbPractice.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/PracticeDialog.h"
#include "GlobalSettings.h"
#include "Database/Database.h"
#include "Database/DbDoctor.h"
#include "DbUpdates/Updater.h"

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
	view->setDbPath(GlobalSettings::getDbPath());
}

void PracticeManagerPresenter::addClicked()
{

    PracticeDialog d;
    d.exec();

    auto result = d.getData();

    if(!result.has_value()) return;

    if(DbPractice::practiceExists(result->rzi)){
        ModalDialogBuilder::showMessage("Практика с такъв РЗИ номер вече съществува");
        return;
    }

    Practice p;
    p.name = result->name;
    p.practice_address = result->address;
    p.firm_address = result->address;
    p.legal_entity = 2;
    p.rziCode = result->rzi;
    p.bulstat = "000000000";

    DbPractice::insertPractice(p);

    if(!DbDoctor::suchDoctorExists(result->lpk)){

        Doctor d;
        d.fname = "Иван";
        d.mname = "Иванов";
        d.lname = "Иванов";
        d.LPK = result->lpk;
        d.hisSpecialty = 2081;
        d.phone = "";
        DbDoctor::insertDoctor(d);
    }

    PracticeDoctor pd;
    pd.lpk = result->lpk;
    pd.admin = true;

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

	if (pathResult.empty()) return;

	Db::setFilePath(pathResult);

	Db::createIfNotExist();

	DbUpdater::updateDb();

	practices = DbPractice::getPracticeList();

	setView(view);
}


