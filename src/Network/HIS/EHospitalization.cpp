#include "EHospitalization.h"
#include "View/ModalDialogBuilder.h"
bool EHospitalization::Fetch::sendRequest(const Patient& patient, const std::string& rzi, const Date& from, const Date& to, decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents;
	contents += bind("fromDate", from.to8601());
	contents += bind("toDate", to.to8601());
	contents += bind("identifierType", patient.type);
	contents += bind("identifier", patient.id);
	contents += bind("practiceNumber", rzi);

	return HisService::sendRequestToHis(contents);
}

void EHospitalization::Fetch::parseReply(const std::string& reply)
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

	ModalDialogBuilder::showMultilineDialog(reply);
}
