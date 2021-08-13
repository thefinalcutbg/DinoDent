#include "Procedure.h"
#include "Model/Tooth/ToothContainer.h"

void Procedure::applyProcedure(ToothContainer& teeth)
{
		auto& tooth = teeth[this->tooth];

		switch (type)
		{
			case::ProcedureType::obturation:
			{
				auto& result = std::get<ObturationData>(this->result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					tooth.setStatus(StatusType::obturation, i, true);
					tooth.setStatus(StatusType::caries, i, false);

					tooth.obturation[i].color = result.color;
					tooth.obturation[i].material = result.material;
					tooth.obturation[i].LPK = LPK;
				}

				if (result.post) tooth.setStatus(StatusType::general, StatusCode::Post, true);
				tooth.post.LPK = LPK;
			}
			break;

			case::ProcedureType::extraction:
			{
				tooth.setStatus(StatusType::general, StatusCode::Extraction);

				if (tooth.extraction.exists()) //if the tooth was temporary or hyperdontic, the status won't be present
						tooth.extraction.LPK = LPK;
			}
			break;

			case::ProcedureType::endo:
			{
				tooth.setStatus(StatusType::general, StatusCode::EndoTreatment);
				tooth.endo.LPK = LPK;
			}
			break;

		case::ProcedureType::crown:
		{
			auto& result = std::get<CrownData>(this->result);

			tooth.setStatus(StatusType::general, StatusCode::Crown);

			tooth.crown.material = result.material;
			tooth.crown.prep_type = result.prep_type;
			tooth.crown.color = result.color;
			tooth.crown.LPK = LPK;
		}
		break;

		case::ProcedureType::implant:
		{
			auto& result = std::get<ImplantData>(this->result);

			tooth.setStatus(StatusType::general, StatusCode::Implant);

			auto& implant = tooth.implant;
			implant.LPK = LPK;
			implant.system = result.system;
			implant.time = result.time;
			implant.type = result.type;
			implant.width = result.width;
			implant.length = result.length;
			implant.tissue_aug = result.tissue_aug;
			implant.bone_aug = result.bone_aug;
			implant.membrane = result.membrane;
			implant.sinusLift = result.sinusLift;
		}
		break;

		case::ProcedureType::bridge:
		{
			auto& result = std::get<BridgeData>(this->result);

			std::vector<int> indexes;
			indexes.reserve(result.tooth_end - result.tooth_begin + 1);

			for (int i = result.tooth_begin; i <= result.tooth_end; i++)
				indexes.push_back(i);

			for (int i : indexes)
				teeth.removeBridge(i);

			for (int i : indexes)
			{
				auto& tooth = teeth[i];

				tooth.setStatus(StatusType::general, StatusCode::Bridge);
				tooth.bridge.color = result.crown.color;
				tooth.bridge.material = result.crown.material;
				tooth.bridge.prep_type = result.crown.prep_type;
				tooth.bridge.LPK = LPK;
			}

			teeth.formatBridges(indexes);
		}
		break;

		default:
			break;
		}
	
}