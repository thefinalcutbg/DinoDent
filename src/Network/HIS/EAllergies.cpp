#include "EAllergies.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Patient.h"

bool EAllergies::Fetch::sendRequest(
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

void EAllergies::Fetch::parseReply(const std::string& reply)
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


}