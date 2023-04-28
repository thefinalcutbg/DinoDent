#include "DbProcedure.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/Parser.h"
#include "Model/User.h"
#include "Database.h"

std::vector<Procedure> DbProcedure::getProcedures(long long amblist_rowid, Db& db, bool nhifOnly, bool removed)
{
	std::vector<Procedure> mList;

	std::string condition = nhifOnly ?
		" AND procedure.financing_source=" + std::to_string(static_cast<int>(FinancingSource::NHIF)) : "" ;

	std::string query =  "SELECT procedure.financing_source, "		//0
								"procedure.code, "					//1
								"procedure.tooth, "					//2
								"procedure.date, "					//3
								"procedure.data, "					//4
								"procedure.deciduous, "				//5
								"amblist.LPK, "						//6
								"procedure.diagnosis, "				//7
								"procedure.notes, "					//8
								"procedure.hyperdontic, "			//9
								"procedure.his_index, "				//10
								"procedure.additional_description "	//11
						"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
						"WHERE amblist.rowid=? "
						"AND procedure.removed=? "
						+ condition +
						" ORDER BY procedure.rowid";

	db.newStatement(query);

	db.bind(1, amblist_rowid);
	db.bind(2, removed);

	while(db.hasRows())
	{
		mList.emplace_back();
		Procedure& p = mList.back();

		p.financingSource = static_cast<FinancingSource>(db.asInt(0));
		p.code = db.asString(1);
		p.tooth = db.asInt(2);
		p.date = Date{ db.asString(3) };
		Parser::parse(db.asString(4), p);
		p.temp = db.asInt(5);
		p.LPK = db.asString(6);
		p.diagnosis = db.asInt(7);
		p.notes = db.asString(8);
		p.hyperdontic = db.asBool(9);
		p.his_index = db.asInt(10);
		p.diagnosis.additionalDescription = db.asString(11);
	}

	return mList;

}

void DbProcedure::saveProcedures(long long amblist_rowid, const std::vector<Procedure>& pList, const std::vector<Procedure> removedList, Db& db)
{
	std::string query = "DELETE FROM procedure WHERE amblist_rowid = " + std::to_string(amblist_rowid);

	db.execute(query);

	struct ProcedurePointer {
		const Procedure* const p;
		const bool removed;
	};

	std::vector<ProcedurePointer> toInsert;
	toInsert.reserve(pList.size() + removedList.size());

	for (auto& procedures : pList) {
		toInsert.push_back(ProcedurePointer{ &procedures, false });
	}

	for (auto& removed : removedList) {

		if (!removed.isSentToHis()) continue;

		toInsert.push_back(ProcedurePointer{ &removed, true });
	}

	for (int i = 0; i < toInsert.size(); i++)
	{
		auto& p = *toInsert[i].p;

		db.newStatement(
			"INSERT INTO procedure "
			"(date, financing_source, code, tooth, deciduous, data, amblist_rowid, diagnosis, notes, hyperdontic, his_index, additional_description, removed) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		db.bind(1, p.date.to8601());
		db.bind(2, static_cast<int>(p.financingSource));
		db.bind(3, p.code.code());
		db.bind(4, p.tooth);
		db.bind(5, p.temp);
		db.bind(6, Parser::write(p));
		db.bind(7, amblist_rowid);
		db.bind(8, p.diagnosis.index());
		db.bind(9, p.notes);
		db.bind(10, p.hyperdontic);
		db.bind(11, p.his_index);
		db.bind(12, p.diagnosis.additionalDescription);
		db.bind(13, toInsert[i].removed);
		db.execute();
		
	}

}

std::vector<ProcedureSummary> DbProcedure::getNhifSummary(long long patientRowId, long long excludeAmbRowId, const Date& from, const Date& to)
{
	std::string query
	{
		"SELECT procedure.date, procedure.code, procedure.tooth, procedure.deciduous, procedure.hyperdontic, "
		"FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE financing_source=" + std::to_string(static_cast<int>(FinancingSource::NHIF)) + " "
		"AND procedure.removed = 0 "
		"AND amblist.patient_rowid = " + std::to_string(patientRowId) + " "
		"AND amblist.rowid != " + std::to_string(excludeAmbRowId) + " "
		"AND amblist.date BETWEEN  ('" + from.to8601() + "') AND ('" + to.to8601() + "')"
	};

	 std::vector<ProcedureSummary> summary;

	 for (Db db(query); db.hasRows();)
	 {

		 ProcedureCode code(db.asString(1));

		 summary.push_back(
			 {
				Date{ db.asString(0) },
				code.oldCode(),
				db.asInt(2),
				db.asBool(3),
				code.type() == ProcedureType::extraction,
				db.asBool(5)
			 });
	 }

	 return summary;

}

std::vector<Procedure> DbProcedure::getToothProcedures(long long patientRowId, int tooth)
{
		std::string query =
		"SELECT  "
			"procedure.date, "
			"procedure.code, "
			"procedure.financing_source, "
			"procedure.data, "
			"amblist.lpk, "
			"procedure.deciduous, "
			"procedure.diagnosis, "
			"procedure.notes, "
			"procedure.hyperdontic, "
			"procedure.additional_description "
			"FROM "
		"procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE tooth = " + std::to_string(tooth) + " "
		"AND patient_rowid = " + std::to_string(patientRowId) + " "
		"AND procedure.removed = 0 "
		"ORDER BY procedure.date ASC, procedure.code ASC, procedure.rowid ASC";

	std::vector<Procedure> procedures;
	
	for (Db db(query); db.hasRows();)
	{

		procedures.emplace_back();
		auto& p = procedures.back();
		
		p.date = Date{ db.asString(0) };

		p.code = db.asString(1);
		p.financingSource = static_cast<FinancingSource>(db.asInt(2));
		Parser::parse(db.asString(3), p);
		p.LPK = db.asString(4);
		p.temp = db.asInt(5);
		p.tooth = tooth;
		p.diagnosis = db.asInt(6);
		p.notes = db.asString(7);
		p.hyperdontic = db.asBool(8);
		p.diagnosis.additionalDescription = db.asString(9);
	}
	
	
	return procedures;

}
