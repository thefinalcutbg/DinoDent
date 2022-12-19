#include "PisService.h"
#include "View/ModalDialogBuilder.h"
#include "Network/PKCS11.h"
#include "Network/NetworkManager.h"
#include "Network/XmlSigner.h"

#include "View/ModalDialogBuilder.h"
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
		ModalDialogBuilder::showMessage("Не е открит КЕП");
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
