#include "DbManipulation.h"
#include <QDebug>

std::vector<Manipulation> DbManipulation::getManipulations(const std::string& amblist_id, const Date& amb_date)
{
	std::vector<Manipulation> mList;

	openConnection();

	std::string query = "SELECT type, code, day, tooth, price, data FROM manipulations "
						"WHERE amblist_id = " +amblist_id + " ORDER BY seq ASC";

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE)
	{

		Manipulation m;
		m.date = amb_date;
		m.type = static_cast<ManipulationType>(sqlite3_column_int(stmt, 0));
		m.code = std::stoi(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))),
		m.date.day = sqlite3_column_int(stmt, 2);
		m.tooth = sqlite3_column_int(stmt, 3);
		m.price = sqlite3_column_double(stmt, 4);
		parser.parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))), m);
		m.nzok = false;
		mList.push_back(m);
	}

	sqlite3_finalize(stmt);

	closeConnection();

	return mList;

}

void DbManipulation::saveManipulations(const std::string& amblist_id, const std::vector<Manipulation>& mList)
{

	openConnection();

	std::string query = "DELETE FROM manipulations WHERE amblist_id = " + amblist_id;

	rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
		
	query = "DELETE FROM nzok WHERE amblist_id = " + amblist_id;

	rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

	for (int i = 0; i < mList.size(); i++)
	{
		auto& m = mList[i];

		if (!m.nzok)
		{
			query = "INSERT INTO manipulations (seq, type, code, day, tooth, price, data, amblist_id) VALUES ('"
				+ std::to_string(i) + "','"
				+ std::to_string(static_cast<int>(m.type)) + "','"
				+ std::to_string(m.code) + "','"
				+ std::to_string(m.date.day) + "','"
				+ std::to_string(m.tooth) + "','"
				+ std::to_string(m.price) + "','"
				+ parser.write(m) + "','"
				+ amblist_id + "')"
				;

			rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
			qDebug() << QString::fromStdString(query);
			if (rc != SQLITE_OK)
			{
				qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
			}
		}
	}
	
	closeConnection();

			

}
