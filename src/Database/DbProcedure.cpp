#include "DbProcedure.h"

#include "Model/Procedure/MasterNZOK.h"
#include "Model/Parser/Parser.h"
#include "Model/User/UserManager.h"
#include "Database.h"

std::vector<Procedure> DbProcedure::getProcedures(const std::string& amblist_id, Db* existingConnection)
{
	std::vector<Procedure> mList;

	

	std::string query = "SELECT	procedure.nzok, "	//0
							    "procedure.type, "	//1
								"procedure.code, "	//2
								"procedure.tooth, "	//3
								"procedure.day, "	//4
								"amblist.month, "	//5
								"amblist.year,	"	//6
								"procedure.price, "	//7
								"procedure.data, "	//8
								"procedure.temp, "	//9
								"amblist.LPK "		//10
						"FROM procedure LEFT JOIN amblist ON procedure.amblist_id = amblist.id "
						"WHERE amblist.id = " + amblist_id + " ORDER BY seq";


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

void DbProcedure::saveProcedures(const std::string& amblist_id, const std::vector<Procedure>& mList, Db* existingConnection)
{

	std::string query = "DELETE FROM procedure WHERE amblist_id = " + amblist_id;

	Db db(existingConnection);

	db.execute(query);

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
			+ Parser::write(m) + "','"
			+ amblist_id + "')"
			;

		db.execute(query);
	}

}

std::vector<ProcedureSummary> DbProcedure::getSummary(const std::string& patientID, const std::string& excludeAmbID)
{

	std::string query
	{
		"SELECT procedure.day, amblist.month, amblist.year, procedure.code, procedure.tooth "
		"FROM procedure LEFT JOIN amblist ON procedure.amblist_id = amblist.id "
		"WHERE nzok = 1 "
		"AND amblist.patient_id = '" + patientID + "' "
		"AND amblist.id != " + excludeAmbID
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
				db.asInt(4)
				 
			 });
	 }

	 return summary;

}

std::vector<Procedure> DbProcedure::getToothProcedures(const std::string& patientID, int tooth)
{
		std::string query =
		"SELECT  procedure.day, amblist.month, amblist.year, procedure.code, procedure.nzok, procedure.data, procedure.price, amblist.lpk, procedure.temp FROM "
		"procedure LEFT JOIN amblist ON procedure.amblist_id = amblist.id "
		"WHERE tooth = " + std::to_string(tooth) + " "
		"AND patient_id = '" + patientID + "' "
		"ORDER BY amblist.year ASC, amblist.month ASC, procedure.code ASC, procedure.seq ASC";

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
