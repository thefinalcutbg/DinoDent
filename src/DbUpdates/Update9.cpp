#include "DbUpdates.h"
#include "Database/Database.h"
#include <JsonCpp/json.h>
#include "Model/Dental/ToothContainer.h"
#include "Model/Parser.h"
#include "View/Widgets/UpdateDialog.h"

namespace Update8fn {

	void parse(const std::string& jsonString, ToothContainer& status)
	{
		Json::Value json;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(jsonString, json);

		if (!parsingSuccessful) {
			return;
		}

		const Json::Value& healthy = json["Healthy"];

		for (int i = 0; i < healthy.size(); i++)
		{
			Tooth& tooth = status[healthy[i]["idx"].asInt()];
			tooth.healthy.set(true);
		}


		const Json::Value& temporary = json["Temporary"];

		for (int i = 0; i < temporary.size(); i++)
		{
			Tooth& tooth = status[temporary[i]["idx"].asInt()];
			tooth.temporary.set(true);
		}

		const Json::Value& hyperdontic = json["Hyperdontic"];

		for (int i = 0; i < hyperdontic.size(); i++)
		{
			Tooth& tooth = status[hyperdontic[i]["idx"].asInt()];
			tooth.dsn.set(true);
			tooth.dsn.tooth().setStatus(StatusCode::Healthy);
		}

		const Json::Value& impacted = json["Impacted"];

		for (int i = 0; i < impacted.size(); i++)
		{
			Tooth& tooth = status[impacted[i]["idx"].asInt()];
			tooth.impacted.set(true);
		}

		const Json::Value& mobility = json["Mobility"];

		for (int i = 0; i < mobility.size(); i++)
		{
			Tooth& tooth = status[mobility[i]["idx"].asInt()];
			tooth.mobility.set(true);
			tooth.mobility.degree = static_cast<Degree>(mobility[i]["degree"].asInt());

		}

		const Json::Value& periodontitis = json["Periodontitis"];

		for (int i = 0; i < periodontitis.size(); i++)
		{
			Tooth& tooth = status[periodontitis[i]["idx"].asInt()];
			tooth.periodontitis.set(true);
		}

		const Json::Value& calculus = json["Calculus"];

		for (int i = 0; i < calculus.size(); i++)
		{
			Tooth& tooth = status[calculus[i]["idx"].asInt()];
			tooth.calculus.set(true);
		}


		const Json::Value& obturation = json["Obturation"];

		for (int i = 0; i < obturation.size(); i++)
		{
			Tooth& tooth = status[obturation[i]["idx"].asInt()];
			int surface = obturation[i]["Surface"].asInt();
			tooth.obturation.set(true, surface);
			tooth.obturation[surface].LPK = obturation[i]["LPK"].asString();

		}

		const Json::Value& car = json["Caries"];

		for (int i = 0; i < car.size(); i++)
		{
			Tooth& tooth = status[car[i]["idx"].asInt()];
			int surface = car[i]["Surface"].asInt();
			tooth.caries.set(true, surface);
		}

		const Json::Value& pulpitis = json["Pulpitis"];

		for (int i = 0; i < pulpitis.size(); i++)
		{
			Tooth& tooth = status[pulpitis[i]["idx"].asInt()];
			tooth.pulpitis.set(true);

		}

		const Json::Value& lesion = json["Lesion"];

		for (int i = 0; i < lesion.size(); i++)
		{
			Tooth& tooth = status[lesion[i]["idx"].asInt()];
			tooth.lesion.set(true);
		}

		const Json::Value& fracture = json["Fracture"];

		for (int i = 0; i < fracture.size(); i++)
		{
			Tooth& tooth = status[fracture[i]["idx"].asInt()];
			tooth.fracture.set(true);
		}

		const Json::Value& endo = json["EndoTreatment"];

		for (int i = 0; i < endo.size(); i++)
		{
			Tooth& tooth = status[endo[i]["idx"].asInt()];
			tooth.endo.set(true);
			tooth.endo.LPK = endo[i]["LPK"].asString();
		}

		const Json::Value& post = json["Post"];

		for (int i = 0; i < post.size(); i++)
		{
			Tooth& tooth = status[post[i]["idx"].asInt()];
			tooth.post.set(true);
			tooth.post.LPK = post[i]["LPK"].asString();
		}

		const Json::Value& extraction = json["Extraction"];

		for (int i = 0; i < extraction.size(); i++)
		{
			Tooth& tooth = status[extraction[i]["idx"].asInt()];
			tooth.extraction.set(true);
			tooth.extraction.LPK = extraction[i]["LPK"].asString();
		}

		const Json::Value& root = json["Root"];

		for (int i = 0; i < root.size(); i++)
		{

			Tooth& tooth = status[root[i]["idx"].asInt()];
			tooth.root.set(true);
		}

		const Json::Value& implant = json["Implant"];

		for (int i = 0; i < implant.size(); i++)
		{
			Tooth& tooth = status[implant[i]["idx"].asInt()];
			tooth.implant.set(true);
			tooth.implant.LPK = implant[i]["LPK"].asString();
		}

		const Json::Value& crown = json["Crown"];

		for (int i = 0; i < crown.size(); i++)
		{
			Tooth& tooth = status[crown[i]["idx"].asInt()];
			tooth.crown.set(true);
			tooth.crown.LPK = crown[i]["LPK"].asString();
		}

		const Json::Value& bridge = json["Bridge"];

		for (int i = 0; i < bridge.size(); i++)
		{
			Tooth& tooth = status[bridge[i]["idx"].asInt()];
			tooth.bridge.set(true);
			tooth.bridge.LPK = bridge[i]["LPK"].asString();
			tooth.bridge.position = static_cast<BridgePos>(bridge[i]["pos"].asInt());
		}

		const Json::Value& splint = json["Splint"];

		for (int i = 0; i < splint.size(); i++)
		{
			Tooth& tooth = status[splint[i]["idx"].asInt()];
			tooth.splint.position = static_cast<BridgePos>(splint[i]["pos"].asInt());
			tooth.splint.LPK = splint[i]["LPK"].asString();
			tooth.splint.set(true);
		}

		const Json::Value& denture = json["Denture"];

		for (int i = 0; i < denture.size(); i++)
		{
			Tooth& tooth = status[denture[i]["idx"].asInt()];
			tooth.denture.LPK = denture[i]["LPK"].asString();
			tooth.denture.set(true);
		}

	}
}

void DbUpdates::update9(UpdateDialog& progressDialog)
{

	if (Db::version() != 8) return;

	Db db;

	db.execute("PRAGMA foreign_keys = 0");

	db.newStatement("SELECT rowid, status FROM amblist");

	std::vector<std::pair<long long, ToothContainer>> rowid_status;

	while (db.hasRows())
	{
		ToothContainer teeth;
		Update8fn::parse(db.asString(1), teeth);
		
		rowid_status.push_back(std::make_pair(db.asRowId(0), teeth));
	}

	progressDialog.setRange(rowid_status.size());

	db.execute("BEGIN TRANSACTION");
	
	for (auto& pair : rowid_status)
	{
		db.newStatement("UPDATE amblist SET status = ? WHERE rowid = ?");

		db.bind(1, Parser::write(pair.second));
		db.bind(2, pair.first);

		db.execute();

		progressDialog.increment();
	}

	db.execute("COMMIT TRANSACTION");
	db.execute("PRAGMA foreign_keys = 1");
	db.execute("PRAGMA user_version = 9");

}