#include "Parser.h"
#include "JsonCpp/json.h"



Json::Value writePathology(int index, const Pathology& pathology)
{
	Json::Value parameters;
	parameters["diag_idx"] = pathology.data.diagnosis_index;
	parameters["date"] = Date::toString(pathology.data.date_diagnosed);
	parameters["idx"] = index;
	return parameters;
}

Json::Value writeDentistMade(int index, const DentistMade& procedure)
{
	Json::Value parameters;
	parameters["LPK"] = procedure.LPK;
	parameters["idx"] = index;
	return parameters;
}

std::string Parser::write(const PerioStatus& status)
{
	Json::Value json;
	
	json["PD"] = Json::Value(Json::arrayValue);
	for (auto& pd : status.pd)
		json["PD"].append(pd);

	json["CAL"] = Json::Value(Json::arrayValue);
	for (auto& cal : status.cal)
		json["CAL"].append(cal);

	json["BOP"] = Json::Value(Json::arrayValue);
	for (auto& bop : status.bop)
		json["BOP"].append(static_cast<int>(bop));
	
	json["AG"] = Json::Value(Json::arrayValue);
	for (auto& ag : status.ag)
		json["AG"].append(ag);
	
	json["Furc"] = Json::Value(Json::arrayValue);
	for (auto& furc : status.furcation)
		json["Furc"].append(furc);

	json["FMBS"] = Json::Value(Json::arrayValue);
	for (auto& FMBS : status.FMBS)
		json["FMBS"].append(static_cast<int>(FMBS));

	json["FMPS"] = Json::Value(Json::arrayValue);
	for (auto& FMPS : status.FMPS)
		json["FMPS"].append(static_cast<int>(FMPS));

	json["Disabled"] = Json::Value(Json::arrayValue);
	for (auto& d : status.disabled)
		json["Disabled"].append(static_cast<int>(d));

	json["Smoker"] = status.smoker;
	json["BoneLoss"] = status.boneLoss;
	json["Restore"] = status.completeRestorationNeeded;
	json["Sys"] = status.systemic;

	Json::FastWriter writer;

	return writer.write(json);
}

std::string Parser::write(const ToothContainer& status)
{
	Json::Value json;

	for (int i = 0; i < 32; i++)
	{
		const Tooth& tooth = status[i];


		if (tooth.temporary.exists())
		{
			if (!json.isMember("Temporary"))
			{
				json["Temporary"] = Json::Value(Json::arrayValue);
			}
			Json::Value parameters;
			parameters["idx"] = i;
			json["Temporary"].append(parameters);
		}

		if (tooth.hyperdontic.exists())
		{
			if (!json.isMember("Hyperdontic"))
			{
				json["Hyperdontic"] = Json::Value(Json::arrayValue);
			}
			Json::Value parameters;
			parameters["idx"] = i;
			json["Hyperdontic"].append(parameters);
		}


		if (tooth.impacted.exists())
		{
			if (!json.isMember("Impacted"))
			{
				json["Impacted"] = Json::Value(Json::arrayValue);
			}
			Json::Value parameters;
			parameters["idx"] = i;
			json["Impacted"].append(parameters);
		}

		if (tooth.mobility.exists())
		{
			if (!json.isMember("Mobility"))
			{
				json["Mobility"] = Json::Value(Json::arrayValue);
			}
			Json::Value parameters;
			parameters["idx"] = i;
			parameters["degree"] = static_cast<int>(tooth.mobility.degree);
			json["Mobility"].append(parameters);
		}

		if (tooth.periodontitis.exists())
		{
			if (!json.isMember("Periodontitis"))
			{
				json["Periodontitis"] = Json::Value(Json::arrayValue);
			}
			Json::Value parameters;
			parameters["idx"] = i;
			json["Periodontitis"].append(parameters);
		}

		if (tooth.obturation.exists())
		{
			if (!json.isMember("Obturation"))
			{
				json["Obturation"] = Json::Value(Json::arrayValue);
			}
			for (int y = 0; y < 6; y++)
			{
				if (tooth.obturation.exists(y)) {

					auto parameters = writeDentistMade(i, tooth.obturation[y]);
					parameters["Surface"] = y;
					parameters["color"] = tooth.obturation[y].data.color;
					parameters["material"] = tooth.obturation[y].data.material;
					json["Obturation"].append(parameters);
				}
			}
		}


		if (tooth.caries.exists())
		{
			if (!json.isMember("Caries"))
			{
				json["Caries"] = Json::Value(Json::arrayValue);
			}
			for (int y = 0; y < 6; y++)
			{
				if (tooth.caries.exists(y)) {

					auto parameters = writePathology(i, tooth.caries[y]);
					parameters["Surface"] = y;
					json["Caries"].append(parameters);
				}

			}
		}

		if (tooth.pulpitis.exists())
		{
			if (!json.isMember("Pulpitis"))
			{
				json["Pulpitis"] = Json::Value(Json::arrayValue);
			}
			json["Pulpitis"].append(writePathology(i, tooth.pulpitis));
		}

		if (tooth.lesion.exists())
		{
			if (!json.isMember("Lesion"))
			{
				json["Lesion"] = Json::Value(Json::arrayValue);
			}

			json["Lesion"].append(writePathology(i, tooth.lesion));
		}

		if (tooth.endo.exists())
		{
			if (!json.isMember("EndoTreatment"))
			{
				json["EndoTreatment"] = Json::Value(Json::arrayValue);
			}

			json["EndoTreatment"].append(writeDentistMade(i, tooth.endo));
		}

		if (tooth.post.exists())
		{
			if (!json.isMember("Post"))
			{
				json["Post"] = Json::Value(Json::arrayValue);
			}

			json["Post"].append(writeDentistMade(i, tooth.post));
		}


		if (tooth.extraction.exists())
		{

			if (!json.isMember("Extraction"))
			{
				json["Extraction"] = Json::Value(Json::arrayValue);
			}

			json["Extraction"].append(writeDentistMade(i, tooth.extraction));
		}

		if (tooth.root.exists())
		{
			if (!json.isMember("Root"))
			{
				json["Root"] = Json::Value(Json::arrayValue);
			}

			json["Root"].append(writePathology(i, tooth.root));
		}


		if (tooth.fracture.exists())
		{
			if (!json.isMember("Fracture"))
			{
				json["Fracture"] = Json::Value(Json::arrayValue);
			}

			json["Fracture"].append(writePathology(i, tooth.fracture));
		}

		if (tooth.crown.exists())
		{
			if (!json.isMember("Crown"))
			{
				json["Crown"] = Json::Value(Json::arrayValue);
			}
			auto param = writeDentistMade(i, tooth.crown);
			param["material"] = tooth.crown.data.material;
			param["prep"] = tooth.crown.data.prep_type;
			param["color"] = tooth.crown.data.color;
			json["Crown"].append(param);
		}

		if (tooth.implant.exists())
		{
			if (!json.isMember("Implant"))
			{
				json["Implant"] = Json::Value(Json::arrayValue);
			}
			auto param = writeDentistMade(i, tooth.implant);
			param["system"] = tooth.implant.data.system;
			param["time"] = tooth.implant.data.time;
			param["type"] = tooth.implant.data.type;
			param["w"] = tooth.implant.data.width;
			param["l"] = tooth.implant.data.length;
			param["bone"] = tooth.implant.data.bone_aug;
			param["tissue"] = tooth.implant.data.tissue_aug;
			param["sinus"] = tooth.implant.data.sinusLift;
			param["membrane"] = tooth.implant.data.membrane;
			json["Implant"].append(param);
		}

		if (tooth.bridge.exists())
		{
			if (!json.isMember("Bridge"))
			{
				json["Bridge"] = Json::Value(Json::arrayValue);
			}
			auto param = writeDentistMade(i, tooth.bridge);
			param["pos"] = static_cast<int>(tooth.bridge.position);
			param["material"] = tooth.bridge.data.material;
			param["prep"] = tooth.bridge.data.prep_type;
			param["color"] = tooth.bridge.data.color;
			json["Bridge"].append(param);
		}

		if (tooth.splint.exists())
		{
			if (!json.isMember("Splint"))
			{
				json["Splint"] = Json::Value(Json::arrayValue);
			}

			auto param = writeDentistMade(i, tooth.splint);
			param["pos"] = static_cast<int>(tooth.splint.position);
			json["Splint"].append(param);
		}
	}

	Json::FastWriter writer;

	return writer.write(json);
}

std::string Parser::write(const Procedure& procedure)
{
	Json::Value json;

	json["diagnosis"] = procedure.diagnosis;
	json["name"] = procedure.name;

	switch (procedure.type)
	{
	case ProcedureType::obturation:
	{
		auto& r = std::get<ProcedureObtData>(procedure.result);
		json["color"] = r.data.color;
		json["post"] = r.post;
		json["material"] = r.data.material;
		json["surfaces"] = Json::Value(Json::arrayValue);

		for (int i = 0; i < r.surfaces.size(); i++)
			if (r.surfaces[i]) json["surfaces"].append(i);

		break;
	}
	case ProcedureType::crown:
	{
		auto& r = std::get<CrownData>(procedure.result);
		json["color_idx"] = r.color;
		json["material"] = r.material;
		json["prep"] = r.prep_type;
		break;
	}
	case ProcedureType::bridge:
	{
		auto& r = std::get<ProcedureBridgeData>(procedure.result);
		json["color_idx"] = r.crown.color;
		json["material"] = r.crown.material;
		json["prep"] = r.crown.prep_type;
		json["begin"] = r.tooth_begin;
		json["end"] = r.tooth_end;
		break;
	}

	case ProcedureType::implant:
	{
		auto& r = std::get<ImplantData>(procedure.result);

		json["system"] = r.system;
		json["time"] = r.time;
		json["type"] = r.type;
		json["w"] = r.width;
		json["l"] = r.length;
		json["tissue"] = r.tissue_aug;
		json["bone"] = r.bone_aug;
		json["membrane"] = r.membrane;
		json["sinus"] = r.sinusLift;

		break;
	}

	default:
		break;
	}

	Json::FastWriter writer;

	return writer.write(json);
}

void Parser::parse(const std::string& jsonString, Procedure& procedure)
{
	Json::Value json;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
	}

	procedure.diagnosis = json["diagnosis"].asString();

	if (!json["name"].isNull())
	{
		procedure.name = json["name"].asString();
	}


	switch (procedure.type)
	{
	case ProcedureType::obturation:
	{
		procedure.result = ProcedureObtData
		{
			std::array<bool, 6>{false},
			json["post"].asBool(),
			ObturationData
			{
				json["color"].asInt(),
				json["material"].asString()

			}
		};

		const Json::Value& surfaces = json["surfaces"];

		for (int i = 0; i < surfaces.size(); i++)
			std::get<ProcedureObtData>(procedure.result).surfaces[surfaces[i].asInt()] = true;


		break;
	}

	case ProcedureType::crown:
	{
		procedure.result = CrownData
		{
			json["material"].asString(),
			json["prep"].asInt(),
			json["color_idx"].asInt()
		};

		break;
	}

	case ProcedureType::bridge:
	{
		procedure.result = ProcedureBridgeData
		{
			json["begin"].asInt(),
			json["end"].asInt(),
			CrownData
			{
				json["material"].asString(),
				json["prep"].asInt(),
				json["color_idx"].asInt()
			}

		};

		break;
	}

	case ProcedureType::implant:
	{
		procedure.result = ImplantData
		{
			json["system"].asString(),
			json["w"].asDouble(),
			json["l"].asDouble(),
			json["time"].asInt(),
			json["type"].asInt(),
			json["tissue"].asInt(),
			json["bone"].asInt(),
			json["membrane"].asBool(),
			json["sinus"].asBool()
		};

		break;
	}
	default:
		procedure.result = NoData{};
	}
}

void Parser::parse(const std::string& jsonString, DetailsSummary& summary)
{
	Json::Value procedure;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, procedure);

	if (!parsingSuccessful) {
		return;
	}

	summary.procedureDiagnosis = procedure["diagnosis"].asString();

	if (!procedure["name"].isNull())
	{
		summary.procedureName = procedure["name"].asString();
	}

}

void Parser::parse(const std::string& jsonString, PerioStatus& status)
{
	Json::Value json;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
	}

	const Json::Value& pd = json["PD"];
	for (int i = 0; i < pd.size(); i++)
		status.pd[i] = pd[i].asInt();

	const Json::Value& cal = json["CAL"];
	for (int i = 0; i < cal.size(); i++)
		status.cal[i] = cal[i].asInt();

	const Json::Value& bop = json["BOP"];
	for (int i = 0; i < bop.size(); i++)
		status.bop[i] = bop[i].asBool();

	const Json::Value& ag = json["AG"];
	for (int i = 0; i < ag.size(); i++)
		status.ag[i] = ag[i].asInt();

	const Json::Value& furc = json["Furc"];
	for (int i = 0; i < furc.size(); i++)
		status.furcation[i] = furc[i].asInt();
	
	const Json::Value& fmps = json["FMPS"];
	for (int i = 0; i < fmps.size(); i++)
		status.FMPS[i] = fmps[i].asBool();

	const Json::Value& fmbs = json["FMBS"];
	for (int i = 0; i < fmbs.size(); i++)
		status.FMBS[i] = fmbs[i].asBool();

	status.smoker = json["Smoker"].asInt();
	status.boneLoss = json["BoneLoss"].asInt();
	status.completeRestorationNeeded = json["Restore"].asBool();
	status.systemic = json["Sys"].asBool();
}

void Parser::parse(const std::string& jsonString, ToothContainer& status)
{
	Json::Value json;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
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
		tooth.hyperdontic.set(true);
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


	const Json::Value& obturation = json["Obturation"];

	for (int i = 0; i < obturation.size(); i++)
	{
		Tooth& tooth = status[obturation[i]["idx"].asInt()];
		int surface = obturation[i]["Surface"].asInt();
		tooth.obturation.set(true, surface);
		tooth.obturation[surface].LPK = obturation[i]["LPK"].asString();
		tooth.obturation[surface].data.material = obturation[i]["material"].asString();
		tooth.obturation[surface].data.color = obturation[i]["color"].asInt();
	}

	const Json::Value& car = json["Caries"];

	for (int i = 0; i < car.size(); i++)
	{
		Tooth& tooth = status[car[i]["idx"].asInt()];
		int surface = car[i]["Surface"].asInt();
		tooth.caries.set(true, surface);
		tooth.caries[surface].data.diagnosis_index = car[i]["diag_idx"].asInt();
		tooth.caries[surface].data.date_diagnosed = car[i]["date"].asString();
	}

	const Json::Value& pulpitis = json["Pulpitis"];

	for (int i = 0; i < pulpitis.size(); i++)
	{
		Tooth& tooth = status[pulpitis[i]["idx"].asInt()];
		tooth.pulpitis.set(true);

		tooth.pulpitis.data.diagnosis_index = pulpitis[i]["diag_idx"].asInt();
		tooth.pulpitis.data.date_diagnosed = pulpitis[i]["date"].asString();
	}

	const Json::Value& lesion = json["Lesion"];

	for (int i = 0; i < lesion.size(); i++)
	{
		Tooth& tooth = status[lesion[i]["idx"].asInt()];
		tooth.lesion.set(true);
		tooth.lesion.data.diagnosis_index = lesion[i]["diag_idx"].asInt();
		tooth.lesion.data.date_diagnosed = lesion[i]["date"].asString();
	}

	const Json::Value& fracture = json["Fracture"];

	for (int i = 0; i < fracture.size(); i++)
	{
		Tooth& tooth = status[fracture[i]["idx"].asInt()];
		tooth.fracture.set(true);
		tooth.fracture.data.diagnosis_index = fracture[i]["diag_idx"].asInt();
		tooth.fracture.data.date_diagnosed = fracture[i]["date"].asString();
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
		tooth.root.data.date_diagnosed = Date{ root[i]["date"].asString() };
		tooth.root.data.diagnosis_index = root[i]["diag_idx"].asInt();
		tooth.root.set(true);
	}

	const Json::Value& implant = json["Implant"];

	for (int i = 0; i < implant.size(); i++)
	{
		Tooth& tooth = status[implant[i]["idx"].asInt()];
		tooth.implant.set(true);
		tooth.implant.LPK = implant[i]["LPK"].asString();
		tooth.implant.data.system = implant[i]["material"].asString();
		tooth.implant.data.length = implant[i]["l"].asDouble();
		tooth.implant.data.width = implant[i]["w"].asDouble();
		tooth.implant.data.time = implant[i]["time"].asInt();
		tooth.implant.data.type = implant[i]["type"].asInt();
		tooth.implant.data.bone_aug = implant[i]["bone"].asInt();
		tooth.implant.data.tissue_aug = implant[i]["tissuee"].asInt();
		tooth.implant.data.sinusLift = implant[i]["sinus"].asBool();
		tooth.implant.data.membrane = implant[i]["membrane"].asBool();

	}

	const Json::Value& crown = json["Crown"];

	for (int i = 0; i < crown.size(); i++)
	{
		Tooth& tooth = status[crown[i]["idx"].asInt()];
		tooth.crown.set(true);
		tooth.crown.LPK = crown[i]["LPK"].asString();
		tooth.crown.data.material = crown[i]["material"].asString();
	}

	const Json::Value& bridge = json["Bridge"];

	for (int i = 0; i < bridge.size(); i++)
	{
		Tooth& tooth = status[bridge[i]["idx"].asInt()];
		tooth.bridge.set(true);
		tooth.bridge.LPK = bridge[i]["LPK"].asString();
		tooth.bridge.data.material = bridge[i]["material"].asString();
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

}