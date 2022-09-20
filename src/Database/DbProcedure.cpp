#include "DbProcedure.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/Parser.h"
#include "Model/User.h"
#include "Database.h"

#include <QDebug>

std::vector<Procedure> DbProcedure::getProcedures(long long amblist_rowid, Db* existingConnection, bool nhifOnly)
{
	std::vector<Procedure> mList;

	const char* condition = nhifOnly ? " AND procedure.nzok = 1 " : "" ;

	std::string query =  "SELECT procedure.nzok, "		//0
							    "procedure.type, "		//1
								"procedure.code, "		//2
								"procedure.tooth, "		//3
								"procedure.date, "		//4
								"procedure.price, "		//5
								"procedure.data, "		//6
								"procedure.deciduous, "	//7
								"amblist.LPK, "			//8
								"procedure.ksmp "		//9
						"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
						"WHERE amblist.rowid = " + std::to_string(amblist_rowid) +
						+ condition +
						" ORDER BY procedure.rowid";


	for (Db db(query, existingConnection); db.hasRows();)
	{
		mList.emplace_back();
		Procedure& p = mList.back();

		p.nhif = db.asInt(0);
		p.type = static_cast<ProcedureType>(db.asInt(1));
		p.code = db.asInt(2);
		p.tooth = db.asInt(3);
		p.date = db.asString(4);
		p.price = db.asDouble(5);
		Parser::parse(db.asString(6), p);
		p.temp = db.asInt(7);
		p.LPK = db.asString(8);
		p.ksmp = db.asString(9);
	}

	return mList;

}

void DbProcedure::saveProcedures(long long amblist_rowid, const std::vector<Procedure>& mList, Db* existingConnection)
{

	std::string query = "DELETE FROM procedure WHERE amblist_rowid = " + std::to_string(amblist_rowid);

	Db db(existingConnection);

	db.execute(query);

	for (int i = 0; i < mList.size(); i++)
	{
		auto& m = mList[i];

		query = "INSERT INTO procedure (nzok, type, code, date, tooth, deciduous, price, data, ksmp, amblist_rowid) VALUES ("
			+ std::to_string(m.nhif) + ","
			+ std::to_string(static_cast<int>(m.type)) + ",'"
			+ std::to_string(m.code) + "','"
			+ m.date.to8601() + "',"
			+ std::to_string(m.tooth) + ","
			+ std::to_string(m.temp) + ",'"
			+ std::to_string(m.price) + "','"
			+ Parser::write(m) + "','"
			+ m.ksmp + "',"
			+ std::to_string(amblist_rowid) + ")"
			;

		db.execute(query);
	}

}

std::vector<ProcedureSummary> DbProcedure::getNhifSummary(long long patientRowId, long long excludeAmbRowId, const Date& from, const Date& to)
{
	std::string query
	{
		"SELECT procedure.date, procedure.code, procedure.tooth, procedure.deciduous, procedure.type "
		"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE nzok = 1 "
		"AND amblist.patient_rowid = " + std::to_string(patientRowId) + " "
		"AND amblist.rowid != " + std::to_string(excludeAmbRowId) + " "
		"AND amblist.date BETWEEN  ('" + from.to8601() + "') AND ('" + to.to8601() + "')"
	};

	 std::vector<ProcedureSummary> summary;

	 for (Db db(query); db.hasRows();)
	 {
		 summary.push_back(
			 {
				Date{ db.asString(0) },
				db.asInt(1),
				db.asInt(2),
				db.asBool(3),
				static_cast<ProcedureType>(db.asInt(4)) == ProcedureType::extraction
			 });
	 }

	 return summary;

}

std::vector<Procedure> DbProcedure::getToothProcedures(long long patientRowId, int tooth)
{
		std::string query =
		"SELECT  procedure.date, procedure.code, procedure.nzok, procedure.data, procedure.price, amblist.lpk, procedure.deciduous, procedure.ksmp FROM "
		"procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE tooth = " + std::to_string(tooth) + " "
		"AND patient_rowid = " + std::to_string(patientRowId) + " "
		"ORDER BY procedure.date ASC, procedure.code ASC, procedure.rowid ASC";

	std::vector<Procedure> procedures;
	
	for (Db db(query); db.hasRows();)
	{

		procedures.emplace_back();
		auto& p = procedures.back();
		
		p.date = Date{ db.asString(0) };

		p.code = db.asInt(1);
		p.nhif = db.asInt(2);

		Parser::parse(db.asString(3), p);

		p.price = db.asDouble(4);
		p.LPK = db.asString(5);
		p.temp = db.asInt(6);
		//p.result = NoData{};
		p.tooth = tooth;
		p.ksmp = db.asString(7);
	}
	
	
	return procedures;

}
