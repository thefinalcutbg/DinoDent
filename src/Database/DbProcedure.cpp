#include "DbProcedure.h"

#include "Model/Procedure/MasterNZOK.h"
#include "Model/Parser/Parser.h"
#include "Model/User/UserManager.h"
#include "Database.h"

#include <QDebug>

std::vector<Procedure> DbProcedure::getProcedures(long long amblist_rowid, Db* existingConnection)
{
	std::vector<Procedure> mList;

	

	std::string query =  "SELECT procedure.nzok, "		//0
							    "procedure.type, "		//1
								"procedure.code, "		//2
								"procedure.tooth, "		//3
								"procedure.day, "		//4
								"amblist.month, "		//5
								"amblist.year, "		//6
								"procedure.price, "		//7
								"procedure.data, "		//8
								"procedure.deciduous, "	//9
								"amblist.LPK "			//10
						"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
						"WHERE amblist.rowid = " + std::to_string(amblist_rowid) + " ORDER BY procedure.rowid";

	
	for (Db db(query, existingConnection); db.hasRows();)
	{
		mList.emplace_back();
		Procedure& p = mList.back();

		p.nzok = db.asInt(0);
		p.type = static_cast<ProcedureType>(db.asInt(1));
		p.code = db.asInt(2);
		p.tooth = db.asInt(3);
		p.date.day = db.asInt(4);
		p.date.month = db.asInt(5);
		p.date.year = db.asInt(6);
		p.price = db.asDouble(7);
		Parser::parse(db.asString(8), p);
		p.temp = db.asInt(9);
		p.LPK = db.asString(10);
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

		query = "INSERT INTO procedure (nzok, type, code, day, tooth, deciduous, price, data, amblist_rowid) VALUES ("
			+ std::to_string(m.nzok) + ","
			+ std::to_string(static_cast<int>(m.type)) + ",'"
			+ std::to_string(m.code) + "',"
			+ std::to_string(m.date.day) + ","
			+ std::to_string(m.tooth) + ","
			+ std::to_string(m.temp) + ",'"
			+ std::to_string(m.price) + "','"
			+ Parser::write(m) + "',"
			+ std::to_string(amblist_rowid) + ")"
			;

		db.execute(query);
	}

}

std::vector<ProcedureSummary> DbProcedure::getNhifSummary(long long patientRowId, long long excludeAmbRowId, const Date& from, const Date& to)
{
	std::string query
	{
		"SELECT procedure.day, amblist.month, amblist.year, procedure.code, procedure.tooth,  procedure.deciduous, procedure.type "
		"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE nzok = 1 "
		"AND amblist.patient_rowid = " + std::to_string(patientRowId) + " "
		"AND amblist.rowid != " + std::to_string(excludeAmbRowId) + " "

		"AND(amblist.year, amblist.month, amblist.day) "
		" BETWEEN  ("
		+ std::to_string(from.year) + ", "
		+ std::to_string(from.month) + ", "
		+ std::to_string(from.day) + ") "
		"AND ("
		+ std::to_string(to.year) + ", "
		+ std::to_string(to.month) + ", "
		+ std::to_string(to.day) + ")"
	};

	 std::vector<ProcedureSummary> summary;

	 for (Db db(query); db.hasRows();)
	 {
		 summary.push_back(
			 {
				Date
				{
					 db.asInt(0),
					 db.asInt(1),
					 db.asInt(2)
				},
				db.asInt(3),
				db.asInt(4),
				db.asBool(5),
				static_cast<ProcedureType>(db.asInt(6)) == ProcedureType::extraction
			 });
	 }

	 return summary;

}

std::vector<Procedure> DbProcedure::getToothProcedures(long long patientRowId, int tooth)
{
		std::string query =
		"SELECT  procedure.day, amblist.month, amblist.year, procedure.code, procedure.nzok, procedure.data, procedure.price, amblist.lpk, procedure.deciduous FROM "
		"procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE tooth = " + std::to_string(tooth) + " "
		"AND patient_rowid = " + std::to_string(patientRowId) + " "
		"ORDER BY amblist.year ASC, amblist.month ASC, procedure.code ASC, procedure.rowid ASC";

	std::vector<Procedure> procedures;
	
	for (Db db(query); db.hasRows();)
	{

		procedures.emplace_back();
		auto& p = procedures.back();
		
		p.date = Date{ 
			db.asInt(0),
			db.asInt(1),
			db.asInt(2)
		};

		p.code = db.asInt(3);
		p.nzok = db.asInt(4);

		Parser::parse(db.asString(5), p);

		p.price = db.asDouble(6);
		p.LPK = db.asString(7);
		p.temp = db.asInt(8);
		//p.result = NoData{};
		p.tooth = tooth;
	}
	
	
	return procedures;

}
