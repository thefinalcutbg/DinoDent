#include "Database/Database.h"
#include "DbUpdates.h"
#include "Resources.h"
#include <qdebug.h>

void DbUpdates::update12()
{
	if (Db::version() != 11) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(12))
	{
		db.execute(query);
	}
}

void DbUpdates::update13()
{
	if (Db::version() != 12) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(13))
	{
		db.execute(query);
	}
}

void DbUpdates::update14()
{
	if (Db::version() != 13) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(14))
	{
		db.execute(query);
	}
}

void DbUpdates::update15()
{
	if (Db::version() != 14) return;
	Db db;

	for (auto& query : Resources::getMigrationScript(15))
	{
		db.execute(query);
	}
}