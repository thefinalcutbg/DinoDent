#include "Procedure.h"
#include "ToothContainer.h"
#include "ToothUtils.h"

using namespace Dental;

const ToothIndex& Procedure::getToothIndex() const
{
	static ToothIndex dummy;

	if (HIS_fetched_result) {

		if (HIS_fetched_result->size() == 1) {
			return HIS_fetched_result->at(0).idx;
		}

		return dummy;
	}

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

std::vector<int> Procedure::getArrayIndexes() const
{
	if (HIS_fetched_result) {
		return HIS_fetched_result->getArrayIndexes();
	}
	
	std::vector<int> result;

	auto scope = getScope();

	if (scope == ProcedureScope::AllOrNone) return result;

	if (scope == ProcedureScope::SingleTooth) return { getToothIndex().index };

	auto& range = std::get<ConstructionRange>(affectedTeeth);
	result.reserve(range.getTeethCount());

	for (int i = range.toothFrom; i < range.toothTo + 1; i++) {
		result.push_back(i);
	}

	return result;
}

std::vector<const Tooth*> Procedure::applyProcedure(ToothContainer& teeth) const
{
	if (HIS_fetched_result) {
		return HIS_fetched_result->applyProcedure(teeth);
	}

	std::vector<int> teeth_indexes;
	std::vector<int> dsn_indexes;

	auto scope = getScope();
	auto type = code.type();

	//first we determine which teeth are affected by the procedure

	switch (scope)
	{
		case ProcedureScope::SingleTooth:
			{
				auto& toothIdx = getToothIndex();

				toothIdx.supernumeral ?
					dsn_indexes.push_back(toothIdx.index)
					:
					teeth_indexes.push_back(toothIdx.index);
			}
			break;

		case ProcedureScope::Range:
		{
			auto& range = std::get<ConstructionRange>(affectedTeeth);

			for (int i = range.toothFrom; i < range.toothTo + 1; i++) {

				if (type == ProcedureType::DepuratioQuadrant) {

					if (teeth[i][Calculus]) { teeth_indexes.push_back(i); }
					if (teeth[i].getSupernumeral()[Calculus]) { dsn_indexes.push_back(i); }

					continue;
				}

				if (type == ProcedureType::Denture) {

					if (teeth[i].canHaveADenture()) {
						teeth_indexes.push_back(i);
					}
					continue;
				}


				if (type == ProcedureType::MultipleExtraction) {

					if (!teeth[i][Missing]) {
						teeth_indexes.push_back(i);
					}

					if (teeth[i][HasSupernumeral] && !teeth[i].getSupernumeral()[Missing]) {
						dsn_indexes.push_back(i);
					}

					continue;
				}

				teeth_indexes.push_back(i);


				if (type == ProcedureType::RemoveCrownOrBridge) {

					if (teeth[i].getSupernumeral()[Crown]) {
						dsn_indexes.push_back(i);
					}
				}
			}
			
		}
		break;

		case ProcedureScope::AllOrNone:
		{
			if (type == ProcedureType::FullExam) {

				for(int i = 0; i < Dental::teethCount; i++) {

					teeth_indexes.push_back(i);

					if (teeth[i][HasSupernumeral]) {
						dsn_indexes.push_back(i);
					}
				}
			}

			if (type == ProcedureType::Depuratio) {

				for (int i = 0; i < Dental::teethCount; i++) {

					if (teeth[i][Calculus]) { teeth_indexes.push_back(i); }
					if (teeth[i].getSupernumeral()[Calculus]) { dsn_indexes.push_back(i); }
				}
			}

			if (type == ProcedureType::DenturePair) {

				std::array range = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,
								17,18,19,20,21,22,23,24,25,26,27,28,29,30 };

				for (auto i : range) {
					if (teeth[i].canHaveADenture()) {
						teeth_indexes.push_back(i);
					}
				}


			}
		}

	}

	//then define how the status should be changed

	struct StatusChange {
		StatusType type;
		int code;
		bool state;
		int lpk = -1; //made by this dentist
	};

	std::vector<StatusChange> changes;

	switch (type)
	{
		case ProcedureType::General:
			break;
		case ProcedureType::FullExam:
			break;
		case ProcedureType::Depuratio:
			changes.push_back({ StatusType::General, Calculus, false });
			break;
		case ProcedureType::DenturePair:
			changes.push_back({ StatusType::General, Denture, true, Denture });
			break;
		case ProcedureType::Anesthesia:
			break;
		case ProcedureType::Restoration:
		{
			changes.push_back({ StatusType::General, Fracture, false });

			auto& data = std::get<RestorationData>(param);

			for (int i = 0; i < data.surfaces.size(); i++) {

				if (!data.surfaces[i]) continue;

				changes.push_back({ StatusType::Caries, i, false });
				changes.push_back({ StatusType::Restoration, i, true, Restoration });
			}

			if (data.post) {
				changes.push_back({ StatusType::General, Post, true, Post });
			}
		}
			break;
		case ProcedureType::RemoveRestoration:
		{
			auto& data = std::get<RestorationData>(param);

			for (int i = 0; i < data.surfaces.size(); i++) {

				if (!data.surfaces[i]) continue;

				changes.push_back({ StatusType::Restoration, i, false });
				changes.push_back({ StatusType::Caries, i, true });
			}

			if (data.post) {
				changes.push_back({ StatusType::General, Post, false });
			}
		}
			break;
		case ProcedureType::ToothNonSpecific:
			break;
		case ProcedureType::DepuratioTooth:
			changes.push_back({ StatusType::General, Calculus, false });
			break;
		case ProcedureType::Extraction:
			changes.push_back({ StatusType::General, Missing, true, Missing });
			break;
		case ProcedureType::Implant:
			changes.push_back({ StatusType::General, Implant, true, Implant });
			break;
		case ProcedureType::Endodontic:
			changes.push_back({ StatusType::General, RootCanal, true, RootCanal });
			break;
		case ProcedureType::Post:
			changes.push_back({ StatusType::General, Post, true, Post });
			break;
		case ProcedureType::RemovePost:
			changes.push_back({ StatusType::General, Post, false });
			break;
		case ProcedureType::PostCore:
			changes.push_back({ StatusType::General, Root, true });
			changes.push_back({ StatusType::General, Fracture, false });
			changes.push_back({ StatusType::General, Post, true, Post });
			for (int i = 0; i < 6; i++) {
				changes.push_back({ StatusType::Restoration, i, true, Restoration });
			}
			break;
		case ProcedureType::PostCrown:
			changes.push_back({ StatusType::General, Fracture, false });
			changes.push_back({ StatusType::General, Post, true, Post });
			changes.push_back({ StatusType::General, Crown, true, Crown });
			break;
		case ProcedureType::Crown:
		{
			changes.push_back({ StatusType::General, Fracture, false });
			auto status = teeth_indexes.size() > 1 ? Bridge : Crown; //ambi
			changes.push_back({ StatusType::General, status, true, status });
		}
			break;
		case ProcedureType::CrownOrBridgeOrVeneer:
		{
			if (std::holds_alternative<RestorationData>(param)) {

				auto& data = std::get<RestorationData>(param);

				for (int i = 0; i < data.surfaces.size(); i++) {

					if (!data.surfaces[i]) continue;

					changes.push_back({ StatusType::Caries, i, false });
					changes.push_back({ StatusType::Restoration, i, true, Restoration });
				}
			}
			else {
				auto status = teeth_indexes.size() > 1 ? Bridge : Crown; //ambi
				changes.push_back({ StatusType::General, status, true, status });
			}
		}
			break;
		case ProcedureType::Bridge:
			changes.push_back({ StatusType::General, Fracture, false });
			changes.push_back({ StatusType::General, Bridge, true, Bridge });
			break;
		case ProcedureType::RemoveCrownOrBridge:
			changes.push_back({ StatusType::General, Crown, false});
			changes.push_back({ StatusType::General, Bridge, false });
			changes.push_back({ StatusType::General, Splint, false });
			break;
		case ProcedureType::Denture:
			changes.push_back({ StatusType::General, Denture, true, Denture });
			break;
		case ProcedureType::Splint:
			changes.push_back({ StatusType::General, Splint, true, Splint });
			break;
		case ProcedureType::DepuratioQuadrant:
			changes.push_back({ StatusType::General, Calculus, false });
			break;
		case ProcedureType::MultipleExtraction:
			changes.push_back({ StatusType::General, Missing, true, Missing });
			break;
		case ProcedureType::MaxCount:
			break;
		default:
			break;
	}

	//the meat grinder:

	for (auto& change : changes) {

		teeth.setStatus(teeth_indexes, change.type, change.code, change.state, false);
		teeth.setStatus(dsn_indexes, change.type, change.code, change.state, true);

		if (change.lpk != -1) {
			for (int i : teeth_indexes) teeth.at(i).setLPK(change.code, LPK);
			for (int i : dsn_indexes) teeth.at(i).getSupernumeral().setLPK(change.code, LPK);
		}
	}

	//generating the result with the affected teeth

	std::vector<const Tooth*> result;
	
	for (auto i : teeth_indexes) {
		result.push_back(&teeth.at(i));
	}

	for (auto i : dsn_indexes) {
		result.push_back(&teeth.at(i).getSupernumeral());
	}

	return result;
}

void Procedure::applyPISProcedure(ToothContainer& teeth) const
{
	/*
	
		This function is a simple status applier, not taking into
		consideration the additional parameters, since PIS doesn't store additional data
	
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
	if (HIS_fetched_result) {
		return HIS_fetched_result->getToothString();
	}

	switch (static_cast<ProcedureScope>(affectedTeeth.index()))
	{
		case ProcedureScope::AllOrNone:
			return std::string();
		case ProcedureScope::SingleTooth:
			return getToothIndex().getNhifNumenclature();
		case ProcedureScope::Range:
		{
			auto& [from, to] = std::get<ConstructionRange>(affectedTeeth);

			if (from == to) {
				return ToothUtils::getNomenclature(from, false);
			}

			return ToothUtils::getNomenclature(from, false) + "-" + ToothUtils::getNomenclature(to, false);
		}
	}

	return std::string();
}

