#include "Procedure.h"
#include "ToothContainer.h"
#include "ToothUtils.h"

using namespace Dental;

void Procedure::applyProcedure(ToothContainer& teeth) const
{
		switch (code.type())
		{
			case::ProcedureType::obturation:
			{
				
				auto& result = std::get<RestorationData>(this->result);

				for (int i = 0; i < result.surfaces.size(); i++)
				{
					if (!result.surfaces[i]) continue;

					teeth.setStatus({ tooth_idx.index }, StatusType::Restoration, i, true, tooth_idx.supernumeral);
					teeth.setStatus({ tooth_idx.index }, StatusType::Caries, i, false, tooth_idx.supernumeral);
					
					teeth.at(tooth_idx).setLPK(i, LPK);
				}

				if (result.post) {
					teeth.setStatus({ tooth_idx.index }, StatusType::General, Post, true, tooth_idx.supernumeral);
					teeth.at(tooth_idx).setLPK(Post, LPK);
				}

				//fracture
				if (diagnosis.index() == 4) {
					teeth.setStatus({ tooth_idx.index }, StatusType::General, Fracture, false, tooth_idx.supernumeral);
				}
			}
			break;

			case::ProcedureType::endo:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, RootCanal, true, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(RootCanal, LPK);

			}
			break;

			case::ProcedureType::extraction:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Dental::Missing, true, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(Missing, LPK);

			}
			break;

			case::ProcedureType::crown:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Crown, true, tooth_idx.supernumeral);
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Fracture, false, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(Crown, LPK);

			}
			break;

			case::ProcedureType::implant:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Implant, true, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(Implant, LPK);

			}
			break;

			case::ProcedureType::bridge:
			{
				auto& result = std::get<ConstructionRange>(this->result);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++)indexes.push_back(i);

				teeth.setStatus(indexes, StatusType::General, Bridge, true);
				teeth.setStatus(indexes, StatusType::General, Fracture, false);
				for (int i : indexes) teeth.at(i).setLPK(Bridge, LPK);

			}
			break;

			case ProcedureType::fibersplint:
			{
				auto& result = std::get<ConstructionRange>(this->result);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++) indexes.push_back(i);

				//teeth.removeBridgeOrSplint(indexes);
				teeth.setStatus(indexes, Dental::StatusType::General, Dental::Splint, true);

				for (int i : indexes) teeth.at(i).setLPK(Splint, LPK);

			}
			break;
			case ProcedureType::denture:
			{
				auto& result = std::get<ConstructionRange>(this->result);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++) {

					if (
						( //is missing
						teeth[i].noData()	 ||
						teeth[i][Missing]	 ||
						teeth[i][Root]		 ||
						teeth[i][Implant] 	 ||
						teeth[i][Impacted]	 ||
						teeth[i][Denture]
						) && // and it's not already restored by
						!teeth[i][Crown]	 &&
						!teeth[i][Bridge]	 &&
						!teeth[i][Splint]
					) 
					{
						indexes.push_back(i);
					}
				}

				//teeth.removeBridgeOrSplint(indexes);
				teeth.setStatus(indexes, Dental::StatusType::General, Dental::Denture, true);

				for (int i : indexes) teeth[i].setLPK(Denture, LPK);
			}
			break;

			case ProcedureType::removeCrown:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Crown, false, tooth_idx.supernumeral);
			}
			break;

			case ProcedureType::removePost:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Post, false, tooth_idx.supernumeral);
			}
			break;

			case ProcedureType::depuratio:
			{
				for (auto& t : teeth)
				{
					t.setStatus(Dental::Calculus, false);
					if (t[HasSupernumeral]) t.getSupernumeral().setStatus(Dental::Calculus, false);
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

	auto idx = tooth_idx.index;

	if (idx == -1){
		return;
	}

	auto& tooth = tooth_idx.supernumeral ? teeth.at(idx).getSupernumeral(): teeth[idx];

	if (tooth_idx.temp) {
		tooth.setStatus(Dental::Temporary, true);
	}
	else if (tooth[Temporary])
	{
		tooth.clearStatuses();
	}

	switch (code.type())
	{
		case ProcedureType::obturation:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Restoration, true, tooth_idx.supernumeral); break;
		case ProcedureType::endo:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::RootCanal, true, tooth_idx.supernumeral); break;
		case ProcedureType::extraction:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Missing, true, tooth_idx.supernumeral); break;
		case ProcedureType::crown:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Crown, true, tooth_idx.supernumeral); break;
		case ProcedureType::implant:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Implant, true, tooth_idx.supernumeral); break;
		case ProcedureType::denture:
		{
			int dentureBegin = code.oldCode() == 832 ? 1 : 17;
			int dentureEnd = code.oldCode() == 832 ? 15 : 30;

			for (int i = dentureBegin; i <= dentureEnd; i++)
			{
				teeth[i].setStatus(Dental::StatusType::General, Dental::Denture, true);
			}
			break;
		}
		default: break;
	}

	
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
