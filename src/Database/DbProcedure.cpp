#include "DbProcedure.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/User.h"
#include "Database.h"

std::vector<Procedure> DbProcedure::getProcedures(long long amblist_rowid, Db& db, bool nhifOnly, bool removed)
{
	std::vector<Procedure> mList;

	std::string condition = nhifOnly ?
		" AND procedure.financing_source=" + std::to_string(static_cast<int>(FinancingSource::NHIF)) : "" ;

	std::string query =  "SELECT procedure.date, "					//0
								"procedure.financing_source, "		//1
								"procedure.code, "					//2
								"procedure.at_tooth_index, "		//3
								"procedure.temporary, "				//4
								"procedure.supernumeral, "			//5
								"amblist.LPK, "						//6
								"procedure.icd, "					//7
								"procedure.diagnosis_description, "	//8
								"procedure.notes, "					//9
								"procedure.his_index, "				//10
								"procedure.surface_o, "				//11
								"procedure.surface_m, "				//12
								"procedure.surface_d, "				//13
								"procedure.surface_b, "				//14
								"procedure.surface_l, "				//15
								"procedure.surface_c, "				//16
								"procedure.post, "					//17
								"procedure.from_tooth_index, "		//18
								"procedure.to_tooth_index, "		//19
								"procedure.minutes "				//20
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

		p.date = db.asString(0);
		p.financingSource = static_cast<FinancingSource>(db.asInt(1));
		p.code = db.asString(2);
		p.LPK = db.asString(6);
		p.diagnosis.icd = db.asString(7);
		p.diagnosis.additional_descr = db.asString(8);
		p.notes = db.asString(9);
		p.his_index = db.asInt(10);

		auto scope = p.code.getScope();

		if (scope == ProcedureScope::SingleTooth || scope == ProcedureScope::Ambi) {

			p.affectedTeeth = ToothIndex{
				.index = db.asInt(3),
				.temp = db.asBool(4),
				.supernumeral = db.asBool(5)
			};
		}

		if (scope == ProcedureScope::Range || scope == ProcedureScope::Ambi)
		{
			if (!p.getToothIndex().isValid()) {

				p.affectedTeeth = ConstructionRange{
					db.asInt(18),
					db.asInt(19)
				};
			}
		}

		RestorationData rData = RestorationData{
				.surfaces = {
					db.asBool(11),
					db.asBool(12),
					db.asBool(13),
					db.asBool(14),
					db.asBool(15),
					db.asBool(16)
				},
				.post = db.asBool(17)

		};

		if (p.code.type() == ProcedureType::Restoration || p.code.type() == ProcedureType::RemoveRestoration)
		{
			p.param = rData;
		}

		if (p.code.type() == ProcedureType::CrownOrBridgeOrVeneer && rData.isValid()) {

			p.param = rData;

		}

		if (p.code.type() == ProcedureType::Anesthesia)
		{
			p.param = AnesthesiaMinutes{ db.asInt(20) };
		}
		
	}

	return mList;

}

std::vector<Procedure> DbProcedure::getProcedures(long long amblist_rowid, bool nhifOnly, bool removed)
{
	Db db;
	return getProcedures(amblist_rowid, db, nhifOnly, removed);
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

    for (std::size_t i = 0; i < toInsert.size(); i++)
	{
		auto& p = *toInsert[i].p;

		db.newStatement(
			"INSERT INTO procedure "
			"(date, code, financing_source, at_tooth_index, temporary, supernumeral, amblist_rowid, icd, diagnosis_description, notes, his_index, removed, "
			"surface_o, surface_m, surface_d, surface_b, surface_l, surface_c, post, from_tooth_index, to_tooth_index, minutes) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		db.bind(1, p.date.to8601());
		db.bind(2, p.code.code());
		db.bind(3, static_cast<int>(p.financingSource));

		//setting the index no matter if it is valid or not
		auto& tooth = p.getToothIndex();

		db.bind(4, tooth.index);
		db.bind(5, tooth.temp);
		db.bind(6, tooth.supernumeral);

		switch (p.getScope())
		{
			case ProcedureScope::SingleTooth:
			{
				if (std::holds_alternative<RestorationData>(p.param)) {

					auto& [surfaces, post] = std::get<RestorationData>(p.param);

					for (int i = 0; i < 6; i++) {
						db.bind(13 + i, surfaces[i]);
					}

					db.bind(19, post);
				}

			}
				break;

			case ProcedureScope::Range:
			{
				auto& [from, to] = std::get<ConstructionRange>(p.affectedTeeth);

				db.bind(4, -1);
				db.bind(20, from);
				db.bind(21, to);

				break;
			}
			case ProcedureScope::AllOrNone:

				db.bind(4, -1);

				if (p.code.type() == ProcedureType::Anesthesia) {
					db.bind(22, std::get<AnesthesiaMinutes>(p.param).minutes);
				}

				break;

		}


		db.bind(7, amblist_rowid);
		db.bind(8, p.diagnosis.icd.code());
		db.bind(9, p.diagnosis.additional_descr);
		db.bind(10, p.notes);
		db.bind(11, p.his_index);
		db.bind(12, toInsert[i].removed);

		db.execute();
		
	}

}

std::vector<ProcedureSummary> DbProcedure::getNhifSummary(long long patientRowId, long long excludeAmbRowId, const Date& from, const Date& to)
{
	std::string query
	{
		"SELECT procedure.date, procedure.code, procedure.at_tooth_index, procedure.temporary, procedure.supernumeral "
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
		 summary.push_back(
			 {
				.date = Date{ db.asString(0) },
				.code = ProcedureCode(db.asString(1)).nhifCode(),
				.tooth_idx = ToothIndex{
							.index = db.asInt(2),
							.temp = db.asBool(3),
							.supernumeral = db.asBool(4)
							}
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
			"amblist.lpk, "
			"procedure.temporary, "
			"procedure.supernumeral, "
			"procedure.icd, "
			"procedure.diagnosis_description, "
			"procedure.notes "
			"FROM "
		"procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE at_tooth_index = " + std::to_string(tooth) + " "
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
		p.LPK = db.asString(3);

		p.affectedTeeth = ToothIndex{
			.index = tooth,
			.temp = db.asBool(4),
			.supernumeral = db.asBool(5)
		};

		p.diagnosis.icd = db.asString(6);
		p.diagnosis.additional_descr = db.asString(7);
		p.notes = db.asString(8);

	}
	
	
	return procedures;

}

std::vector<Procedure> DbProcedure::getPatientProcedures(long long patientRowid)
{
	std::vector<Procedure> mList;

	std::string query = "SELECT procedure.date, "					//0
		"procedure.financing_source, "		//1
		"procedure.code, "					//2
		"procedure.at_tooth_index, "		//3
		"procedure.temporary, "				//4
		"procedure.supernumeral, "			//5
		"amblist.LPK, "						//6
		"procedure.icd, "					//7
		"procedure.diagnosis_description, "	//8
		"procedure.notes, "					//9
		"procedure.his_index, "				//10
		"procedure.surface_o, "				//11
		"procedure.surface_m, "				//12
		"procedure.surface_d, "				//13
		"procedure.surface_b, "				//14
		"procedure.surface_l, "				//15
		"procedure.surface_c, "				//16
		"procedure.post, "					//17
		"procedure.from_tooth_index, "		//18
		"procedure.to_tooth_index, "		//19
		"procedure.minutes "				//20
		"FROM procedure "				
		"LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"LEFT JOIN patient ON amblist.patient_rowid = patient.rowid "
		"WHERE patient.rowid=? "
		"AND procedure.removed=0 "
		" ORDER BY procedure.date DESC";

	Db db;

	db.newStatement(query);

	db.bind(1, patientRowid);

	while (db.hasRows())
	{
		mList.emplace_back();
		Procedure& p = mList.back();

		p.date = db.asString(0);
		p.financingSource = static_cast<FinancingSource>(db.asInt(1));
		p.code = db.asString(2);

		p.LPK = db.asString(6);
		p.diagnosis.icd = db.asString(7);
		p.diagnosis.additional_descr = db.asString(8);
		p.notes = db.asString(9);
		p.his_index = db.asInt(10);
		p.LPK = db.asString(6);
		p.diagnosis.icd = db.asString(7);
		p.diagnosis.additional_descr = db.asString(8);
		p.notes = db.asString(9);
		p.his_index = db.asInt(10);
		auto scope = p.code.getScope();

		if (scope == ProcedureScope::SingleTooth || scope == ProcedureScope::Ambi) {

			p.affectedTeeth = ToothIndex{
				.index = db.asInt(3),
				.temp = db.asBool(4),
				.supernumeral = db.asBool(5)
			};
		}

		if (scope == ProcedureScope::Range || scope == ProcedureScope::Ambi)
		{
			if (!p.getToothIndex().isValid()) {

				p.affectedTeeth = ConstructionRange{
					db.asInt(18),
					db.asInt(19)
				};
			}
		}


		RestorationData rData = RestorationData{
				.surfaces = {
					db.asBool(11),
					db.asBool(12),
					db.asBool(13),
					db.asBool(14),
					db.asBool(15),
					db.asBool(16)
				},
				.post = db.asBool(17)

		};

		if (p.code.type() == ProcedureType::Restoration || p.code.type() == ProcedureType::RemoveRestoration)
		{
			p.param = rData;
		}

		if (p.code.type() == ProcedureType::CrownOrBridgeOrVeneer && rData.isValid()) {

			p.param = rData;

		}

		if (p.code.type() == ProcedureType::Anesthesia)
		{
			p.param = AnesthesiaMinutes{ db.asInt(20) };
		}

	}

	return mList;

}

std::vector<int> DbProcedure::getDailyNhifProcedures(const Date& date, long long excludeAmblistRowid)
{
	std::vector<int> result;

	Db db;

	db.newStatement(
		"SELECT code FROM procedure "
		"LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
		"WHERE amblist.lpk=? "
		"AND amblist.rzi=? "
		"AND procedure.date=? "
		"AND amblist.rowid!=? "
		"AND procedure.removed = 0 "
		"AND financing_source=2" 
	);

	db.bind(1, User::doctor().LPK);
	db.bind(2, User::practice().rziCode);
	db.bind(3, date.to8601());
	db.bind(4, excludeAmblistRowid);

	while (db.hasRows())
	{
		result.push_back(ProcedureCode(db.asString(0)).nhifCode());
	}

	return result;
}
/*
std::unordered_map<std::string, double> DbProcedure::getCodeValues()
{
	std::unordered_map<std::string, double> result;

	Db db;

	db.newStatement("SELECT code, value FROM code_list WHERE rzi=?");

	db.bind(1, User::practice().rziCode);

	while (db.hasRows()) {
		result[db.asString(0)] = db.asDouble(1);
	}

	return result;
}

bool DbProcedure::setPrice(const std::string& code, double price)
{
	Db db;

	if (!price) {
		db.newStatement("DELETE FROM code_list WHERE code=? AND rzi=?");
		db.bind(1, code);
		db.bind(2, User::practice().rziCode);
		return db.execute();
	}

	db.newStatement(
		"INSERT INTO code_list(rzi, code, value) VALUES(?,?,?) "
		"ON CONFLICT(rzi, code) DO UPDATE SET value = excluded.value"
	);

	db.bind(1, User::practice().rziCode);
	db.bind(2, code);
	db.bind(3, price);

	return db.execute();
}
*/