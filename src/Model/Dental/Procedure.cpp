#include "Procedure.h"
#include "Model/Dental/ToothContainer.h"
#include <qdebug.h>
void Procedure::applyProcedure(ToothContainer& teeth) const
{
		switch (code.type())
		{
			case::ProcedureType::obturation:
			{
				if (hyperdontic) return;

				auto& tooth = teeth[this->tooth];
				auto& result = std::get<ProcedureObtData>(this->result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					tooth.setStatus(StatusType::obturation, i, true);
					tooth.setStatus(StatusType::caries, i, false);
					
					//tooth.obturation[i].data = result.data;
					tooth.obturation[i].LPK = LPK;
					

				}

				if (result.post) {
					tooth.addStatus(StatusCode::Post);
					//tooth.post.data.type = result.post.value().type;
					tooth.post.LPK = LPK;
				}
			}
			break;

			case::ProcedureType::endo:
			{
				if (hyperdontic) return;
				teeth.setStatus({ this->tooth }, StatusCode::EndoTreatment, true);
				teeth[this->tooth].endo.LPK = LPK;

			}
			break;

			case::ProcedureType::extraction:
			{
				auto& tooth = teeth[this->tooth];
				if (hyperdontic) {
					tooth.removeStatus(StatusCode::Dsn);
					return;
				}
				
				teeth.setStatus({ this->tooth }, StatusCode::Implant, false);
				teeth.setStatus({this->tooth}, StatusCode::Extraction, true);
				
				if (tooth.extraction) //if the tooth was temporary or hyperdontic, the status won't be present
				{
					tooth.extraction.LPK = LPK;
				}
			}
			break;

			case::ProcedureType::crown:
			{
				if (hyperdontic) return;

				auto& tooth = teeth[this->tooth];
				//auto& result = std::get<CrownData>(this->result);

				tooth.setStatus(StatusType::general, StatusCode::Crown);

				//tooth.crown.data = result;
				tooth.crown.LPK = LPK;

			}
			break;

			case::ProcedureType::implant:
			{
				if (hyperdontic) return;

				auto& tooth = teeth[this->tooth];

				tooth.addStatus(StatusCode::Implant);

				tooth.implant.LPK = LPK;

			}
			break;

			case::ProcedureType::bridge:
			{
				auto& result = std::get<ConstructionRange>(this->result);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++)indexes.push_back(i);

				//teeth.removeBridgeOrSplint(indexes);
				teeth.setStatus(indexes, StatusCode::Bridge, true);

				for (int i : indexes) teeth[i].bridge.LPK = LPK;

			}
			break;

			case ProcedureType::fibersplint:
			{
				auto& result = std::get<ConstructionRange>(this->result);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++) indexes.push_back(i);

				//teeth.removeBridgeOrSplint(indexes);
				teeth.setStatus(indexes, StatusCode::FiberSplint, true);

				for (int i : indexes) teeth[i].splint.LPK = LPK;

			}
			break;
			case ProcedureType::denture:
			{
				auto& result = std::get<ConstructionRange>(this->result);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++) {
					
					if (
						teeth[i].extraction ||
						teeth[i].root ||
						teeth[i].implant ||
						teeth[i].impacted
					) 
					{
						indexes.push_back(i);
					}
				}

				//teeth.removeBridgeOrSplint(indexes);
				teeth.setStatus(indexes, StatusCode::Denture, true);

				for (int i : indexes) teeth[i].splint.LPK = LPK;
			}
			break;
			case ProcedureType::removeCrown:
			{
				if (hyperdontic) return;

				auto& tooth = teeth[this->tooth];
				tooth.removeStatus(StatusCode::Crown);
			}
			break;

			case ProcedureType::removePost:
			{
				if (hyperdontic) return;

				auto& tooth = teeth[this->tooth];
				tooth.removeStatus(StatusCode::Post);
			}
			break;

			case ProcedureType::deputatio:
			{
				for (auto& t : teeth)
				{
					t.calculus.set(false);
				}
			}

/*
			case ProcedureType::removebridgeOrSplint:
			{
				auto [begin, end] = std::get<ConstructionRange>(result);

				teeth.removeBridgeOrSplint(begin, end);
			}
			break;
			*/
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

	switch (code.type())
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

bool Procedure::isToothSpecific() const
{
	auto type = this->code.type();

	return
		type != ProcedureType::general &&
		type != ProcedureType::bridge &&
		type != ProcedureType::fibersplint &&
		type != ProcedureType::denture
		// && type != ProcedureType::removebridgeOrSplint
	;
}
