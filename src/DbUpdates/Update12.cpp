#include "Database/Database.h"
#include "DbUpdates.h"
#include "Resources.h"

void DbUpdates::update12()
{
	if (Db::version() != 11) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(12))
	{
		db.execute(query);
	}
}