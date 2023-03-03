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

	//ModalDialogBuilder::showMultilineDialog(reply);

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrn = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		FirstChildElement().ToElement(); //nrn

	std::string nrnStr;

	if (nrn) {
		nrnStr = nrn->Attribute("value");
	}

	std::vector<int> procedureIndex;

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents


	//dentalProcedures
	for (int i = 3; contents.Child(i).ToElement(); i++)
	{
		//index
		procedureIndex.push_back(std::stoi(contents.Child(i).Child(1).ToElement()->Attribute("value")));	
	}

	m_callback(nrnStr, procedureIndex);
	
}

bool EDental::Open::sendRequest(
    const AmbList& ambSheet, 
    const Patient& patient, 
    std::function<void(const std::string&, const std::vector<int>&)> nrnCallback
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
			+ getProcedures(ambSheet.procedures, ambSheet.teeth)
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

void EDental::GetStatus::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	ModalDialogBuilder::showMultilineDialog(reply);
}

bool EDental::GetStatus::sendRequest(const Patient& patient, std::function<void(const ToothContainer&, const ProcedureContainer&)> callback)
{
	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("practiceNumber", User::practice().rziCode)
	;

	return HisService::sendRequestToHis(contents);
}
