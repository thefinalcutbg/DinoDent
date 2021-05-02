#include "DbManipulation.h"

std::vector<Manipulation> DbManipulation::getManipulations(const std::string& amblist_id)
{
	std::vector<Manipulation> mList;

	openConnection();

	

	closeConnection();

	return mList;

}

void DbManipulation::saveManipulations(const std::string& amblist_id, const std::vector<Manipulation>& mList)
{

	openConnection();

	std::string query = "DELETE FROM manipulations WHERE amblist_id = " + amblist_id; //+

	rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
		
	query = "DELETE FROM nzok WHERE amblist_id = " + amblist_id;

	rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

	for (int i = 0; i < mList.size(); i++)
	{
		auto& m = mList[i];

		if (!m.nzok)
		{
			query = "INSERT INTO manipulations (seq, type, day, tooth, price, data, amblist_id) VALUES ('"
				+ std::to_string(i) + "','"
				+ std::to_string(static_cast<int>(m.type)) + "','"
				+ std::to_string(m.date.day) + "','"
				+ std::to_string(m.tooth) + "','"
				+ std::to_string(m.price) + "','"
				+ "null" + "','"  //parser should go here
				+ amblist_id + "')"
				;

			rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

			if (rc != SQLITE_OK)
			{
				qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
			}
		}
	}
	

	closeConnection();

			

}
