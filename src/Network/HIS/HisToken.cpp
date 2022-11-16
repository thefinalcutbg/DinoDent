#include <QElapsedTimer>
#include "View/ModalDialogBuilder.h"
#include "Network/PKCS11.h"
#include "Network/XmlSigner.h"
#include "Network/NetworkManager.h"
#include "HisToken.h"
#include "HisService.h"

namespace sToken {

	HisService* current_service{ nullptr };
	std::string query_temp;

	std::string token{};
	unsigned long long ms_expire{ 0 };
	QElapsedTimer timer;
}

void abort(const std::string& uiMessage) {

	sToken::current_service = nullptr;

	ModalDialogBuilder::showMessage(uiMessage);
}

void HisToken::setChallengeMessage(const std::string& challenge)
{
	if (challenge.empty()) {
		abort("Неуспешна връзка с НЗИС");
		return;
	}

	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		abort("Не е открит КЕП");
		return;
	}

	if (signer.loginRequired()) {

		NetworkManager::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(signer.pem_x509cert());

		if (pin.empty()) {
			sToken::current_service = nullptr;
			return;
		}


		if (!signer.login(pin))
		{
			abort("Грешна парола или блокирана карта");
			return;
		};
	}

	auto signedChallenge = XmlSigner::signNhifMessage(challenge, signer.takePrivateKey(), signer.pem_x509cert());

	if (signedChallenge.empty()) {
		abort("Неуспешна автентификация в НЗИС");
		return;
	}

	NetworkManager::requestToken(signedChallenge);

}

void HisToken::nullifyToken()
{
	sToken::current_service = nullptr;
	sToken::token.clear();
}

bool HisToken::requestToken(HisService* requester, const std::string& query)
{
	if (sToken::current_service) return false;

	sToken::current_service = requester;
	sToken::query_temp = query;


	NetworkManager::requestChallenge();

	return true;
}

const std::string& HisToken::getToken()
{

	if (sToken::timer.isValid() &&
		sToken::timer.hasExpired(sToken::ms_expire))
	{
		nullifyToken();
	}

	return sToken::token;
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

	if (!tokenElement) { abort("Неуспешна идентификация"); return;  }

	const char* ptr = tokenElement->Attribute("value");

	if (!ptr) { abort("Невалидна стойност на токена"); return; }

	sToken::token = ptr;

	auto seconds_to_expire = strtoull(
		tokenElement->
		NextSiblingElement()->							//token type
		NextSiblingElement()->Attribute("value"), 		//seconds to expire
		nullptr, 0);

	sToken::ms_expire = seconds_to_expire * 999ULL;

	/* GIVES 2 HOURS BEHIND
	auto expiresOn = tokenElement->					
		NextSiblingElement()->					
		NextSiblingElement()->
		NextSiblingElement()->						//issuedOn
		NextSiblingElement()->Attribute("value");	//expiresOn
		*/

	if (sToken::timer.isValid())
	{
		sToken::timer.restart();
	}
	else
	{
		sToken::timer.start();
	}

	sToken::current_service->sendRequestToHis(sToken::query_temp);

	sToken::current_service = nullptr;
}
