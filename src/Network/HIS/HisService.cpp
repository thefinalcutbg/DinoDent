#include "HisService.h"
#include "Network/Common//XmlSigner.h"
#include "Network/Common//PKCS11.h"
#include "Network/Common/NetworkManager.h"
#include "HisToken.h"
#include "View/ModalDialogBuilder.h"


bool HisService::sendRequest(const std::string& query)
{
	if (HisToken::getToken().empty()) {
		return HisToken::requestToken(this, query);
	}

	auto signedMsg = signMessage(query);

	//if (signedMsg.empty()) return false;

	NetworkManager::sendRequestToHis(signedMsg, this, HisToken::getToken());

	return true;
	
}

void HisService::parseReply(const std::string& reply)
{
}

std::string HisService::signMessage(const std::string& message)
{
	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		ModalDialogBuilder::showMessage(u8"Не е открит КЕП");
		return {};
	}

	if (signer.loginRequired()) {

		NetworkManager::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(signer.pem_x509cert());

		if (pin.empty()) {
			return {};
		}


		if (!signer.login(pin))
		{
			ModalDialogBuilder::showError(u8"Грешна парола или блокирана карта");
			return {};
		};
	}

	return XmlSigner::signNhifMessage(message, signer.takePrivateKey(), signer.pem_x509cert());
}
