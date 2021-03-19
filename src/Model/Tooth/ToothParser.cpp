#include "ToothParser.h"

ToothParser::ToothParser()
{

}

Json::Value ToothParser::writePathology(int index, const Pathology& pathology)
{
	Json::Value parameters;
	parameters["diagnosis"] = pathology.diagnosis;
	parameters["date"] = pathology.date_diagnosed;
	parameters["idx"] = index;
	return parameters;
}

Json::Value ToothParser::writeDentistMade(int index, const DentistMade& procedure)
{
	Json::Value parameters;
	parameters["LPK"] = procedure.LPK;
	parameters["material"] = procedure.material;
	parameters["idx"] = index;
	return parameters;
}

std::string ToothParser::write(const std::array<Tooth, 32>& teeth)
{
	Json::Value status;
	status["Temporary"] = Json::Value(Json::arrayValue);
	status["Hyperdontic"] = Json::Value(Json::arrayValue);
	status["Mobility"] = Json::Value(Json::arrayValue);
	status["Parodontitis"] = Json::Value(Json::arrayValue);
	status["Obturation"] = Json::Value(Json::arrayValue);
	status["Caries"] = Json::Value(Json::arrayValue);
	status["Pulpitis"] = Json::Value(Json::arrayValue);
	status["Lesion"] = Json::Value(Json::arrayValue);
	status["EndoTreatment"] = Json::Value(Json::arrayValue);
	status["Extraction"] = Json::Value(Json::arrayValue);
	status["Root"] = Json::Value(Json::arrayValue);
	status["Fracture"] = Json::Value(Json::arrayValue);
	status["Crown"] = Json::Value(Json::arrayValue);
	status["Implant"] = Json::Value(Json::arrayValue);
	status["Bridge"] = Json::Value(Json::arrayValue);

	for (int i = 0; i < teeth.size(); i++)
	{
		const Tooth& tooth = teeth.at(i);

				
				if (tooth.temporary.exists())
				{
					Json::Value parameters;
					parameters["idx"] = i;
					status["Temporary"].append(parameters);
				}

				if (tooth.hyperdontic.exists())
				{
					Json::Value parameters;
					parameters["idx"] = i;
					status["Hyperdontic"].append(parameters);
				}

				if (tooth.mobility.exists())
				{
					Json::Value parameters;
					parameters["idx"] = i;
					parameters["degree"] = static_cast<int>(tooth.mobility.degree);
					status["Mobility"].append(parameters);
				}

				if (tooth.periodontitis.exists())
				{
					Json::Value parameters;
					parameters["idx"] = i;
					status["Periodontitis"].append(parameters);
				}

				if (tooth.obturation)
				{
					for (int y = 0; y < 6; y++)
					{
						if (tooth.o_surf[y].exists()) {

							auto parameters = writeDentistMade(i, tooth.o_surf[y]);
							parameters["Surface"] = y;
							status["Obturation"].append(parameters);
						}
					}
				}


				if (tooth.caries)
				{
					for (int y = 0; y < 6; y++)
					{
						if (tooth.c_surf[y].exists()) {

							auto parameters = writePathology(i, tooth.c_surf[y]);
							parameters["Surface"] = y;
							status["Caries"].append(parameters);
						}

					}
				}

				if (tooth.pulpitis.exists()){
					status["Pulpitis"].append(writePathology(i, tooth.pulpitis));
				}

				if (tooth.lesion.exists()){
					status["Lesion"].append(writePathology(i, tooth.lesion));
				}

				if (tooth.endo.exists()) {
					status["EndoTreatment"].append(writeDentistMade(i, tooth.endo));
				}

				if (tooth.extraction.exists()) {
					status["Extraction"].append(writeDentistMade(i, tooth.extraction));
				}

				if (tooth.root.exists()) {
					status["Root"].append(writePathology(i, tooth.root));
				}


				if (tooth.fracture.exists()) {
					status["Fracture"].append(writePathology(i, tooth.fracture));
				}

				if (tooth.crown.exists()) {
					status["Crown"].append(writeDentistMade(i, tooth.crown));
				}

				if (tooth.implant.exists()) {
					status["Implant"].append(writeDentistMade(i, tooth.implant));
				}

				if (tooth.bridge.exists()) {
					auto param = writeDentistMade(i, tooth.bridge);
					param["pos"] = static_cast<int>(tooth.bridge.position);
					status["Bridge"].append(param);
				}

	}

	return writer.write(status);
}

void ToothParser::parse(const std::string& jsonString, std::array<Tooth, 32>& teeth)
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
		tooth.obturation = true;
		DentistMade& obtur = tooth.o_surf[obturation[i]["Surface"].asInt()];
		obtur.set(true);
		obtur.LPK = obturation[i]["LPK"].asString();
		obtur.material = obturation[i]["material"].asString();
	}

	const Json::Value& car = status["Caries"];

	for (int i = 0; i < car.size(); i++)
	{
		Tooth& tooth = teeth[car[i]["idx"].asInt()];
		tooth.caries = true;
		Pathology& caries = tooth.c_surf[car[i]["Surface"].asInt()];
		caries.set(true);
		caries.diagnosis = car[i]["diagnosis"].asString();
		caries.date_diagnosed = car[i]["date"].asString();
	}

	const Json::Value& pulpitis = status["Pulpitis"];

	for(int i = 0; i < pulpitis.size(); i++)
	{
		Tooth& tooth = teeth[pulpitis[i]["idx"].asInt()];
		tooth.pulpitis.set(true);
		tooth.pulpitis.diagnosis = pulpitis[i]["diagnosis"].asString();
		tooth.pulpitis.date_diagnosed = pulpitis[i]["date"].asString();
	}

	const Json::Value& lesion = status["Lesion"];

	for (int i = 0; i < lesion.size(); i++)
	{
		Tooth& tooth = teeth[lesion[i]["idx"].asInt()];
		tooth.lesion.set(true);
		tooth.lesion.diagnosis = lesion[i]["diagnosis"].asString();
		tooth.lesion.date_diagnosed = lesion[i]["date"].asString();
	}

	const Json::Value& endo = status["EndoTreatment"];

	for (int i = 0; i < endo.size(); i++)
	{
		Tooth& tooth = teeth[endo[i]["idx"].asInt()];
		tooth.endo.set(true);
		tooth.endo.LPK = endo[i]["LPK"].asString();
		tooth.endo.material = endo[i]["LPK"].asString();
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
		tooth.root.date_diagnosed = status[i]["date"].asString();
		tooth.root.diagnosis = status[i]["diagnosis"].asString();
		tooth.root.set(true);
	}

	const Json::Value& implant = status["Implant"];

	for (int i = 0; i < implant.size(); i++)
	{
		Tooth& tooth = teeth[implant[i]["idx"].asInt()];
		tooth.implant.set(true);
		tooth.implant.LPK = implant[i]["LPK"].asString();
		tooth.implant.material = implant[i]["material"].asString();
	}

	const Json::Value& crown = status["Crown"];

	for (int i = 0; i < crown.size(); i++)
	{
		Tooth& tooth = teeth[crown[i]["idx"].asInt()];
		tooth.crown.set(true);
		tooth.crown.LPK = crown[i]["LPK"].asString();
		tooth.crown.material = crown[i]["material"].asString();
	}

	const Json::Value& bridge = status["Bridge"];

	for (int i = 0; i < bridge.size(); i++)
	{
		Tooth& tooth = teeth[bridge[i]["idx"].asInt()];
		tooth.bridge.set(true);
		tooth.bridge.LPK = bridge[i]["LPK"].asString();
		tooth.bridge.material = bridge[i]["material"].asString();
		tooth.bridge.position = static_cast<BridgePos>(bridge[i]["pos"].asInt());
	}
}