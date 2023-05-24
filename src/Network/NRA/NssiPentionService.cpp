#include "NssiPentionService.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Network/NetworkManager.h"

void NssiPentionService::parseReply(const std::string& reply)
{
	ModalDialogBuilder::showMultilineDialog(reply);
}

bool NssiPentionService::sendRequest(const Patient& p, std::function<void(const std::optional<Pention>&)> callback, bool showDialogs)
{
	m_callback = callback;

	this->show_dialogs = showDialogs;

	auto& practice = User::practice();

	if (!practice.hasNssiAccess())
	{
		if (showDialogs) {
			ModalDialogBuilder::showMessage("Не е въведена парола за достъп към НОИ");
		}
		return false;
	}

	std::string type = p.type == 1 ? "0" : "2";

	std::string query{

		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
			"<soap:Body>"
				"<GetNSSIPensionsData xmlns=\"http://tempuri.org/\">"
					"<Bulstat>" + practice.bulstat + "</Bulstat>"
					"<UniqueAccesCode>"+ practice.nhif_contract->nssi_pass + "</UniqueAccesCode>"
					"<RegNumber>" + practice.rziCode + "</RegNumber>"
					"<EGN>" + p.id + type + "</EGN>"
				"</GetNSSIPensionsData>"
			"</soap:Body>"
		"</soap:Envelope>"

	};

	awaiting_reply = true;

	NetworkManager::sendRequestToNssi(query, this);

	return true;
}
