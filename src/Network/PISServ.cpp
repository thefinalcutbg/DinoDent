#include "PISServ.h"
#include "View/ModalDialogBuilder.h"
#include "PKCS11.h"
#include "Network.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "XmlSigner.h"
TiXmlElement* nullCheck(TiXmlElement* e) {

	if (e == nullptr) {
		throw std::exception("Invalid reply");
	}

	return e;
}

std::string PISServ::parseHIRBNoReply(const std::string& reply)
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

	std::string result = tableData->GetText();

	if (result.size() > 8)
	{
		return result.substr(result.size() - 8);
	}

	return leadZeroes(tableData->GetText(), 8);

}

/*
* No need of this function, since xmlsec handles the signing for us
std::string canonicalize(const std::string& soapBody)
{
	std::string append = " xmlns:e=\"http://schemas.xmlsoap.org/soap/envelope/\"";
	std::string canon;
	canon.reserve(soapBody.length() + append.length());
	canon = soapBody;
	canon.insert(7, append);

	return canon;
}
*/
std::string soapToSign(const std::string& soapBody)
{
	
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
							"<DigestValue>"/*digest goes here*/"</DigestValue>"
						"</Reference>"
					"</SignedInfo>"
				"<SignatureValue>"/*signature value*/"</SignatureValue>"
				"<KeyInfo>" 
					"<X509Data>"
						"<X509Certificate>"/*x509 certificate*/"</X509Certificate>"
					"</X509Data>"
				"</KeyInfo>"
				"</Signature>"
			"</e:Header>"
			"<e:Body id=\"signedContent\">"
				+ soapBody +
			"</e:Body>"
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

		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>ACTIVE_HB</ns3:from_clause>"
		"</ns3:query>";
		
}
/*
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
*/
void PISServ::sendRequest(const std::string& soapBody, ReplyHandler* handler)
{

/*
Since both xmlSec and qt network manager adopt the private key and release it on their own
we have to create two PKCS11 instances - one for the signing and one for the SSL connection
*/

	PKCS11 signer;

	if (signer.loginRequired()) {

		auto pin = ModalDialogBuilder::getStringInput(signer.subjectName(), u8"ПИН:");

		/*if the dialog has been cancled*/
		if (!pin.has_value()) {
			return;
		}

	
		if (!signer.login(pin.value()))
		{
			ModalDialogBuilder::showError(u8"Грешна парола или блокирана карта");
			return;
		};
	}
			

	//creating another instance for the SSL certificate
	PKCS11 sslBuilder;

	try {
		Network::sendRequestToPis(

			XmlSigner::signSoapTemplate(
				soapToSign(soapBody), 
				signer.takePrivateKey(), 
				signer.ssl_x509cert()
			),

			sslBuilder,
			handler
		);
	}
	catch(std::exception& e) {
        ModalDialogBuilder::showMessage("Не е открита активна здравна книжка");
	}
	
}
