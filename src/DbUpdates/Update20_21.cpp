#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"

void DbUpdates::update20()
{
	if (Db::version() != 19) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(20))
	{
		db.execute(query);
	}
}

void DbUpdates::update21()
{
	if (Db::version() != 20) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(21))
	{
		db.execute(query);
	}
}