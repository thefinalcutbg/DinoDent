#include "Procedure.h"
#include "Model/Dental/ToothContainer.h"

void Procedure::applyProcedure(ToothContainer& teeth) const
{
		switch (code.type())
		{
			case::ProcedureType::obturation:
			{
				
				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];
				auto& result = std::get<ProcedureObtData>(this->result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					teeth.setStatus({ this->tooth }, StatusType::obturation, i, true, hyperdontic);
					teeth.setStatus({ this->tooth }, StatusType::caries, i, false, hyperdontic);
					
					//tooth.obturation[i].data = result.data;
					tooth.obturation[i].LPK = LPK;
					

				}

				if (result.post) {
					teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::Post, true, hyperdontic);
					tooth.post.LPK = LPK;
				}
			}
			break;

			case::ProcedureType::endo:
			{
				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];

				tooth.setStatus(StatusCode::EndoTreatment);
				teeth[this->tooth].endo.LPK = LPK;

			}
			break;

			case::ProcedureType::extraction:
			{
				teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::Crown, true, hyperdontic);

				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];
				
				tooth.extraction.LPK = LPK;

			}
			break;

			case::ProcedureType::crown:
			{
				teeth.setStatus({this->tooth}, StatusType::general, StatusCode::Crown, true, hyperdontic);
				
				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];
				tooth.crown.LPK = LPK;

			}
			break;

			case::ProcedureType::implant:
			{
				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];

				tooth.setStatus(StatusCode::Implant);

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
				teeth.setStatus(indexes, StatusType::general, StatusCode::Bridge, true);

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
				teeth.setStatus(indexes, StatusType::general, StatusCode::FiberSplint, true);

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
				teeth.setStatus(indexes, StatusType::general, StatusCode::Denture, true);

				for (int i : indexes) teeth[i].splint.LPK = LPK;
			}
			break;

			case ProcedureType::removeCrown:
			{
				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];
				tooth.removeStatus(StatusCode::Crown);
			}
			break;

			case ProcedureType::removePost:
			{
				auto& tooth = hyperdontic ? teeth[this->tooth].dsn.tooth() : teeth[this->tooth];
				tooth.removeStatus(StatusCode::Post);
			}
			break;

			case ProcedureType::deputatio:
			{
				for (auto& t : teeth)
				{
					t.setStatus(StatusCode::Calculus, false);
					if (t.dsn) t.dsn.tooth().setStatus(StatusCode::Calculus, false);
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
#include <qdebug.h>
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

	auto& tooth = hyperdontic ? teeth.at(idx).dsn.tooth() : teeth[idx];

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
			teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::Obturation, true, hyperdontic); break;
		case ProcedureType::endo:
			teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::EndoTreatment, true, hyperdontic); break;
		case ProcedureType::extraction:
			teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::Extraction, true, hyperdontic); break;
		case ProcedureType::crown:
			teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::Crown, true, hyperdontic); break;
		case ProcedureType::implant:
			teeth.setStatus({ this->tooth }, StatusType::general, StatusCode::Implant, true, hyperdontic); break;
	}

	
}

bool Procedure::isToothSpecific() const
{
	auto type = this->code.type();

	return
		type != ProcedureType::general &&
		type != ProcedureType::bridge &&
		type != ProcedureType::fibersplint &&
		type != ProcedureType::denture &&
		type != ProcedureType::deputatio
		// && type != ProcedureType::removebridgeOrSplint
	;
}
