#include "DbProcedure.h"
#include <QDebug>
#include "Model/Procedure/MasterNZOK.h"

std::vector<Procedure> DbProcedure::getManipulations(const std::string& amblist_id, const Date& amb_date)
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

void DbProcedure::saveManipulations(const std::string& amblist_id, const std::vector<Procedure>& mList)
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

bool DbProcedure::procedureExists(int tooth, int procedure, const std::string& patient_id, 
									const Date& date, int year_range, const std::string& excludeAmb_id)
{
	openConnection();

	std::string query = "SELECT COUNT(*) FROM "
		"nzok INNER JOIN amblist ON nzok.amblist_id = amblist.id "
		"WHERE nzok.tooth = " + std::to_string(tooth) + " "
		"AND amblist.patient_id = '" + patient_id + "' "
		"AND amblist.id != '" + excludeAmb_id + "' "
		"AND(amblist.year, amblist.month, nzok.day) "
		"BETWEEN ("
			+ std::to_string(date.year - year_range) + ", "
			+ std::to_string(date.month) + ", "
			+ std::to_string(date.day) + ") "
		"AND ("
		+ std::to_string(date.year + year_range) + ", "
		+ std::to_string(date.month) + ", "
		+ std::to_string(date.day) + ")";

	bool exists = false;

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		exists = sqlite3_column_int(stmt, 0);
	}

	closeConnection();

	return exists;

}

std::unordered_map<int, int> DbProcedure::totalNZOKProcedures(const std::string& patientID, const std::string& excludeAmbId, int ambList_year)
{

	std::unordered_map<int, int> procedure_count;

	openConnection();

	std::string query =
		"SELECT nzok.code, COUNT(*) "
		"FROM nzok INNER JOIN amblist ON nzok.amblist_id = amblist.id "
		"WHERE amblist.patient_id = '" + patientID + "' " +
		"AND amblist.year = " + std::to_string(ambList_year) + " "
		"AND amblist.id != '" + excludeAmbId + "' "
		"GROUP BY nzok.code";

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		procedure_count[sqlite3_column_int(stmt, 0)] = sqlite3_column_int(stmt, 1);
	}

	closeConnection();

	return procedure_count;
}

bool DbProcedure::procedureExists(int tooth, int procedure_code, const std::string& patient_id, const std::string& excludeAmbId)
{
	bool exists = false;

	openConnection();

	std::string query
	{
		"SELECT count(*) FROM nzok INNER JOIN amblist "
		"ON nzok.amblist_id = amblist.id "
		"WHERE amblist.patient_id = '" + patient_id + "' "
		"AND amblist.id != " + excludeAmbId + " "
		"AND nzok.code = '" + std::to_string(procedure_code) + "' "
		"AND nzok.tooth = " + std::to_string(tooth)
	};

	qDebug() << QString::fromStdString(query);

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE)
		exists = sqlite3_column_int(stmt, 0);


	closeConnection();

	return exists;

}
