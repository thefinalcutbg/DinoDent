#include "ProcedureParser.h"

std::string ProcedureParser::write(const Procedure& m)
{
	Json::Value procedure;

	procedure["diagnosis"] = m.diagnosis;
	procedure["name"] = m.name;

	switch (m.type)
	{
	case ProcedureType::obturation:
	{
		auto& r = std::get<PObturationData>(m.result);
		procedure["color"] = r.color;
		procedure["post"] = r.post;
		procedure["material"] = r.material;
		procedure["surfaces"] = Json::Value(Json::arrayValue);

		for (int i = 0; i < r.surfaces.size(); i++)
			if (r.surfaces[i]) procedure["surfaces"].append(i);

		break;
	}
	case ProcedureType::crown:
	{
		auto& r = std::get<CrownData>(m.result);
		procedure["color_idx"] = r.color;
		procedure["material"] = r.material;
		procedure["prep"] = r.prep_type;
		break;
	}
	case ProcedureType::bridge:
	{
		auto& r = std::get<BridgeData>(m.result);
		procedure["color_idx"] = r.crown.color;
		procedure["material"] = r.crown.material;
		procedure["prep"] = r.crown.prep_type;
		procedure["begin"] = r.tooth_begin;
		procedure["end"] = r.tooth_end;
		break;
	}

	case ProcedureType::implant:
	{
		auto& r = std::get<ImplantData>(m.result);
		
		procedure["system"] = r.system;
		procedure["time"] = r.time;
		procedure["type"] = r.type;
		procedure["w"] = r.width;
		procedure["l"] = r.length;
		procedure["tissue"] = r.tissue_aug;
		procedure["bone"] = r.bone_aug;
		procedure["membrane"] = r.membrane;
		procedure["sinus"] = r.sinusLift;
		
		break;
	}

	default:
		break;
	}


	return writer.write(procedure);
}

void ProcedureParser::parse(const std::string& jsonString, Procedure& m)
{
	
	Json::Value procedure;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, procedure);

	if (!parsingSuccessful) {
		return;
	}

	m.diagnosis = procedure["diagnosis"].asString();

	if (!procedure["name"].isNull())
	{
		m.name = procedure["name"].asString();
	}
	
	
	switch (m.type)
	{
		case ProcedureType::obturation:
		{
			PObturationData r;
			r.material = procedure["material"].asString();
			r.post = procedure["post"].asBool();
			r.color = procedure["color"].asInt();
		
			const Json::Value& surfaces = procedure["surfaces"];

			for (int i = 0; i < surfaces.size(); i++)
			{
				r.surfaces[surfaces[i].asInt()] = true;
			}

			m.result = r;

			break;
		}

		case ProcedureType::crown:
		{
			CrownData r;
			r.material = procedure["material"].asString();
			r.prep_type = procedure["prep"].asInt();
			r.color = procedure["color_idx"].asInt();

			m.result = r;

			break;
		}

		case ProcedureType::bridge:
		{
			BridgeData r;
			r.tooth_begin = procedure["begin"].asInt();
			r.tooth_end = procedure["end"].asInt();
			r.crown.prep_type = procedure["prep"].asInt();
			r.crown.material = procedure["material"].asString();
			r.crown.color = procedure["color_idx"].asInt();

			m.result = r;

			break;
		}

		case ProcedureType::implant:
		{
			ImplantData r;

			r.system = procedure["system"].asString();
			r.time = procedure["time"].asInt();
			r.type = procedure["type"].asInt();
			r.width = procedure["w"].asDouble();
			r.length = procedure["l"].asDouble();
			r.tissue_aug = procedure["tissue"].asInt();
			r.bone_aug = procedure["bone"].asInt();
			r.membrane = procedure["membrane"].asBool();
			r.sinusLift = procedure["sinus"].asBool();

			m.result = r;

			break;
		}
		default:
			NoData r;
			m.result = r;
	}
}
