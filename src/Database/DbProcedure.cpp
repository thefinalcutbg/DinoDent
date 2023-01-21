#include "DbProcedure.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/Parser.h"
#include "Model/User.h"
#include "Database.h"

#include <QDebug>

std::vector<Procedure> DbProcedure::getProcedures(long long amblist_rowid, Db& db, bool nhifOnly)
{
	std::vector<Procedure> mList;

	const char* condition = nhifOnly ? " AND procedure.nzok = 1 " : "" ;

	std::string query =  "SELECT procedure.nzok, "		//0
							    "procedure.type, "		//1
								"procedure.code, "		//2
								"procedure.tooth, "		//3
								"procedure.date, "		//4
								"procedure.data, "		//5
								"procedure.deciduous, "	//6
								"amblist.LPK, "			//7
								"procedure.ksmp, "		//8
								"procedure.name, "		//9
								"procedure.diagnosis, " //10
								"procedure.notes, "		//11
								"procedure.hyperdontic "//12
						"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
						"WHERE amblist.rowid = " + std::to_string(amblist_rowid) +
						+ condition +
						" ORDER BY procedure.rowid";

	db.newStatement(query);

	while(db.hasRows())
	{
		mList.emplace_back();
		Procedure& p = mList.back();

		p.financingSource = static_cast<Procedure::FinancingSource>(db.asInt(0));
		p.type = static_cast<ProcedureType>(db.asInt(1));
		p.code = db.asInt(2);
		p.tooth = db.asInt(3);
		p.date = Date{ db.asString(4) };
//		p.price = db.asDouble(5);
		Parser::parse(db.asString(5), p);
		p.temp = db.asInt(6);
		p.LPK = db.asString(7);
		p.ksmp = db.asString(8);
		p.name = db.asString(9);
		p.diagnosis = db.asString(10);
		p.notes = db.asString(11);
		p.hyperdontic = db.asBool(12);
	}

	return mList;

}

void DbProcedure::saveProcedures(long long amblist_rowid, const std::vector<Procedure>& mList, Db& db)
{
	std::string query = "DELETE FROM procedure WHERE amblist_rowid = " + std::to_string(amblist_rowid);

	db.execute(query);

	for (int i = 0; i < mList.size(); i++)
	{
		auto& p = mList[i];

		db.newStatement(
			"INSERT INTO procedure "
			"(date, nzok, type, code, tooth, deciduous, data, ksmp, amblist_rowid, name, diagnosis, notes, hyperdontic) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		db.bind(1, p.date.to8601());
		db.bind(2, p.financingSource);
		db.bind(3, static_cast<int>(p.type));
		db.bind(4, p.code);
		db.bind(5, p.tooth);
		db.bind(6, p.temp);
		//    db.bind(7, p.price);
		db.bind(7, Parser::write(p));
		db.bind(8, p.ksmp);
		db.bind(9, amblist_rowid);
		db.bind(10, p.name);
		db.bind(11, p.diagnosis);
		db.bind(12, p.notes);
		db.bind(13, p.hyperdontic);
		db.execute();
		
	}

}

std::vector<ProcedureSummary> DbProcedure::getNhifSummary(long long patientRowId, long long excludeAmbRowId, const Date& from, const Date& to)
{
	std::string query
	{
		"SELECT procedure.date, procedure.code, procedure.tooth, procedure.deciduous, procedure.type, procedure.hyperdontic "
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
				static_cast<ProcedureType>(db.asInt(4)) == ProcedureType::extraction,
				db.asBool(5)
			 });
	 }

	 return summary;

}

std::vector<Procedure> DbProcedure::getToothProcedures(long long patientRowId, int tooth)
{
		std::string query =
		"SELECT  procedure.date, procedure.code, procedure.nzok, procedure.data, amblist.lpk, procedure.deciduous, procedure.ksmp, procedure.name, procedure.diagnosis, procedure.notes, procedure.hyperdontic FROM "
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
		p.financingSource = static_cast<Procedure::FinancingSource>(db.asInt(2));

		Parser::parse(db.asString(3), p);

//		p.price = db.asDouble(4);
		p.LPK = db.asString(4);
		p.temp = db.asInt(5);
		//p.result = NoData{};
		p.tooth = tooth;
		p.ksmp = db.asString(6);
		p.name = db.asString(7);
		p.diagnosis = db.asString(8);
		p.notes = db.asString(9);
		p.hyperdontic = db.asBool(10);
	}
	
	
	return procedures;

}
