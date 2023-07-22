#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"

void DbUpdates::update14()
{
	if (Db::version() != 13) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(14))
	{
		db.execute(query);
	}
}
