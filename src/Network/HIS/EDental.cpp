#include "EDental.h"
#include "Model/User.h"
#include <TinyXML/tinyxml.h>

#include "View/ModalDialogBuilder.h"

void EDental::Open::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrn = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		FirstChildElement().ToElement(); //nrn

	if (nrn) {
		m_callback(nrn->Attribute("value"));
		return;
	}

	m_callback(std::string());
	
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
			+ bind("lrn", ambSheet.lrn)
			+ bind("basedOn", ambSheet.basedOn)
			+ bind("treatmentStart", ambSheet.date)
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

void EDental::Augment::parseReply(const std::string& reply)
{
}

bool EDental::Augment::sendRequest(const AmbList& ambSheet, const Patient& patient, std::function<void(bool)> success)
{
	return false;
}


bool EDental::Cancel::sendRequest(const std::string& nrn, std::function<void(bool)> success)
{
	m_callback = success;

	auto reason = ModalDialogBuilder::inputDialog(
		"Основание за анулиране на денталния преглед:",
		"Анулиране на дентален преглед"
	);

	if (reason.empty()) return false;

	std::string contents =
		bind("nrnDental", nrn)
		+ bind("cancelReason", reason);

	return HisService::sendRequestToHis(contents);
}

void EDental::Cancel::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto status = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		Child(1).ToElement(); //status

	if (
		status &&
		status->FirstAttribute()->IntValue() == 3
		)
	{
		m_callback(true); return;
	}

	m_callback(false);

}