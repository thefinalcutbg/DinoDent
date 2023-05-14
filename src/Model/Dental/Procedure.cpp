#include "Procedure.h"
#include "Model/Dental/ToothContainer.h"

void Procedure::applyProcedure(ToothContainer& teeth) const
{
		switch (code.type())
		{
			case::ProcedureType::obturation:
			{
				
				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];
				auto& result = std::get<RestorationData>(this->result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					teeth.setStatus({ tooth_idx.index }, StatusType::obturation, i, true, tooth_idx.supernumeral);
					teeth.setStatus({ tooth_idx.index }, StatusType::caries, i, false, tooth_idx.supernumeral);
					
					//tooth.obturation[i].data = result.data;
					tooth.obturation[i].LPK = LPK;
					

				}

				if (result.post) {
					teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Post, true, tooth_idx.supernumeral);
					tooth.post.LPK = LPK;
				}
			}
			break;

			case::ProcedureType::endo:
			{
				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];

				tooth.setStatus(StatusCode::EndoTreatment);
				teeth[tooth_idx.index].endo.LPK = LPK;

			}
			break;

			case::ProcedureType::extraction:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Extraction, true, tooth_idx.supernumeral);

				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];
				
				tooth.extraction.LPK = LPK;

			}
			break;

			case::ProcedureType::crown:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Crown, true, tooth_idx.supernumeral);
				
				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];
				tooth.crown.LPK = LPK;

			}
			break;

			case::ProcedureType::implant:
			{
				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];

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
				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];
				tooth.removeStatus(StatusCode::Crown);
			}
			break;

			case ProcedureType::removePost:
			{
				auto& tooth = tooth_idx.supernumeral ? teeth[tooth_idx.index].dsn.tooth() : teeth[tooth_idx.index];
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

	auto idx = tooth_idx.index;

	if (idx == -1){
		return;
	}

	auto& tooth = tooth_idx.supernumeral ? teeth.at(idx).dsn.tooth() : teeth[idx];

	if (tooth_idx.temp) {
		tooth.setStatus(StatusCode::Temporary);
	}
	else if (tooth.temporary.exists())
	{
		tooth.removeStatus();
	}

	switch (code.type())
	{
		case ProcedureType::obturation:
			teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Obturation, true, tooth_idx.supernumeral); break;
		case ProcedureType::endo:
			teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::EndoTreatment, true, tooth_idx.supernumeral); break;
		case ProcedureType::extraction:
			teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Extraction, true, tooth_idx.supernumeral); break;
		case ProcedureType::crown:
			teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Crown, true, tooth_idx.supernumeral); break;
		case ProcedureType::implant:
			teeth.setStatus({ tooth_idx.index }, StatusType::general, StatusCode::Implant, true, tooth_idx.supernumeral); break;
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

std::string Procedure::getToothString() const
{
	if (tooth_idx.isValid()) 
		return tooth_idx.getNhifNumenclature();

	if (std::holds_alternative<ConstructionRange>(result)) {
		auto& [from, to] = std::get<ConstructionRange>(result);
		return ToothUtils::getNomenclature(from, false) + "-" + ToothUtils::getNomenclature(to, false);
	}

	return std::string{};
}
