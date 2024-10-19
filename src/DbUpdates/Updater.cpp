#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Version.h"

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
	DbUpdates::update12();
	DbUpdates::update13();
	DbUpdates::update14();
	DbUpdates::update15();
	DbUpdates::update16();
	DbUpdates::update17();
	DbUpdates::update18();
	DbUpdates::update19(d);
	DbUpdates::update20();
	DbUpdates::update21();
	DbUpdates::update22();
	DbUpdates::update23();
	DbUpdates::update24();
	DbUpdates::update25();
}
