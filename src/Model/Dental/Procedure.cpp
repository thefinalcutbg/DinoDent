#include "Procedure.h"
#include "Model/Dental/ToothContainer.h"

void Procedure::applyProcedure(ToothContainer& teeth) const
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
					
					tooth.obturation[i].data = result.data;
					tooth.obturation[i].LPK = LPK;
					

				}

				if (result.post) {
					tooth.setStatus(StatusType::general, StatusCode::Post, true);
					tooth.post.data.type = result.post.value().type;
					tooth.post.LPK = LPK;
				}
			}
			break;

			case::ProcedureType::extraction:
			{
				auto& tooth = teeth[this->tooth];
				tooth.setStatus(StatusType::general, StatusCode::Extraction);

				if (!tooth.extraction.exists()) return; //if the tooth was temporary or hyperdontic, the status won't be present
						
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

			tooth.crown.data = result;
			tooth.crown.LPK = LPK;

		}
		break;

		case::ProcedureType::implant:
		{
			auto& tooth = teeth[this->tooth];
			auto& result = std::get<ImplantData>(this->result);

			tooth.setStatus(StatusType::general, StatusCode::Implant);

			tooth.implant.data = result;

			tooth.implant.LPK = LPK;

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
				tooth.bridge.data = result.crown;
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
				tooth.splint.data = result.obtur;

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

void Procedure::applyPISProcedure(ToothContainer& teeth) const
{
	/*
	
		This function is a simple status applier, not taking into
		consideration the result, since PIS doesn't store additional data
	
	*/

	auto idx = this->tooth;

	if (idx == -1){
		return;
	}

	auto& tooth = teeth[idx];

	if (temp) {
		tooth.setStatus(StatusCode::Temporary);
	}
	else if (tooth.temporary.exists())
	{
		tooth.removeStatus();
	}

	switch (type)
	{
	case ProcedureType::obturation:
		tooth.setStatus(StatusCode::Obturation); break;
	case ProcedureType::endo:
		tooth.setStatus(StatusCode::EndoTreatment); break;
	case ProcedureType::extraction:
		tooth.setStatus(StatusCode::Extraction); break;
	case ProcedureType::crown:
		tooth.setStatus(StatusCode::Crown); break;
	case ProcedureType::implant:
		tooth.setStatus(StatusCode::Implant); break;
	}

	
}
