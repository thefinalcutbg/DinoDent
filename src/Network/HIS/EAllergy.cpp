#include "EAllergy.h"

#include <TinyXML/tinyxml.h>

#include "View/ModalDialogBuilder.h"
#include "Model/Patient.h"

bool EAllergy::Report::sendRequest(const Patient& patient, const Allergy& a, decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents;

	contents+=openTag("allergy");
		contents += bind("lrn", a.lrn);
		contents += bind("type", a.getHisNumenclatureIndex(Allergy::NumenclatureType::type));
		contents += bind("category", a.getHisNumenclatureIndex(Allergy::NumenclatureType::category));
		contents += bind("criticality", a.getHisNumenclatureIndex(Allergy::NumenclatureType::criticality));
		contents += bind("clinicalStatus", a.getHisNumenclatureIndex(Allergy::NumenclatureType::clinicalStatus));
		contents += bind("verificationStatus", a.getHisNumenclatureIndex(Allergy::NumenclatureType::verificationStatus));
		contents += bind("lastOccurrence", a.lastOccurrence.has_value() ? a.lastOccurrence->to8601() : "");
		contents += bind("note", a.description, true);
	contents += closeTag("allergy");

	contents += HisService::subject(patient);
	contents += HisService::performer(false);

	return sendRequestToHis(contents);
}

void EAllergy::Report::parseReply(const std::string& reply)
{
    auto cb = std::move(m_callback);

    m_callback = nullptr;

    if(cb == nullptr) return;

	if (reply.empty()) {
        cb({});
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
        cb({});
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrn = docHandle.FirstChild("nhis:message").FirstChild("nhis:contents").FirstChild("nhis:nrnAllergy");

    if (nrn.ToElement()) cb(nrn.ToElement()->Attribute("value"));
}

bool EAllergy::Edit::sendRequest(const Allergy& a, decltype(m_callback) callback/*, bool enteredInError*/)
{
	m_callback = callback;

	std::string contents;

	contents += openTag("allergy");
		contents += bind("nrnAllergy", a.nrn);
		contents += bind("type", a.getHisNumenclatureIndex(Allergy::NumenclatureType::type));
		contents += bind("category", a.getHisNumenclatureIndex(Allergy::NumenclatureType::category));
		contents += bind("criticality", a.getHisNumenclatureIndex(Allergy::NumenclatureType::criticality));
		contents += bind("clinicalStatus", a.getHisNumenclatureIndex(Allergy::NumenclatureType::clinicalStatus));
		contents += bind("verificationStatus", a.getHisNumenclatureIndex(Allergy::NumenclatureType::verificationStatus));
		contents += bind("lastOccurrence", a.lastOccurrence.has_value() ? a.lastOccurrence->to8601() : "");
		contents += bind("note", a.description, true);
	contents += closeTag("allergy");

	return sendRequestToHis(contents);
}

void EAllergy::Edit::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrn = docHandle.FirstChild("nhis:message").FirstChild("nhis:contents").FirstChild("nhis:nrnAllergy");

	if (nrn.ToElement())
	{
		m_callback(nrn.ToElement()->FirstAttribute()->ValueStr());
	}
	else {
		m_callback("");
	}
	
	m_callback = nullptr;
}

bool EAllergy::Fetch::sendRequest(
	const Patient& patient,
	const std::string& rzi,
	decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents;

	contents += bind("identifierType", patient.type);
	contents += bind("identifier", patient.id);
	contents += bind("practiceNumber", rzi);

	return HisService::sendRequestToHis(contents);
}

void EAllergy::Fetch::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback({});
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		if (show_dialogs) {
			ModalDialogBuilder::showError(errors);
		}
		m_callback({});
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto allergyXml = docHandle.FirstChild("nhis:message").FirstChild("nhis:contents").FirstChild("nhis:allergy").ToElement();

	std::vector<Allergy> result;

	while (allergyXml != nullptr)
	{
		result.emplace_back();
		auto& a = result.back();
		
		a.nrn = allergyXml->FirstChildElement("nhis:nrnAllergy")->FirstAttribute()->ValueStr();
		a.lrn = allergyXml->FirstChildElement("nhis:lrn")->FirstAttribute()->ValueStr();
		a.description = allergyXml->FirstChildElement("nhis:note")->FirstAttribute()->ValueStr();
		
		a.setHisNumenclature(Allergy::NumenclatureType::type, allergyXml->FirstChildElement("nhis:type")->FirstAttribute()->IntValue());
		a.setHisNumenclature(Allergy::NumenclatureType::category, allergyXml->FirstChildElement("nhis:category")->FirstAttribute()->IntValue());
		a.setHisNumenclature(Allergy::NumenclatureType::criticality, allergyXml->FirstChildElement("nhis:criticality")->FirstAttribute()->IntValue());
		a.setHisNumenclature(Allergy::NumenclatureType::clinicalStatus, allergyXml->FirstChildElement("nhis:clinicalStatus")->FirstAttribute()->IntValue());
		a.setHisNumenclature(Allergy::NumenclatureType::verificationStatus, allergyXml->FirstChildElement("nhis:verificationStatus")->FirstAttribute()->IntValue());

		auto occurence = allergyXml->FirstChildElement("nhis:lastOccurrence");

		if (occurence) {
			a.lastOccurrence.emplace(occurence->FirstAttribute()->ValueStr());
		}

		if (a.verificationStatus == Allergy::EnteredInError) {
			result.pop_back();
		}

		allergyXml = allergyXml->NextSiblingElement("nhis:allergy");
	}

	m_callback(result);

	m_callback = nullptr;


}
