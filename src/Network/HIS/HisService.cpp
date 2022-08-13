#include "HisService.h"
#include "Network/Common//XmlSigner.h"
#include "Network/Common//PKCS11.h"
#include "Network/Common/NetworkManager.h"
#include "HisToken.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User/User.h"
#include "View/ModalDialogBuilder.h"

#include <QUuid>
#include <QDateTime>
std::string getUuid() {
	return QUuid::createUuid().toString(QUuid::StringFormat::Id128).toStdString();
}

std::string timeNow() {
	auto t = QDateTime::currentDateTime();
	return t.toString(Qt::DateFormat::ISODate).toStdString();
}


bool HisService::sendRequestToHis(const std::string& query)
{
	if (HisToken::getToken().empty()) {
		return HisToken::requestToken(this, query);
	}

	ModalDialogBuilder::showMultilineDialog(buildMessage(query));

	auto signedMsg = signMessage(buildMessage(query));

	if (signedMsg.empty()) return false;

	return true;

	awaiting_reply = true;


	NetworkManager::sendRequestToHis(
		  this, 
		  signedMsg, 
		  HisToken::getToken(), 
		 "https://ptest-api.his.bg/" + servicePath
	);

	return true;
	
}

const std::string HisService::signMessage(const std::string& message)
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




const std::string HisService::buildMessage(const std::string& query)
{

	constexpr const char* softwareName = "Torque";

	std::string result;

	result.reserve(4000);

	result +=

	"<nhis:message "
		"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
		"xmlns:nhis=\"https://www.his.bg\" "
		"xsi:schemaLocation=\"https://www.his.bg/api/v1/NHIS-" + messageType + ".xsd\">"
	
			"<nhis:header>"
				"<nhis:sender value=\"2\"/>"
				"<nhis:senderId value=\"" + User::doctor().LPK + "\"/>"
				"<nhis:senderISName value=\"" + softwareName + "\"/>"
				"<nhis:recipient value=\"4\"/>"
				"<nhis:recipientId value=\"NHIS\"/>"
				"<nhis:messageId value=\"" + getUuid() + "\"/>"
				"<nhis:messageType value=\"" + messageType + "\"/>"
				"<nhis:createdOn  value=\"" + timeNow() + "\"/>"
			"</nhis:header>"
		
			"<nhis:contents>"
				+query+
			"</nhis:contents>"
	"</nhis:message>"
	
	;

	return result;
	
}

std::string HisService::subject(const Patient& p)
{
	std::string middleNameTag = p.MiddleName.size()?
		"<nhis:middle value=\"" + p.MiddleName + "\"/>" : "";

	std::string subject =
	"<nhis:subject>"
		"<nhis:identifierType value=\"" + std::to_string(p.type) + "\"/>"
		"<nhis:identifier value=\"" + p.id + "\"/>"
		"<nhis:birthDate value=\"" + p.birth.toXMLString() + "\"/>"
		"<nhis:gender value=\"" + std::to_string(static_cast<int>(p.sex) + 1) + "\"/>"
		"<nhis:name>"
			"<nhis:given value=\"" + p.FirstName + "\"/>"
			+ middleNameTag + //because it is optional
			"<nhis:family value=\"" + p.LastName + "\"/>"
		"</nhis:name>"
		"<nhis:address>"
			"<nhis:country value=\"BG\"/>"
			//<!-- Optional: -->
			"<nhis:ekatte value=\"" + p.city.ekatte() + "\"/>"
			"<nhis:city value=\"" + p.city.getString() + "\"/>"
		"</nhis:address>"
		"<nhis:phone value=\""+p.phone+"\"/>"
		//<nhis:email value="[string]"/>
	"</nhis:subject>"
	;

	return subject;
}

std::string HisService::requester()
{
	//cl008 numenclature
	auto lambda = []()->std::string{

		switch (User::doctor().specialty)
		{
			case NhifSpecialty::Pediatric: return "2079";
			case NhifSpecialty::General: return "2081";
			case NhifSpecialty::OralSurgeon: return "2083";
			case NhifSpecialty::Maxillofacial: return "3088";
			default: throw std::exception("bad specialty");
		}

	};

	std::string qualification = User::practice().nzok_contract ?
		"<nhis:qualification value=\"" + lambda() + "\" nhifCode=\"" + std::to_string(User::doctor().specialtyAsInt()) + "\"/>"
		:
		"";

	std::string requester =
		"<nhis:requester>"
			"<nhis:pmi value=\"" + User::doctor().LPK + "\"/>"
			+ qualification +
			"<nhis:practiceNumber value=\"" + User::practice().rziCode + "\"/>"
			"<nhis:nhifNumber value=\"" + User::practice().RHIF() + "\"/>"
			"<nhis:phone value=\"" + User::doctor().phone + "\"/>"
		"</nhis:requester>"
		;
	return requester;

}

