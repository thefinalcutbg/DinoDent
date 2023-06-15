#include "NssiPentionService.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Network/NetworkManager.h"
#include "TinyXML/tinyxml.h"

void NssiPentionService::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) return;

	TiXmlHandle response(&doc);

	auto report =
		response
		.FirstChildElement()			//envelope
		.FirstChildElement()			//body
		.FirstChildElement()			//GetNSSIPensionDataResponse
		.FirstChildElement()			//GetNSSIPensionDataResult
		.FirstChildElement()			//start
		.FirstChildElement();			//Pensioner

	auto responseString = report.ChildElement(2);
	auto responseCode = report.ChildElement(3);
	
	if (responseString.ToElement() == nullptr)
	{
		ModalDialogBuilder::showMessage("Некоректни данни за проверка");
		return;
	}

	ModalDialogBuilder::showMessage(responseString.ToElement()->GetText());

	
	return;
}

bool NssiPentionService::sendRequest(const Patient& p)
{

	auto& practice = User::practice();

	if (!practice.hasNssiAccess())
	{
		ModalDialogBuilder::showMessage("Не е въведена парола за достъп към НОИ");
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
