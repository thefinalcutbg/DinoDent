#include "EDental.h"
#include "Model/User.h"


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
			+ bind("lrn", ambSheet.lnr)
			+ bind("basedOn", ambSheet.basedOn)
			+ bind("treatmentStart", ambSheet.time.to8601(ambSheet.getDate()))
			//+ bind("treatmentEnd", ambSheet.time.to8601(ambSheet.getDate())) //TO IMPLEMENT!!!
			+ bind("adverseConditions", adverseConditions)
			+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
			+ "<nhis:medicalStatus />" //TO IMPLEMENT!!!
			+ getStatus(ambSheet.teeth)
			+ getProcedures(ambSheet.procedures)
		+"</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer()
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

void EDental::Augment::parseReply(const std::string& reply)
{
}

bool EDental::Augment::sendRequest(const AmbList& ambSheet, const Patient& patient, std::function<void(bool)> success)
{
	return false;
}
