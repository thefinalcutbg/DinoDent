#include "DbManipulation.h"
#include <QDebug>
#include "Model/Procedure/MasterNZOK.h"

std::vector<Procedure> DbManipulation::getManipulations(const std::string& amblist_id, const Date& amb_date)
{
	std::vector<Procedure> mList;

	openConnection();

	std::string query = "SELECT nzok, type, code, tooth, day, price, data FROM "
		"(SELECT 0 AS nzok, m.type, m.code, m.seq, m.day, m.tooth, m.price, m.data "
		"FROM manipulations m WHERE m.amblist_id = " + amblist_id + " "
		"UNION "
		"SELECT 1 AS nzok, -1, n.code, n.seq, n.day, n.tooth, -1, n.data "
		"FROM nzok n WHERE n.amblist_id = " + amblist_id + ") "
		"ORDER BY seq";

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	qDebug() << QString::fromStdString(query);

	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		mList.emplace_back(Procedure{});
		Procedure& m = mList.back();
		
		m.date = amb_date;
		m.nzok = sqlite3_column_int(stmt, 0);
		m.code = sqlite3_column_int(stmt, 2);

		if(m.nzok)
		{
			m.type = MasterNZOK::instance().getTemplateByCode(m.code).type;
			m.price = 0;
		}
		else
		{
			m.type = static_cast<ProcedureType>(sqlite3_column_int(stmt, 1));
			m.price = sqlite3_column_double(stmt, 5);
		}
		
		m.tooth = sqlite3_column_int(stmt, 3);
		m.date.day = sqlite3_column_int(stmt, 4);

		procedureParser.parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6))), m);
	}

	sqlite3_finalize(stmt);

	closeConnection();

	return mList;

}

void DbManipulation::saveManipulations(const std::string& amblist_id, const std::vector<Procedure>& mList)
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
				+ procedureParser.write(m) + "','"
				+ amblist_id + "')"
				;

			rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
			
			if (rc != SQLITE_OK)
			{
				qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
			}
		}
		else
		{
			query = "INSERT INTO nzok (seq, code, day, tooth, data, amblist_id) VALUES ('"
				+ std::to_string(i) + "','"
				+ std::to_string(m.code) + "','"
				+ std::to_string(m.date.day) + "','"
				+ std::to_string(m.tooth) + "','"
				+ procedureParser.write(m) + "','"
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
