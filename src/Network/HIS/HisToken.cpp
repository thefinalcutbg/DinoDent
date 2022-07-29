
#include "View/ModalDialogBuilder.h"
#include "Network/Common//PKCS11.h"
#include "Network/Common//XmlSigner.h"
#include "Network/Common/NetworkManager.h"
#include "HisToken.h".h"
#include "HisService.h"

HisService* current_service{nullptr};
std::string query_temp;

std::string s_token{};

void abort(const std::string& uiMessage) {

	current_service = nullptr;

	ModalDialogBuilder::showMessage(uiMessage);
}

void HisToken::setChallengeMessage(const std::string& challenge)
{
	if (challenge.empty()) {
		abort(u8"Неуспешна връзка с НЗИС");
		return;
	}

	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		abort(u8"Не е открит КЕП");
		return;
	}

	if (signer.loginRequired()) {

		NetworkManager::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(signer.pem_x509cert());

		if (pin.empty()) {
			return;
		}


		if (!signer.login(pin))
		{
			abort(u8"Грешна парола или блокирана карта");
			return;
		};
	}

	auto signedChallenge = XmlSigner::signNhifMessage(challenge, signer.takePrivateKey(), signer.pem_x509cert());

	if (signedChallenge.empty()) {
		abort(u8"Неуспешна автентификация в НЗИС");
		return;
	}

	NetworkManager::requestToken(signedChallenge);

}

void HisToken::nullifyToken()
{
	s_token.clear();
}

bool HisToken::requestToken(HisService* requester, const std::string& query)
{
	if (current_service) return false;

	current_service = requester;
	query_temp = query;


	NetworkManager::requestChallenge();

	return true;
}

const std::string& HisToken::getToken()
{
	return s_token;
}

#include "TinyXML/tinyxml.h"

void HisToken::setAuthRepy(const std::string& reply)
{
	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	TiXmlElement* tokenElement =
		docHandle
		.FirstChildElement()			  //message
		.FirstChildElement()			  //contents
		.FirstChildElement().ToElement(); //token

	if (!tokenElement) { abort(u8"Неуспешна идентификация"); return;  }

	const char* ptr = tokenElement->Attribute("value");

	if (!ptr) { abort(u8"Невалидна стойност на токена"); return; }

	s_token = ptr;

	auto seconds = std::stoi(tokenElement->
		NextSiblingElement()->						//token type
		NextSiblingElement()->Attribute("value"));	//seconds to expire

	auto expiresOn = tokenElement->					
		NextSiblingElement()->					
		NextSiblingElement()->
		NextSiblingElement()->						//issuedOn
		NextSiblingElement()->Attribute("value");	//expiresOn

	ModalDialogBuilder::showMessage(u8"Токен: " + s_token + 
		"\n изтича на: " + expiresOn);

	current_service->sendRequest(query_temp);
}
