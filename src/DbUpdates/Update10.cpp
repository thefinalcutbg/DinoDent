#include "DbUpdates.h"
#include "Resources.h"
#include "Database/Database.h"
#include <qstring.h>

void DbUpdates::update10(UpdateDialog&)
{

	if (Db::version() != 9) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(10))
	{
		db.execute(query);
	}

	struct RowidDiagnosis {
		long long rowid;
		int diagnosis;
	};

	auto getDiagnosisByDescription = [](const std::string& description)->int
	{
		QString d = QString::fromStdString(description).toLower();

		if (d.contains("caries")) return 1;
		if (d.contains("кариес")) return 1;
		if (d.contains("pulp")) return 2;
		if (d.contains("periodontitis")) return 3;
		if (d.contains("per.")) return 3;
		if (d.contains("fractura")) return 4;
		if (d.contains("causa")) return 9;
		if (d.contains("devital")) return 8;
		if (d.contains("totalis")) return 6;
		if (d.contains("partialis")) return 5;
		if (d.contains("adontia")) return 5;

		return 0;
	};

	db.newStatement("SELECT rowid, diagnosis_description FROM procedure WHERE diagnosis = 0");

	std::vector<RowidDiagnosis> vec;

	while (db.hasRows())
	{
		vec.push_back(
			RowidDiagnosis
			{
				.rowid = db.asRowId(0),
				.diagnosis = getDiagnosisByDescription(db.asString(1))
			}
		);
	}

	db.execute("BEGIN TRANSACTION");
	
	for (auto& it : vec)
	{
		db.newStatement("UPDATE procedure SET diagnosis = ? WHERE rowid = ?");
		db.bind(1, it.diagnosis);
		db.bind(2, it.rowid);
		db.execute();
	}

	db.execute("COMMIT TRANSACTION");

}
