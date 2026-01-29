#include "Updater.h"
#include "Database/Database.h"
#include "Updater.h"
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"
#include "src/Version.h"
#include "Resources.h"
#include "Database/DbBackend.h"

void DbUpdates::commonUpdate(int toVersion, DbBackend* backend_ptr)
{
	auto db_version = Db::version(backend_ptr);

	if (db_version != toVersion - 1) return;

	for (auto& query : Resources::getMigrationScript(toVersion))
	{
		backend_ptr->execute(query);
	}
}

void DbUpdater::updateDb(DbBackend* backend_ptr)
{
	UpdateDialog d;

	d.show();

	DbUpdates::update10(d, backend_ptr);
	DbUpdates::update11(d, backend_ptr);
	DbUpdates::commonUpdate(12, backend_ptr);
	DbUpdates::commonUpdate(13, backend_ptr);
	DbUpdates::commonUpdate(14, backend_ptr);
	DbUpdates::commonUpdate(15, backend_ptr);
	DbUpdates::update16(backend_ptr);
	DbUpdates::update17(backend_ptr);
	DbUpdates::update18(backend_ptr);
	DbUpdates::update19(d, backend_ptr);
	DbUpdates::commonUpdate(20, backend_ptr);
	DbUpdates::commonUpdate(21, backend_ptr);
	DbUpdates::commonUpdate(22, backend_ptr);
	DbUpdates::commonUpdate(23, backend_ptr);
	DbUpdates::commonUpdate(24, backend_ptr);
	DbUpdates::commonUpdate(25, backend_ptr);
	DbUpdates::update26(d, backend_ptr);
	DbUpdates::commonUpdate(27, backend_ptr);
    DbUpdates::commonUpdate(28, backend_ptr);
	DbUpdates::commonUpdate(29, backend_ptr);
	DbUpdates::commonUpdate(30, backend_ptr);
	DbUpdates::commonUpdate(31, backend_ptr);
	DbUpdates::commonUpdate(32, backend_ptr);
	DbUpdates::commonUpdate(33, backend_ptr);
    DbUpdates::commonUpdate(34, backend_ptr);
	DbUpdates::commonUpdate(35, backend_ptr);
	DbUpdates::update36(d, backend_ptr);
    DbUpdates::commonUpdate(37, backend_ptr);
	DbUpdates::commonUpdate(38, backend_ptr);
}
