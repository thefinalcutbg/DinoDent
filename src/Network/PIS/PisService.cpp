#include "PisService.h"
#include "View/ModalDialogBuilder.h"
#include "Network/PKCS11.h"
#include "Network/NetworkManager.h"
#include "Network/XmlSigner.h"
#include "Model/Patient.h"

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

	if (p.foreigner && p.foreigner->isEHIC()) {
		personType++;
	}

	return personTypeArr[personType];
}

bool PisService::sendRequest(const std::string& query, SOAPAction header)
{
	/*
Since both xmlSec and qt network manager adopt the private key and release it on their own
we have to create two PKCS11 instances - one for the signing and one for the SSL connection
*/
	if (awaiting_reply) return true;

	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		if (show_dialogs) {
			ModalDialogBuilder::showMessage("Не е открит КЕП");
		}

		show_dialogs = true;
		return false;
	}

	if (signer.loginRequired()) {

		NetworkManager::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(signer.pem_x509cert());

		if (pin.empty()) {
			return false;
		}


		if (!signer.login(pin))
		{
			ModalDialogBuilder::showError("Грешна парола или блокирана карта");
			return false;
		};
	}

	//creating another instance for the SSL certificate
	PKCS11 clientSsl;

	auto signedRequest = XmlSigner::signPisQuery(
		query,
		signer.takePrivateKey(),
		signer.pem_x509cert()
	);

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
		signedRequest,
		clientSsl,
		this,
		soapActionHeader.c_str()
	);

	//XmlSigner::cleanup();



	return true;
}
