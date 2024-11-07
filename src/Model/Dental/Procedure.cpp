#include "Procedure.h"
#include "ToothContainer.h"
#include "ToothUtils.h"

using namespace Dental;

const ToothIndex& Procedure::getToothIndex() const
{
	static ToothIndex dummy;

	if (affectedTeeth.index() != 1) {
		return dummy;
	}

	return std::get<ToothIndex>(affectedTeeth);
}

ProcedureScope Procedure::getScope() const
{
	auto scope = static_cast<ProcedureScope>(affectedTeeth.index());

	//affectedTeeth might contain unknown bug
	if (scope == ProcedureScope::SingleTooth && !getToothIndex().isValid()) {
		return ProcedureScope::AllOrNone;
	}

	return scope;
}

void Procedure::applyProcedure(ToothContainer& teeth) const
{	

	//NOT IMPLEMENTED YET!
		return;

		auto& tooth_idx = getToothIndex();

		switch (code.type())
		{
			case::ProcedureType::Restoration:
			{
				
				auto& result = std::get<RestorationData>(param);

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

			case::ProcedureType::Endodontic:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, RootCanal, true, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(RootCanal, LPK);

			}
			break;

			case::ProcedureType::Extraction:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Dental::Missing, true, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(Missing, LPK);

			}
			break;

			case::ProcedureType::Crown:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Crown, true, tooth_idx.supernumeral);
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Fracture, false, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(Crown, LPK);

			}
			break;

			case::ProcedureType::Implant:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Implant, true, tooth_idx.supernumeral);
				teeth.at(tooth_idx).setLPK(Implant, LPK);

			}
			break;

			case::ProcedureType::Bridge:
			{
				auto& result = std::get<ConstructionRange>(affectedTeeth);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++)indexes.push_back(i);

				teeth.setStatus(indexes, StatusType::General, Bridge, true);
				teeth.setStatus(indexes, StatusType::General, Fracture, false);
				for (int i : indexes) teeth.at(i).setLPK(Bridge, LPK);

			}
			break;

			case ProcedureType::Splint:
			{
				auto& result = std::get<ConstructionRange>(affectedTeeth);

				std::vector<int> indexes;
				indexes.reserve(result.tooth_end - result.tooth_begin + 1);

				for (int i = result.tooth_begin; i <= result.tooth_end; i++) indexes.push_back(i);

				//teeth.removeBridgeOrSplint(indexes);
				teeth.setStatus(indexes, Dental::StatusType::General, Dental::Splint, true);

				for (int i : indexes) teeth.at(i).setLPK(Splint, LPK);

			}
			break;
			case ProcedureType::Denture:
			{
				auto& result = std::get<ConstructionRange>(affectedTeeth);

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

			case ProcedureType::RemoveCrownOrBridge:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Crown, false, tooth_idx.supernumeral);
			}
			break;

			case ProcedureType::RemovePost:
			{
				teeth.setStatus({ tooth_idx.index }, StatusType::General, Post, false, tooth_idx.supernumeral);
			}
			break;

			case ProcedureType::Depuratio:
			{
				for (auto& t : teeth)
				{
					t.setStatus(Dental::Calculus, false);
					if (t[HasSupernumeral]) t.getSupernumeral().setStatus(Dental::Calculus, false);
				}
			}
			break;

/*
			case ProcedureType::removebridgeOrSplint:
			{
				auto [begin, end] = std::get<ConstructionRange>(result);

				teeth.removeBridgeOrSplint(begin, end);
			}
			break;
			*/
		}
	
}

void Procedure::applyPISProcedure(ToothContainer& teeth) const
{
	/*
	
		This function is a simple status applier, not taking into
		consideration the result, since PIS doesn't store additional data
	
	*/

	auto& tooth_idx = getToothIndex();

	auto idx = getToothIndex().index;

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
		case ProcedureType::Restoration:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Restoration, true, tooth_idx.supernumeral); break;
		case ProcedureType::Endodontic:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::RootCanal, true, tooth_idx.supernumeral); break;
		case ProcedureType::Extraction:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Missing, true, tooth_idx.supernumeral); break;
		case ProcedureType::Crown:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Crown, true, tooth_idx.supernumeral); break;
		case ProcedureType::Implant:
			teeth.setStatus({ tooth_idx.index }, Dental::StatusType::General, Dental::Implant, true, tooth_idx.supernumeral); break;
		case ProcedureType::Denture:
		{
			int dentureBegin = code.nhifCode() == 832 ? 1 : 17;
			int dentureEnd = code.nhifCode() == 832 ? 14 : 30;

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
	switch (static_cast<ProcedureScope>(affectedTeeth.index()))
	{
		case ProcedureScope::AllOrNone:
			return std::string();
		case ProcedureScope::SingleTooth:
			return getToothIndex().getNhifNumenclature();
		case ProcedureScope::Range:
		{
			auto& [from, to] = std::get<ConstructionRange>(affectedTeeth);
			return ToothUtils::getNomenclature(from, false) + "-" + ToothUtils::getNomenclature(to, false);
		}
	}

}
