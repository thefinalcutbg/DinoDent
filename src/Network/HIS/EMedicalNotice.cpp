#include "EMedicalNotice.h"
#include "Model/MedicalNotice.h"

#include "View/ModalDialogBuilder.h"

#include <TinyXML/tinyxml.h>

bool EMedicalNotice::Issue::sendRequest(const MedicalNotice& notice, const Patient& patient, const std::string examNrn, std::function<void(const std::string&)> nrnCallback)
{
	m_callback = nrnCallback;

	std::string contents;

	contents.reserve(4000);

	contents += "<nhis:medicalNotice>";

		contents += HisService::bind("lrn", notice.lrn);
		contents += HisService::bind("basedOn", examNrn);
		contents += HisService::bind("authoredOn", notice.issue_date.to8601());
		contents += HisService::bind("reason", notice.reason);
		contents += HisService::bind("code", notice.mkb.code());
		contents += HisService::bind("fromDate", notice.from_date.to8601());
		contents += HisService::bind("toDate", notice.to_date.to8601());
		contents += HisService::bind("location", notice.location);
		contents += HisService::bind("institution", notice.institution, true);
		contents += HisService::bind("note", notice.note, true);

	contents += "</nhis:medicalNotice>";

	contents += subject(patient);
	contents += performer();

	return HisService::sendRequestToHis(contents);
}


void EMedicalNotice::Issue::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrnMedicalNotice = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		FirstChildElement().ToElement(); //nrnMedicalNotice

	if (nrnMedicalNotice) {
		m_callback(nrnMedicalNotice->Attribute("value"));
	}
	else
	{
		m_callback(std::string());
	};

	m_callback = nullptr;
}