#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"
#include "Model/Dental/DentalNum.h"
#include "Model/Dental/Procedure.h"
#include "JsonCpp/json.h"
#include "Model/Dental/VitaColor.h"
#include "View/Widgets/UpdateDialog.h"



template<typename T>
std::string jsonToNotesConversionU7
(
	Json::Value & json,
	const std::string keyName
)

{
	std::string result;

	{
		if (json.isMember(keyName))
		{
			auto idx = json.removeMember(keyName).asInt();

			if (idx)
			{
				result += T(idx).getName();
				result += " ";
			}
		}

		return result;
	}
};
#include "View/ModalDialogBuilder.h"
void DbUpdates::update7(UpdateDialog& progressDialog)
{
	if (Db::version() != 6) return;

	Db db;

	auto script = Resources::getMigrationScript(7);

	if (script.empty()) return;

	for (auto& query : script) { db.execute(query);};
	

	//separating procedure data and procedure notes:
	db.newStatement(
		"SELECT rowid, type, data FROM procedure"
	);

	struct ProcedureDbData {
		long long rowid;
		ProcedureType type;
		std::string data;
		std::string notes;
	};

	std::vector<ProcedureDbData> procedureData;

	while (db.hasRows())
	{
		procedureData.push_back(
			ProcedureDbData{
				.rowid = db.asRowId(0),
				.type = static_cast<ProcedureType>(db.asInt(1)),
				.data = db.asString(2)
			}
		);
	}

	Json::Reader jReader;
	Json::FastWriter jWriter;

	for (auto& p : procedureData)
	{
		Json::Value jsonData;

		jReader.parse(p.data, jsonData);

		switch (p.type)
		{
			case ProcedureType::obturation:
			{
				p.notes += jsonToNotesConversionU7<RestorationMaterial>(jsonData, "material");
				p.notes += jsonToNotesConversionU7<VitaColor>(jsonData, "color");
				p.data = jWriter.write(jsonData);
			}
			break;

			case ProcedureType::crown:
			{
				p.notes += jsonToNotesConversionU7<CrownMaterial>(jsonData, "material");
				p.notes += jsonToNotesConversionU7<CrownPreparation>(jsonData, "prep");
				p.notes += jsonToNotesConversionU7<VitaColor>(jsonData, "color_idx");
				p.data = jWriter.write(jsonData);
			}
			break;

			case ProcedureType::bridge:
			{
				p.notes += jsonToNotesConversionU7<CrownMaterial>(jsonData, "material");
				p.notes += jsonToNotesConversionU7<CrownPreparation>(jsonData, "prep");
				p.notes += jsonToNotesConversionU7<VitaColor>(jsonData, "color_idx");
				p.data = jWriter.write(jsonData);
			}
			break;

			case ProcedureType::fibersplint:
			{
				p.notes += jsonToNotesConversionU7<VitaColor>(jsonData, "color");
				jsonToNotesConversionU7<VitaColor>(jsonData, "material");
				p.data = jWriter.write(jsonData);
			}
			break;

			default: p.data = "";
		};

		
	}

	db.execute("BEGIN TRANSACTION");

	progressDialog.setRange(procedureData.size());

	for (auto& p : procedureData)
	{
		progressDialog.increment();

		db.newStatement("UPDATE procedure SET data=?, notes=? WHERE procedure.rowid=?");

		db.bind(1, p.data);
		db.bind(2, p.notes);
		db.bind(3, p.rowid);

		db.execute();
	}

	db.execute("COMMIT");
	
}
