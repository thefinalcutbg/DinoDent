#include "PISServ.h"
#include "View/ModalDialogBuilder.h"
#include "PKCS11.h"
#include "Network.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"

TiXmlElement* nullCheck(TiXmlElement* e) {

	if (e == nullptr) {
		throw std::exception("Invalid reply");
	}

	return e;
}

std::string parseHirbnoReply(const std::string& reply)
{
	if (reply.empty())
		return {};

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	auto env = doc.RootElement();
	auto header = nullCheck(env->FirstChildElement());
	auto body = nullCheck(header->NextSiblingElement());
	auto table = nullCheck(body->FirstChildElement());
	auto headerRow = nullCheck(table->FirstChildElement());
	auto tableRow = nullCheck(headerRow->NextSiblingElement());
	auto tableData = nullCheck(tableRow->FirstChildElement());

	return leadZeroes(tableData->GetText(), 8);

}


std::string canonicalize(const std::string& soapBody)
{
	std::string append = " xmlns:e=\"http://schemas.xmlsoap.org/soap/envelope/\"";
	std::string canon;
	canon.reserve(soapBody.length() + append.length());
	canon = soapBody;
	canon.insert(7, append);

	return canon;
}

std::string signSoap(const std::string& soapBody, PKCS11& pkcs11)
{

	auto digestValue = PKCS11::sha1digest64(canonicalize(soapBody));
	auto signedValue = pkcs11.getSignedValue64(digestValue);

	if (signedValue.empty()) {
		return std::string();
	}

	auto certificate = pkcs11.x509certBase64();

	return
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<e:Envelope xmlns:e=\"http://schemas.xmlsoap.org/soap/envelope/\">"
			"<e:Header>"

				"<Signature xmlns=\"http://www.w3.org/2000/09/xmldsig#\">"
					"<SignedInfo>"
						"<CanonicalizationMethod Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\" />"
						"<SignatureMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#rsa-sha1\" />"
						"<Reference URI=\"#signedContent\">"
							"<DigestMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" />"
							"<DigestValue>" + digestValue + "</DigestValue>"
						"</Reference>"
					"</SignedInfo>"
				"<SignatureValue>"
					+signedValue+
				"</SignatureValue>"

				"<KeyInfo>" 
					"<X509Data>"
						"<X509Certificate>"
							+ certificate +
						"</X509Certificate>"
					"</X509Data>"
				"</KeyInfo>"

				"</Signature>"
			"</e:Header>"

				+ soapBody +

		"</e:Envelope>";
}


std::string PISServ::activeHIRBNo(const std::string& id, int personType)
{
	const char* personTypeArr[5]
	{
		"",
		"egn",
		"lnch",
		"",
		"",
	};

	std::string tag = personTypeArr[personType];

	return
		"<e:Body id=\"signedContent\">"
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>ACTIVE_HB</ns3:from_clause>"
		"</ns3:query>"
		"</e:Body>";
}

#include "View/ModalDialogBuilder.h"
void replyHandler(const std::string& reply) 
{
	try {
		auto result = parseHirbnoReply(reply);
		ModalDialogBuilder::showMessage(result);
	}
	catch (std::exception& e)
	{

		ModalDialogBuilder::showMessage(u8"Не е открита здравна книжка");
	}

}

void PISServ::sendRequest(const std::string& soapBody)
{
	PKCS11 token;

	if (token.loginRequired()) {

		auto pin = ModalDialogBuilder::getStringInput(u8"Вход с КЕП", u8"ПИН:");

		if (!pin.has_value()) {
			return;
		}

		if (!token.login(pin.value()))
		{
			ModalDialogBuilder::showError(u8"Wrong Password");
			return;
		};
	}

	try {
		Network::sendRequestToPis(signSoap(soapBody, token), token, replyHandler);
	}
	catch(std::exception& e) {
		ModalDialogBuilder::showMessage("Не е открита активна здравна книжка");
	}

}
