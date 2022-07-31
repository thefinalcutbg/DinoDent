#include "HisService.h"
#include "Network/Common//XmlSigner.h"
#include "Network/Common//PKCS11.h"
#include "Network/Common/NetworkManager.h"
#include "HisToken.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User/User.h"
#include "View/ModalDialogBuilder.h"

bool HisService::sendRequestToHis(const std::string& query)
{
	if (HisToken::getToken().empty()) {
		return HisToken::requestToken(this, query);
	}

	auto signedMsg = signMessage(buildMessage(query));

	if (signedMsg.empty()) return false;

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

#include <QUuid>
#include <QDateTime>
const std::string getUuid() {
	return QUuid::createUuid().toString(QUuid::StringFormat::Id128).toStdString();
}

const std::string timeNow() {
	auto t = QDateTime::currentDateTime();
	return t.toString(Qt::DateFormat::ISODate).toStdString();
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
