#include "ToothParser.h"
#include "ToothContainer.h"

Json::Value ToothParser::writePathology(int index, const Pathology& pathology)
{
	Json::Value parameters;
	parameters["diag_idx"] = pathology.data.diagnosis_index;
	parameters["date"] = Date::toString(pathology.data.date_diagnosed);
	parameters["idx"] = index;
	return parameters;
}

Json::Value ToothParser::writeDentistMade(int index, const DentistMade& procedure)
{
	Json::Value parameters;
	parameters["LPK"] = procedure.LPK;
	parameters["idx"] = index;
	return parameters; 
}

std::string ToothParser::write(const ToothContainer& teeth)
{
	Json::Value status;

	for (int i = 0; i < 32; i++)
	{
		const Tooth& tooth = teeth[i];

				
				if (tooth.temporary.exists())
				{
					if (!status.isMember("Temporary"))
					{
						status["Temporary"] = Json::Value(Json::arrayValue);
					}
					Json::Value parameters;
					parameters["idx"] = i;
					status["Temporary"].append(parameters);
				}

				if (tooth.hyperdontic.exists())
				{
					if (!status.isMember("Hyperdontic"))
					{
						status["Hyperdontic"] = Json::Value(Json::arrayValue);
					}
					Json::Value parameters;
					parameters["idx"] = i;
					status["Hyperdontic"].append(parameters);
				}

				if (tooth.mobility.exists())
				{
					if (!status.isMember("Mobility"))
					{
						status["Mobility"] = Json::Value(Json::arrayValue);
					}
					Json::Value parameters;
					parameters["idx"] = i;
					parameters["degree"] = static_cast<int>(tooth.mobility.degree);
					status["Mobility"].append(parameters);
				}

				if (tooth.periodontitis.exists())
				{
					if (!status.isMember("Periodontitis"))
					{
						status["Periodontitis"] = Json::Value(Json::arrayValue);
					}
					Json::Value parameters;
					parameters["idx"] = i;
					status["Periodontitis"].append(parameters);
				}

				if (tooth.obturation.exists())
				{
					if (!status.isMember("Obturation"))
					{
						status["Obturation"] = Json::Value(Json::arrayValue);
					}
					for (int y = 0; y < 6; y++)
					{
						if (tooth.obturation.exists(y)) {

							auto parameters = writeDentistMade(i, tooth.obturation[y]);
							parameters["Surface"] = y;
							parameters["color"] = tooth.obturation[y].data.color;
							parameters["material"] = tooth.obturation[y].data.material;
							status["Obturation"].append(parameters);
						}
					}
				}


				if (tooth.caries.exists())
				{
					if (!status.isMember("Caries"))
					{
						status["Caries"] = Json::Value(Json::arrayValue);
					}
					for (int y = 0; y < 6; y++)
					{
						if (tooth.caries.exists(y)) {

							auto parameters = writePathology(i, tooth.caries[y]);
							parameters["Surface"] = y;
							status["Caries"].append(parameters);
						}

					}
				}

				if (tooth.pulpitis.exists())
				{
					if (!status.isMember("Pulpitis"))
					{
						status["Pulpitis"] = Json::Value(Json::arrayValue);
					}
					status["Pulpitis"].append(writePathology(i, tooth.pulpitis));
				}

				if (tooth.lesion.exists())
				{
					if (!status.isMember("Lesion"))
					{
						status["Lesion"] = Json::Value(Json::arrayValue);
					}

					status["Lesion"].append(writePathology(i, tooth.lesion));
				}

				if (tooth.endo.exists()) 
				{
					if (!status.isMember("EndoTreatment"))
					{
						status["EndoTreatment"] = Json::Value(Json::arrayValue);
					}

					status["EndoTreatment"].append(writeDentistMade(i, tooth.endo));
				}

				if (tooth.post.exists())
				{
					if (!status.isMember("Post"))
					{
						status["Post"] = Json::Value(Json::arrayValue);
					}

					status["Post"].append(writeDentistMade(i, tooth.post));
				}


				if (tooth.extraction.exists()) 
				{

					if (!status.isMember("Extraction"))
					{
						status["Extraction"] = Json::Value(Json::arrayValue);
					}

					status["Extraction"].append(writeDentistMade(i, tooth.extraction));
				}

				if (tooth.root.exists()) 
				{
					if (!status.isMember("Root"))
					{
						status["Root"] = Json::Value(Json::arrayValue);
					}

					status["Root"].append(writePathology(i, tooth.root));
				}


				if (tooth.fracture.exists())
				{
					if (!status.isMember("Fracture"))
					{
						status["Fracture"] = Json::Value(Json::arrayValue);
					}
					status["Fracture"].append(writePathology(i, tooth.fracture));
				}

				if (tooth.crown.exists())
				{
					if (!status.isMember("Crown"))
					{
						status["Crown"] = Json::Value(Json::arrayValue);
					}
					auto param = writeDentistMade(i, tooth.crown);
					param["material"] = tooth.crown.data.material;
					param["prep"] = tooth.crown.data.prep_type;
					param["color"] = tooth.crown.data.color;
					status["Crown"].append(param);
				}

				if (tooth.implant.exists())
				{
					if (!status.isMember("Implant"))
					{
						status["Implant"] = Json::Value(Json::arrayValue);
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
					status["Implant"].append(param);
				}

				if (tooth.bridge.exists())
				{
					if (!status.isMember("Bridge"))
					{
						status["Bridge"] = Json::Value(Json::arrayValue);
					}
					auto param = writeDentistMade(i, tooth.bridge);
					param["pos"] = static_cast<int>(tooth.bridge.position);
					param["material"] = tooth.bridge.data.material;
					param["prep"] = tooth.bridge.data.prep_type;
					param["color"] = tooth.bridge.data.color;
					status["Bridge"].append(param);
				}
	}

	return writer.write(status);
}

void ToothParser::parse(const std::string& jsonString, ToothContainer& teeth)
{
	Json::Value status;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, status);

	if (!parsingSuccessful) {
		 return;
	}

	const Json::Value& temporary = status["Temporary"];

	for (int i = 0; i < temporary.size(); i++)
	{
		Tooth& tooth = teeth[temporary[i]["idx"].asInt()];
		tooth.temporary.set(true);
	}

	const Json::Value& hyperdontic = status["Hyperdontic"];

	for (int i = 0; i < hyperdontic.size(); i++)
	{
		Tooth& tooth = teeth[hyperdontic[i]["idx"].asInt()];
		tooth.hyperdontic.set(true);
	}

	const Json::Value& mobility = status["Mobility"];

	for (int i = 0; i < mobility.size(); i++)
	{
		Tooth& tooth = teeth[mobility[i]["idx"].asInt()];
		tooth.mobility.set(true);
		tooth.mobility.degree = static_cast<Degree>(mobility[i]["degree"].asInt());
	}

	const Json::Value& periodontitis = status["Periodontitis"];

	for (int i = 0; i < periodontitis.size(); i++)
	{
		Tooth& tooth = teeth[periodontitis[i]["idx"].asInt()];
		tooth.periodontitis.set(true);
	}


	const Json::Value& obturation = status["Obturation"];

	for (int i = 0; i < obturation.size(); i++)
	{
		Tooth& tooth = teeth[obturation[i]["idx"].asInt()];
		int surface = obturation[i]["Surface"].asInt();
		tooth.obturation.set(true, surface);
		tooth.obturation[surface].LPK = obturation[i]["LPK"].asString();
		tooth.obturation[surface].data.material = obturation[i]["material"].asString();
		tooth.obturation[surface].data.color = obturation[i]["color"].asInt();
	}

	const Json::Value& car = status["Caries"];

	for (int i = 0; i < car.size(); i++)
	{
		Tooth& tooth = teeth[car[i]["idx"].asInt()];
		int surface = car[i]["Surface"].asInt();
		tooth.caries.set(true, surface);
		tooth.caries[surface].data.diagnosis_index = car[i]["diag_idx"].asInt();
		tooth.caries[surface].data.date_diagnosed = car[i]["date"].asString();
	}

	const Json::Value& pulpitis = status["Pulpitis"];

	for(int i = 0; i < pulpitis.size(); i++)
	{
		Tooth& tooth = teeth[pulpitis[i]["idx"].asInt()];
		tooth.pulpitis.set(true);

		tooth.pulpitis.data.diagnosis_index = pulpitis[i]["diag_idx"].asInt();
		tooth.pulpitis.data.date_diagnosed = pulpitis[i]["date"].asString();
	}

	const Json::Value& lesion = status["Lesion"];

	for (int i = 0; i < lesion.size(); i++)
	{
		Tooth& tooth = teeth[lesion[i]["idx"].asInt()];
		tooth.lesion.set(true);
		tooth.lesion.data.diagnosis_index = lesion[i]["diag_idx"].asInt();
		tooth.lesion.data.date_diagnosed = lesion[i]["date"].asString();
	}

	const Json::Value& endo = status["EndoTreatment"];

	for (int i = 0; i < endo.size(); i++)
	{
		Tooth& tooth = teeth[endo[i]["idx"].asInt()];
		tooth.endo.set(true);
		tooth.endo.LPK = endo[i]["LPK"].asString();
	}

	const Json::Value& post = status["Post"];

	for (int i = 0; i < post.size(); i++)
	{
		Tooth& tooth = teeth[post[i]["idx"].asInt()];
		tooth.post.set(true);
		tooth.post.LPK = post[i]["LPK"].asString();
	}

	const Json::Value &extraction = status["Extraction"];

	for (int i = 0; i < extraction.size(); i++) 
	{
		Tooth& tooth = teeth[extraction[i]["idx"].asInt()];
		tooth.extraction.set(true);
		tooth.extraction.LPK = extraction[i]["LPK"].asString();
	}

	const Json::Value &root = status["Root"];

	for (int i = 0; i < root.size(); i++)
	{

		Tooth& tooth = teeth[root[i]["idx"].asInt()];
		tooth.root.data.date_diagnosed = Date{root[i]["date"].asString()};
		tooth.root.data.diagnosis_index = root[i]["diag_idx"].asInt();
		tooth.root.set(true);
	}

	const Json::Value& implant = status["Implant"];

	for (int i = 0; i < implant.size(); i++)
	{
		Tooth& tooth = teeth[implant[i]["idx"].asInt()];
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

	const Json::Value& crown = status["Crown"];

	for (int i = 0; i < crown.size(); i++)
	{
		Tooth& tooth = teeth[crown[i]["idx"].asInt()];
		tooth.crown.set(true);
		tooth.crown.LPK = crown[i]["LPK"].asString();
		tooth.crown.data.material = crown[i]["material"].asString();
	}

	const Json::Value& bridge = status["Bridge"];

	for (int i = 0; i < bridge.size(); i++)
	{
		Tooth& tooth = teeth[bridge[i]["idx"].asInt()];
		tooth.bridge.set(true);
		tooth.bridge.LPK = bridge[i]["LPK"].asString();
		tooth.bridge.data.material = bridge[i]["material"].asString();
		tooth.bridge.position = static_cast<BridgePos>(bridge[i]["pos"].asInt());
	}

}

std::string NotesParser::write(const ToothContainer& teeth)
{
	std::string notesString;

	Json::Value notes;

	for (int i = 0; i < 32; i++)
	{
		const Tooth& tooth = teeth[i];

		if (!tooth.notes.empty())
		{
			if (!notes.isMember("Notes"))
				notes["Notes"] = Json::Value(Json::arrayValue);

			Json::Value parameters;
			parameters["idx"] = i;
			parameters["text"] = tooth.notes;
			notes["Notes"].append(parameters);
		}

	}
	
	return notesString;
}

void NotesParser::parse(const std::string& jsonString, ToothContainer& teeth)
{
	Json::Value notes;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, notes);

	if (!parsingSuccessful) {
		return;
	}

	const Json::Value& notes = notes["Notes"];

	for (int i = 0; i < notes.size(); i++)
	{
		Tooth& tooth = teeth[notes[i]["idx"].asInt()];
		tooth.notes = notes[i]["text"].asString();
	}

}
