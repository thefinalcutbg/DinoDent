#include "Procedure.h"
#include "Model/Tooth/ToothContainer.h"
#include <QDebug>
void Procedure::applyProcedure(ToothContainer& teeth)
{


		switch (type)
		{
			case::ProcedureType::obturation:
			{
				auto& tooth = teeth[this->tooth];
				auto& result = std::get<ProcedureObtData>(this->result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					tooth.setStatus(StatusType::obturation, i, true);
					tooth.setStatus(StatusType::caries, i, false);

					tooth.obturation[i].data.color = result.data.color;
					tooth.obturation[i].data.material = result.data.material;
					tooth.obturation[i].LPK = LPK;
				}

				if (result.post) tooth.setStatus(StatusType::general, StatusCode::Post, true);
				tooth.post.LPK = LPK;
			}
			break;

			case::ProcedureType::extraction:
			{
				auto& tooth = teeth[this->tooth];
				tooth.setStatus(StatusType::general, StatusCode::Extraction);

				if (tooth.extraction.exists()) //if the tooth was temporary or hyperdontic, the status won't be present
						tooth.extraction.LPK = LPK;
			}
			break;

			case::ProcedureType::endo:
			{
				auto& tooth = teeth[this->tooth];
				tooth.setStatus(StatusType::general, StatusCode::EndoTreatment);
				tooth.endo.LPK = LPK;
			}
			break;

		case::ProcedureType::crown:
		{
			auto& tooth = teeth[this->tooth];
			auto& result = std::get<CrownData>(this->result);

			tooth.setStatus(StatusType::general, StatusCode::Crown);

			tooth.crown.data.material = result.material;
			tooth.crown.data.prep_type = result.prep_type;
			tooth.crown.data.color = result.color;
			tooth.crown.LPK = LPK;
		}
		break;

		case::ProcedureType::implant:
		{
			auto& tooth = teeth[this->tooth];
			auto& result = std::get<ImplantData>(this->result);

			tooth.setStatus(StatusType::general, StatusCode::Implant);

			auto& implant = tooth.implant;
			implant.LPK = LPK;
			implant.data.system = result.system;
			implant.data.time = result.time;
			implant.data.type = result.type;
			implant.data.width = result.width;
			implant.data.length = result.length;
			implant.data.tissue_aug = result.tissue_aug;
			implant.data.bone_aug = result.bone_aug;
			implant.data.membrane = result.membrane;
			implant.data.sinusLift = result.sinusLift;
		}
		break;

		case::ProcedureType::bridge:
		{
			auto& result = std::get<ProcedureBridgeData>(this->result);

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
				tooth.bridge.data.color = result.crown.color;
				tooth.bridge.data.material = result.crown.material;
				tooth.bridge.data.prep_type = result.crown.prep_type;
				tooth.bridge.LPK = LPK;
	
			}

			teeth.formatBridges(indexes);
		}
		break;

		case ProcedureType::fibersplint:
		{
			auto& result = std::get<ProcedureFiberData>(this->result);

			std::vector<int> indexes;
			indexes.reserve(result.tooth_end - result.tooth_begin + 1);

			for (int i = result.tooth_begin; i <= result.tooth_end; i++)
				indexes.push_back(i);

			for (int i : indexes)
				teeth.removeBridge(i);

			for (int i : indexes)
			{
				auto& tooth = teeth[i];

				tooth.setStatus(StatusType::general, StatusCode::FiberSplint);
				tooth.splint.color = result.obtur.color;
				tooth.splint.material = result.obtur.material;
				tooth.splint.LPK = LPK;

			}

			teeth.formatBridges(indexes);
		}
		break;

		case ProcedureType::removecrown:
		{
			auto& tooth = teeth[this->tooth];
			tooth.removeStatus(StatusCode::Crown);
		}
		break;

		default:
			break;
		}
	
}