#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"

void DbUpdates::update11()
{
	if (Db::version() != 10) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(11))
	{
		db.execute(query);
	}

}
