#include "ManipulationParser.h"

std::string ManipulationParser::write(Manipulation m)
{
	Json::Value manipulation;

	manipulation["diagnosis"] = m.diagnosis;
	manipulation["name"] = m.name;

	switch (m.type)
	{
	case ManipulationType::obturation:
	{
		auto& r = std::get<ObtData>(m.result);
		manipulation["color"] = r.color;
		manipulation["post"] = r.post;
		manipulation["material"] = r.material;
		manipulation["surfaces"] = Json::Value(Json::arrayValue);

		for (int i = 0; i < r.surfaces.size(); i++)
			if (r.surfaces[i]) manipulation["surfaces"].append(i);

		break;
	}
	case ManipulationType::crown:
	{
		auto& r = std::get<CrownData>(m.result);
		manipulation["color_idx"] = r.color.index;
		manipulation["3dMaster"] = r.color.Vita3dMaster;
		manipulation["material"] = r.material;
		manipulation["prep"] = r.prep_type;
		break;
	}
	case ManipulationType::bridge:
	{
		auto& r = std::get<BridgeData>(m.result);
		manipulation["color_idx"] = r.color.index;
		manipulation["3dMaster"] = r.color.Vita3dMaster;
		manipulation["material"] = r.material;
		manipulation["prep"] = r.prep_type;
		manipulation["begin"] = r.tooth_begin;
		manipulation["end"] = r.tooth_end;
		break;
	}

	case ManipulationType::implant:
	{
		auto& r = std::get<ImplData>(m.result);
		
		break;
	}

	default:
		break;
	}


	return writer.write(manipulation);
}

#include <QDebug>

void ManipulationParser::parse(const std::string& jsonString, Manipulation& m)
{
	
	Json::Value manipulation;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, manipulation);

	if (!parsingSuccessful) {
		return;
	}

	m.diagnosis = manipulation["diagnosis"].asString();

	if (!manipulation["name"].isNull())
	{
		m.name = manipulation["name"].asString();
	}
	
	
	switch (m.type)
	{
		case ManipulationType::obturation:
		{
			ObtData r;
			r.material = manipulation["material"].asString();
			r.post = manipulation["post"].asBool();
			r.color = manipulation["color"].asInt();
		
			const Json::Value& surfaces = manipulation["surfaces"];

			for (int i = 0; i < surfaces.size(); i++)
			{
				r.surfaces[surfaces[i].asInt()] = true;
			}

			m.result = r;

			break;
		}

		case ManipulationType::crown:
		{
			CrownData r;
			r.material = manipulation["material"].asString();
			r.prep_type = manipulation["prep"].asInt();
			r.color.index = manipulation["color_idx"].asInt();
			r.color.Vita3dMaster = manipulation["3dMaster"].asBool();

			m.result = r;

			break;
		}

		case ManipulationType::bridge:
		{
			BridgeData r;
			r.tooth_begin = manipulation["begin"].asInt();
			r.tooth_end = manipulation["end"].asInt();
			r.prep_type = manipulation["prep"].asInt();
			r.material = manipulation["material"].asString();
			r.color.index = manipulation["color_idx"].asInt();
			r.color.Vita3dMaster = manipulation["3dMaster"].asBool();

			m.result = r;

			break;
		}

		case ManipulationType::implant:
		{
			ImplData r;

			//some fancy parsing here

			m.result = r;

			break;
		}
		default:
			NoData r;
			m.result = r;
	}
}
