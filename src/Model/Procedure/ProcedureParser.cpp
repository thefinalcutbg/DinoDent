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
		auto& r = std::get<ProcedureObtData>(m.result);
		procedure["color"] = r.data.color;
		procedure["post"] = r.post;
		procedure["material"] = r.data.material;
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
		auto& r = std::get<ProcedureBridgeData>(m.result);
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
			m.result = ProcedureObtData
			{
				std::array<bool, 6>{false},
				procedure["post"].asBool(),
				ObturationData
				{
					procedure["color"].asInt(),
					procedure["material"].asString()

				}
			};

			const Json::Value& surfaces = procedure["surfaces"];
			
			for (int i = 0; i < surfaces.size(); i++)
				std::get<ProcedureObtData>(m.result).surfaces[surfaces[i].asInt()] = true;


			break;
		}

		case ProcedureType::crown:
		{
			m.result = CrownData
			{
				procedure["material"].asString(),
				procedure["prep"].asInt(),
				procedure["color_idx"].asInt()
			};

			break;
		}

		case ProcedureType::bridge:
		{
			m.result = ProcedureBridgeData
			{
				procedure["begin"].asInt(),
				procedure["end"].asInt(),
				CrownData
				{
					procedure["material"].asString(),
					procedure["prep"].asInt(),
					procedure["color_idx"].asInt()
				}

			};

			break;
		}

		case ProcedureType::implant:
		{
			m.result = ImplantData
			{
				procedure["system"].asString(),
				procedure["w"].asDouble(),
				procedure["l"].asDouble(),
				procedure["time"].asInt(),
				procedure["type"].asInt(),
				procedure["tissue"].asInt(),
				procedure["bone"].asInt(),
				procedure["membrane"].asBool(),
				procedure["sinus"].asBool()
			};

			break;
		}
		default:
			m.result = NoData{};
	}
}

#include "Database/DbProcedure.h"

void ProcedureParser::parse(const std::string& jsonString, DetailsSummary& s)
{
	Json::Value procedure;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, procedure);

	if (!parsingSuccessful) {
		return;
	}

	s.procedureDiagnosis = procedure["diagnosis"].asString();

	if (!procedure["name"].isNull())
	{
		s.procedureName = procedure["name"].asString();
	}

}
