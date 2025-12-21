#include "EHospitalization.h"
#include "View/ModalDialogBuilder.h"
#include <TinyXML/tinyxml.h>

bool EHospitalization::Fetch::sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents;
	contents += bind("fromDate", "1900-01-01");
	contents += bind("toDate", Date::currentDate().to8601());
	contents += bind("identifierType", patient.type);
	contents += bind("identifier", patient.id);
	contents += bind("practiceNumber", rzi);

	return HisService::sendRequestToHis(contents);
}

void EHospitalization::Fetch::parseReply(const std::string& reply)
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
		if (show_dialogs) {
			ModalDialogBuilder::showError(errors);
		}
        cb({});
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	std::vector<Hospitalization> result;

	for (
		auto resultXml = docHandle
		.FirstChild("nhis:message")
		.FirstChild("nhis:contents")
		.FirstChild("nhis:results").ToElement();
		resultXml != nullptr;
		resultXml = resultXml->NextSiblingElement("nhis:results")
		)
	{
		result.emplace_back();

		auto hospitalization = resultXml->FirstChildElement("nhis:hospitalization");

		result.back().authoredOn =hospitalization->FirstChildElement("nhis:authoredOn")->FirstAttribute()->ValueStr();
		result.back().status = static_cast<Hospitalization::Status>(hospitalization->FirstChildElement("nhis:status")->FirstAttribute()->IntValue());

	}

    cb(result);

	return;

}
