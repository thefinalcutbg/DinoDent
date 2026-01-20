#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"
#include "src/Version.h"
#include "Resources.h"

void commonUpdate(int toVersion) {

	if (Db::version() != toVersion-1) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(toVersion))
	{
		db.execute(query);
	}

}

void DbUpdater::updateDb()
{
	if (Db::version() == Version::dbVersion()) return;

	if (Db::version() > Version::dbVersion()) {

		ModalDialogBuilder::showMessage(
			"Версията на базата данни е по-нова от тази, която се поддържа от програмата. "
			"Задължително актуализирайте софтуера до най-последна версия, преди да го използвате!"
		);

		return;
	}

	DbUpdates::backupDatabase();
	
	UpdateDialog d;

	d.show();

	DbUpdates::update10(d);
	DbUpdates::update11(d);
	commonUpdate(12);
	commonUpdate(13);
	commonUpdate(14);
	commonUpdate(15);
	DbUpdates::update16();
	DbUpdates::update17();
	DbUpdates::update18();
	DbUpdates::update19(d);
	commonUpdate(20);
	commonUpdate(21);
	commonUpdate(22);
	commonUpdate(23);
	commonUpdate(24);
	commonUpdate(25);
	DbUpdates::update26(d);
	commonUpdate(27);
    commonUpdate(28);
	commonUpdate(29);
	commonUpdate(30);
	commonUpdate(31);
	commonUpdate(32);
	commonUpdate(33);
    commonUpdate(34);
	commonUpdate(35);
	DbUpdates::update36(d);
    commonUpdate(37);
	commonUpdate(38);
}
