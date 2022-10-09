#include "DbUpdateStatus.h"
#include "Database.h"
#include "Model/Date.h"
#include <qdebug.h>

std::string numRowNames[1]{"med"};

void DbUpdateStatus::setAsUpdated(DynamicNum num)
{

	auto dateValue = Date::currentDate().to8601();

	Db db;

	std::string query = "INSERT INTO num_update (name,date) VALUES(?,?)"
		" ON CONFLICT(name) DO UPDATE SET date=? WHERE name=?";

	db.newStatement(query);

	db.bind(1, numRowNames[static_cast<int>(num)]);
	db.bind(2, dateValue);
	db.bind(3, dateValue);
	db.bind(4, numRowNames[static_cast<int>(num)]);
	
	db.execute();

}

Date DbUpdateStatus::lastUpdated(DynamicNum num)
{
	Date result;

	Db db;
	db.newStatement("SELECT date FROM num_update WHERE name=?");

	db.bind(1, numRowNames[static_cast<int>(num)]);

	while (db.hasRows()) {
		result = db.asString(0);
	}

	return result;

}
