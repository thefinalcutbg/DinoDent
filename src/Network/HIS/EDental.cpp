#include "EDental.h"
#include "Model/User.h"

std::string EDental::Open::getStatus(const ToothContainer& teeth)
{
	std::string result;

	result.reserve(1000);

	result += "<nhis:dentalStatus>";

	for (auto& tooth : teeth)
	{
		std::vector<int> statuses;

		auto boolStatus = tooth.getBoolStatus();

		for (int i = 0; i < statusCount; i++) {
			if (boolStatus[i]) statuses.push_back(StatusCode::hisNum[i]);
		}

		if (statuses.empty()) continue; //should intact teeth be included?
		
		result += "<nhis:tooth>";
		result += bind("toothIndex", ToothUtils::getToothNumber(tooth.index, tooth.temporary));
		result += bind("dentalStatusDate", "!!!!!"); //TO IMPLEMENT!!!
		
		for (auto& s : statuses)
		{
			result += "<nhis:condition>";

			result += bind("code", s);

			switch (s)
			{
				case 11: //mobility
				{
					result += bind("mobilityDegree",
						static_cast<int>(tooth.mobility.degree) + 1
					);
				}
					break;

				case 6: //restoration
				{
					for (int i = 0; i < surfaceCount; i++)
						if (tooth.obturation.exists(i))
							result += bind("surface", i + 1);
				}
					break;

				case 2: //caries
				{
					for (int i = 0; i < surfaceCount; i++)
						if (tooth.caries.exists(i)) 
							result += bind("surface", i + 1);
				}
					break;

				case 17: //bridge
				{
					result += bind(
						"isEndmost",
						tooth.bridge.position != BridgePos::Middle ? "true" : "false"
					);
				}
					break;

				case 18: //splint
				{
					result += bind(
						"isEndmost",
						tooth.splint.position != BridgePos::Middle ? "true" : "false"
					);
				}
					break;
			}

			result += "</nhis:condition>";
		}

		result += "</nhis:tooth>";
	}

	result += "</nhis:dentalStatus>";

	return result;
}

std::string EDental::Open::getProcedures(const ProcedureContainer& procedures)
{
	std::string result;

	result.reserve(1000);


	int sequence = 0;

	for (auto& p : procedures)
	{
		result += "<nhis:dentalProcedure>";


		result += bind("sequence", sequence);

		sequence++;

		//result += bind("index", ???);

		result += bind("code", p.ksmp);
		result += bind("status", 6);
		result += bind("type", static_cast<int>(p.type) + 1);
		
		//result += bind("duration", ...);
		result += bind("datePerformed", p.date.to8601());
		result += bind("onSupernumeral", p.hyperdontic ? "true" : "false");
		result += bind("financingSource", static_cast<int>(p.financingSource));
		// CONDITION?!?!?!
		if (p.isToothSpecific())
		{
			result += bind("atToothIndex", ToothUtils::getToothNumber(p.tooth, p.temp));
		}

		if (p.isRangeSpecific())
		{
			auto [begin, end] = std::get<ConstructionRange>(p.result);
			result += bind("fromToothIndex", ToothUtils::getToothNumber(begin, false));
			result += bind("toToothIndex", ToothUtils::getToothNumber(end, false));
		}
		
		if (p.type == ProcedureType::obturation)
		{
			auto obtData = std::get<ProcedureObtData>(p.result);

			for (int i = 0; i < obtData.surfaces.size(); i++) {
				if (obtData.surfaces[i])
					result += bind("surface", i + 1);
			}

			result += bind("dentalPin", obtData.post);
		}

		result += bind("note", p.notes);

		result += bind("diagnosis", p.diagnosis);

		result += "</nhis:dentalProcedure>";

	}

	return result;
}
#include "View/ModalDialogBuilder.h"
void EDental::Open::parseReply(const std::string& reply)
{
	ModalDialogBuilder::showMultilineDialog(reply);
}

bool EDental::Open::sendRequest(
    const AmbList& ambSheet, 
    const Patient& patient, 
    std::function<void(const std::string&)> nrnCallback
)
{
	m_callback = nrnCallback;

	std::string contents;

	bool isNhif = ambSheet.isNhifSheet();

	bool adverseConditions = isNhif && ambSheet.nhifData.isUnfavourable;

	contents.reserve(4000);

	contents +=
		"<nhis:dentalTreatment>"
			+ bind("lnr", ambSheet.LNR)
			+ bind("basedOn", ambSheet.basedOn)
			+ bind("treatmentStart", ambSheet.time.to8601(ambSheet.getDate()))
			+ bind("treatmentEnd", "!!!!") //TO IMPLEMENT!!!
			+ bind("adverseConditions", adverseConditions ? "true" : "false")
			+ bind("rhifAreaNumber", User::practice().RHIF())
			+ "<nhis:medicalStatus />" //TO IMPLEMENT!!!
			+ getStatus(ambSheet.teeth)
			+ getProcedures(ambSheet.procedures)
			+ HisService::subject(patient)
			+ HisService::performer()
		+"</nhis:dentalTreatment>"
	;		

	return HisService::sendRequestToHis(contents);
}

void EDental::Cancel::parseReply(const std::string& reply)
{

}

bool EDental::Cancel::sendRequest(const std::string& nrn, std::function<void(bool)> success)
{
	return false;
}


