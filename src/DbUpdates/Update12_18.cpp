#include "Database/Database.h"
#include "DbUpdates.h"
#include "Resources.h"
#include "Database/DbPatient.h"
#include "Model/FreeFunctions.h"
#include "GlobalSettings.h"


void DbUpdates::update16()
{
	if (Db::version() != 15) return;
	Db db;

	for (auto& query : Resources::getMigrationScript(16))
	{
		db.execute(query);
	}

	struct PatientAllergy {
		long long patient_rowid;
		std::string allergy;
	};

	db.newStatement("SELECT patient_rowid, data FROM medical_status WHERE type=0");

	std::vector<PatientAllergy> toInsert;

	while (db.hasRows())
	{
		toInsert.push_back({ db.asRowId(0), db.asString(1) });
		toInsert.back().allergy += " (моля, редактирайте)";
	}

	for (auto& a : toInsert) {

			db.newStatement(
				"INSERT INTO allergy "
				"(patient_rowid, lrn, nrn, description, type, category, criticality, clinical_status, verification_status, last_occurrence, edited) "
				"VALUES (?,?,?,?,?,?,?,?,?,?,?)"
			);

			db.bind(1, a.patient_rowid);
			db.bind(2, FreeFn::getUuid());
			db.bind(3, "");
			db.bind(4, a.allergy);
			db.bind(5, 0);
			db.bind(6, 0);
			db.bind(7, 0);
			db.bind(8, 0);
			db.bind(9, 0);
			db.bind(10, "");
			db.bind(11, 0);

			db.execute();
	}

	db.newStatement("DELETE FROM medical_status WHERE type=0");
	db.execute();

}

void DbUpdates::update17()
{
	if (Db::version() != 16) return;
	Db db;

	for (auto& query : Resources::getMigrationScript(17))
	{
		db.execute(query);
	}

	GlobalSettings::setPkcs11PathList(GlobalSettings::getDefaultPkcs11Paths());
}

void DbUpdates::update18()
{
	if (Db::version() != 17) return;

	Db::crudQuery("PRAGMA user_version=18");
	GlobalSettings::setPkcs11PathList(GlobalSettings::getDefaultPkcs11Paths());
}