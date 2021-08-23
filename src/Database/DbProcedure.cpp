#include "DbProcedure.h"
#include <QDebug>
#include "Model/Procedure/MasterNZOK.h"
#include <QDebug>

std::vector<Procedure> DbProcedure::getProcedures(const std::string& amblist_id, const Date& amb_date)
{
	std::vector<Procedure> mList;

	openConnection();

	std::string query = "SELECT nzok, type, code, tooth, day, price, data, temp FROM "
		"procedure WHERE amblist_id = " + amblist_id + " ORDER BY seq";

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		mList.emplace_back();
		Procedure& p = mList.back();
		
		p.date = amb_date;
		p.nzok = sqlite3_column_int(stmt, 0);
		p.code = sqlite3_column_int(stmt, 2);
		p.type = static_cast<ProcedureType>(sqlite3_column_int(stmt, 1));
		p.price = sqlite3_column_double(stmt, 5);
		p.tooth = sqlite3_column_int(stmt, 3);
		p.date.day = sqlite3_column_int(stmt, 4);

		procedure_parser.parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6))), p);
		p.temp = sqlite3_column_int(stmt, 7);
	}

	sqlite3_finalize(stmt);

	closeConnection();

	return mList;

}

void DbProcedure::saveProcedures(const std::string& amblist_id, const std::vector<Procedure>& mList)
{

	openConnection();

	std::string query = "DELETE FROM procedure WHERE amblist_id = " + amblist_id;

	rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
		

	for (int i = 0; i < mList.size(); i++)
	{
		auto& m = mList[i];

		query = "INSERT INTO procedure (seq, nzok, type, code, day, tooth, temp, price, data, amblist_id) VALUES ('"
			+ std::to_string(i) + "','"
			+ std::to_string(m.nzok) + "','"
			+ std::to_string(static_cast<int>(m.type)) + "','"
			+ std::to_string(m.code) + "','"
			+ std::to_string(m.date.day) + "','"
			+ std::to_string(m.tooth) + "','"
			+ std::to_string(m.temp) + "','"
			+ std::to_string(m.price) + "','"
			+ procedure_parser.write(m) + "','"
			+ amblist_id + "')"
			;

			rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
			
			if (rc != SQLITE_OK)
			{
				qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
			}
	}
	
	closeConnection();

}

std::vector<ProcedureSummary> DbProcedure::getSummary(const std::string& patientID, const std::string& excludeAmbID)
{
	openConnection();

	std::string query
	{
		"SELECT procedure.day, amblist.month, amblist.year, procedure.code, procedure.tooth "
		"FROM procedure LEFT JOIN amblist ON procedure.amblist_id = amblist.id "
		"WHERE nzok = 1 "
		"AND amblist.patient_id = '" + patientID + "' "
		"AND amblist.id != " + excludeAmbID
	};

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	 std::vector<ProcedureSummary> summary;

	 while (sqlite3_step(stmt) != SQLITE_DONE)
	 {
		 summary.push_back(
			 {
				Date
				{
					 sqlite3_column_int(stmt, 0),
					 sqlite3_column_int(stmt, 1),
					 sqlite3_column_int(stmt, 2)
				},
				 sqlite3_column_int(stmt, 3),
				 sqlite3_column_int(stmt, 4)
				 
			 });
	 }

	 sqlite3_finalize(stmt);

	 closeConnection();

	 return summary;

}

std::vector<Procedure> DbProcedure::getToothProcedures(const std::string& patientID, int tooth)
{
	openConnection();
	
	std::string query =
		"SELECT  procedure.day, amblist.month, amblist.year, procedure.code, procedure.nzok, procedure.data, procedure.price, amblist.lpk, procedure.temp FROM "
		"procedure LEFT JOIN amblist ON procedure.amblist_id = amblist.id "
		"WHERE tooth = " + std::to_string(tooth) + " "
		"AND patient_id = '" + patientID + "' "
		"ORDER BY amblist.year ASC, amblist.month ASC, procedure.code ASC, procedure.seq ASC";
		
	qDebug() << QString::fromStdString(query);

	sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

	std::vector<Procedure> procedures;
	
	while (sqlite3_step(stmt) != SQLITE_DONE)
	{

		procedures.emplace_back();
		auto& p = procedures.back();
		
		p.date = Date{ 
			sqlite3_column_int(stmt, 0), 
			sqlite3_column_int(stmt, 1),
			sqlite3_column_int(stmt, 2)
		};

		p.code = sqlite3_column_int(stmt, 3);
		p.nzok = sqlite3_column_int(stmt, 4);

		procedure_parser.parse(
			std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) }, p
		);
		p.price = sqlite3_column_double(stmt, 6);
		p.LPK = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
		p.temp = sqlite3_column_int(stmt, 8);
		p.result = NoData{};
		p.tooth = tooth;
	}
	
	sqlite3_finalize(stmt);

	closeConnection();

	return procedures;

}
