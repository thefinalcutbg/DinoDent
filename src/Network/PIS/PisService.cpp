#include "PisService.h"

#include "View/ModalDialogBuilder.h"
#include "Network/GetHSM.h"
#include "Network/NetworkManager.h"
#include "Network/signer.h"
#include "Model/Patient.h"
#include "Network/crypto.h"

void PisService::parseReply(const std::string& reply)
{

	//the html error reply from PIS begins with <!DOCTYPE ...
	//otherwise it begins with <?xml ....

	if (reply.size() && reply[1] == '!') {
		processPISReply("");
		return;
	}

	processPISReply(reply);
}

std::string PisService::getPersonIdTag(const Patient& p)
{

	static const char* personTypeArr[6]
	{
		"",
		"egn",
		"lnch",
		"nra",
		"con",
		"ehc"
	};

	int personType = p.type;
	//HIS and NHIF id type nomenclatures don't match!!!
	if (p.foreigner && p.foreigner->isEHIC()) {
		personType++;
	}

	return personTypeArr[personType];
}

bool PisService::sendRequest(const std::string& query, SOAPAction header)
{
	if (awaiting_reply) return true;

	auto hsm = GetHSM::get(show_dialogs);

	if (!hsm) { return false; }

	//Building the SOAP
	std::string body = "<e:Body id=\"signedContent\">" + query + "</e:Body>" ;

	std::string signedSoap = R"(<?xml version="1.0" encoding="utf-8"?><e:Envelope xmlns:e="http://schemas.xmlsoap.org/soap/envelope/"><e:Header>)";

	signedSoap += Signer::getSignature(
		//setting envelope namespace
		Crypto::addNamespacesToRoot(body, { {"e", "http://schemas.xmlsoap.org/soap/envelope/"} }),
		hsm->takePrivateKey(),
		hsm->x509ptr(),
		"#signedContent",
		true
	);
	
	signedSoap += "</e:Header>";

	signedSoap += body;

	signedSoap += "</e:Envelope>";

	std::string soapActionHeader;

	switch (header)
	{
	case SOAPAction::View:
		soapActionHeader = "\"http://pis.technologica.com/view\"";
		break;

	case SOAPAction::Files:
		soapActionHeader = "\"http://pis.technologica.com/files/\"";
		break;
	case SOAPAction::Diagnosis:
		soapActionHeader = "\"http://pis.technologica.com/pdDiagnosis\"";
		break;

	}

	awaiting_reply = true;

	NetworkManager::sendRequestToPis(
		signedSoap,
		hsm.value(),
		this,
		soapActionHeader.c_str()
	);

	return true;
}
