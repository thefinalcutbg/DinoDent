#include "ProcedureApplier.h"

void ProcedureApplier::applyProcedures(const std::vector<Procedure>& m, std::array<Tooth, 32>& teeth, const std::string& LPK)
{

	for (const Procedure& m : m)
	{
		switch (m.type)
		{
			case::ProcedureType::obturation:
			{
				auto& tooth = teeth.at(m.tooth);
				obtur_ctrl.setTooth(&tooth);
				status_ctrl.setTooth(&tooth);
				caries_ctrl.setTooth(&tooth);
			
				auto& result = std::get<ObturationData>(m.result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					obtur_ctrl.addStatus(i);
					caries_ctrl.removeStatus(i);

					tooth.obturation[i].color = result.color;
					tooth.obturation[i].material = result.material;
					tooth.obturation[i].LPK = LPK;
				}

				if (result.post) status_ctrl.addStatus(StatusCode::Post);
				tooth.post.LPK = LPK;
			}
			break;

			case::ProcedureType::extraction:
			{
				auto& tooth = teeth.at(m.tooth);
				status_ctrl.setTooth(&tooth);
				status_ctrl.addStatus(StatusCode::Extraction);

				if(tooth.extraction.exists()) //if the tooth was temporary, the status won't be present
					tooth.extraction.LPK = LPK;
			}
			break;

			case::ProcedureType::endo:
			{
				auto& tooth = teeth.at(m.tooth);
				status_ctrl.setTooth(&tooth);
				status_ctrl.addStatus(StatusCode::EndoTreatment);
				tooth.endo.LPK = LPK;
			}
			break;

			case::ProcedureType::crown:
			{
				auto& result = std::get<CrownData>(m.result);

				auto& tooth = teeth.at(m.tooth);
				status_ctrl.setTooth(&tooth);
				status_ctrl.addStatus(StatusCode::Crown);
				
				tooth.crown.material = result.material;
				tooth.crown.prep_type = result.prep_type;
				tooth.crown.color = result.color;
				tooth.crown.LPK = LPK;
			}
			break;

			case::ProcedureType::implant:
			{
				auto& result = std::get<ImplantData>(m.result);

				auto& tooth = teeth.at(m.tooth);
				status_ctrl.setTooth(&tooth);
				status_ctrl.addStatus(StatusCode::Implant);
				

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
				auto& result = std::get<BridgeData>(m.result);
				
				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++)
					indexes.push_back(i);

				for (int i : indexes)
					bridge_ctrl.removeBridge(i, &teeth);

				for (int i : indexes)
				{
					auto& tooth = teeth.at(i);
					status_ctrl.setTooth(&tooth);
					status_ctrl.addStatus(StatusCode::Bridge);
					tooth.bridge.color = result.crown.color;
					tooth.bridge.material = result.crown.material;
					tooth.bridge.prep_type = result.crown.prep_type;
					tooth.bridge.LPK = LPK;
				}

				bridge_ctrl.formatBridges(indexes, &teeth);
			}
			break;

			default: 
				break;
		}
	}

}
