#include "HisToken.h"

#include <QElapsedTimer>
#include "View/ModalDialogBuilder.h"
#include "Network/GetHSM.h"
#include "Network/signer.h"
#include "Network/NetworkManager.h"
#include "HisService.h"

namespace sToken {

	HisService* current_service{ nullptr };
	std::string query_temp;
	std::string patient_signature;
	std::string token{};
	unsigned long long ms_expire{ 0 };
	QElapsedTimer timer;

	bool silent{ false };
}

void abort(const std::string& uiMessage) {

    if (sToken::current_service) {
        sToken::current_service->abortRequest();
		sToken::current_service = nullptr;
	}

    if (sToken::silent || uiMessage.empty()) return;

	ModalDialogBuilder::showMessage(uiMessage);
}

void HisToken::setChallengeMessage(const std::string& challenge)
{
	if (challenge.empty()) {
		abort("Неуспешна връзка с НЗИС");
		return;
	}

    auto hsm = GetHSM::get(!sToken::silent);

    if (!hsm) {
        //empty string, since the dialog is shown on GetHSM::get
        abort("");
		return;
	}

	auto signedChallenge = Signer::signEnveloped(challenge, hsm->takePrivateKey(), hsm->x509ptr(), true);

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

bool HisToken::requestToken(HisService* requester, const std::string& query, const std::string& patientSignature, bool silent)
{
	if (sToken::current_service) return false;

	sToken::current_service = requester;
	sToken::query_temp = query;
	sToken::patient_signature = patientSignature;
	sToken::silent = silent;

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

	if (!tokenElement) { 
		abort("Неуспешна идентификация"); 
		return;
	}

	const char* ptr = tokenElement->Attribute("value");

	if (!ptr) { abort("Невалидна стойност на токена"); return; }

	sToken::token = ptr;

	auto seconds_to_expire = strtoull(
		tokenElement->
		NextSiblingElement()->							//token type
		NextSiblingElement()->Attribute("value"), 		//seconds to expire
		nullptr, 0);

	sToken::ms_expire = seconds_to_expire * 999ULL;

	/* GIVES UTC
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

	sToken::current_service->sendRequestToHis(sToken::query_temp, sToken::patient_signature);

	sToken::current_service = nullptr;
}
