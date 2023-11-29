#include "DbUpdates.h"

#include <JsonCpp/json.h>
#include <unordered_map>
#include <vector>

#include "Database/Database.h"
#include "Resources.h"

#include "View/Widgets/UpdateDialog.h"

void DbUpdates::update19(UpdateDialog& progressDialog)
{
	if (Db::version() != 18) return;

	Json::Value ekatteJson;

	Json::Reader().parse(Resources::citiesJson(), ekatteJson);

	std::unordered_map<int, int> ekatte_map;

	for (auto& val : ekatteJson)
	{
		if (!val.isMember("wrong_ekatte")) continue;

		ekatte_map[val["wrong_ekatte"].asInt()] = val["ekatte"].asInt();
	}

	std::vector<std::pair<long long, int>> rowid_ekatte;

	Db db("SELECT rowid, ekatte FROM patient");

	while (db.hasRows()) {

		long long rowid = db.asRowId(0);
		int ekatte = db.asInt(1);

		if (!ekatte_map.count(ekatte)) continue;

		rowid_ekatte.push_back(std::make_pair(rowid, ekatte_map[ekatte]));
	}

	progressDialog.setRange(rowid_ekatte.size());

	for (auto& pair : rowid_ekatte)
	{
		db.newStatement("UPDATE patient SET ekatte=? WHERE rowid=?");
		db.bind(1, pair.second);
		db.bind(2, pair.first);
		db.execute();

		progressDialog.increment();
	}

	rowid_ekatte.clear();

	db.newStatement("SELECT rowid, practice_address FROM practice");

	while (db.hasRows()) {

		long long rowid = db.asRowId(0);
		int ekatte = db.asInt(1);

		if (!ekatte_map.count(ekatte)) continue;

		rowid_ekatte.push_back(std::make_pair(rowid, ekatte_map[ekatte]));
	}

	progressDialog.setRange(rowid_ekatte.size());

	for (auto& pair : rowid_ekatte)
	{
		db.newStatement("UPDATE practice SET practice_address=? WHERE rowid=?");
		db.bind(1, pair.second);
		db.bind(2, pair.first);
		db.execute();

		progressDialog.increment();
	}

	db.setVersion(19);

}