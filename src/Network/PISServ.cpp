#include "PISServ.h"
#include "View/ModalDialogBuilder.h"
#include "PKCS11.h"
#include "Network.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "XmlSigner.h"


const char* personTypeArr[5]
{
	"",
	"egn",
	"lnch",
	"",
	"",
};


std::string SOAP::dentalActivities(const std::string& id, int personType)
{

	std::string tag = personTypeArr[personType];

	return
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>INYEAR_DENTAL_ACTS</ns3:from_clause>"
		"</ns3:query>"
		;
}

std::string SOAP::activeHIRBNo(const std::string& id, int personType)
{
	std::string tag = personTypeArr[personType];

	return
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
		"xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>ACTIVE_HB</ns3:from_clause>"
		"</ns3:query>"
		;

}

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
				+ soapBody + //the soap body
			"</e:Body>"
		"</e:Envelope>";
}


bool PIS::sendRequest(const std::string& soapBody, AbstractReplyHandler& handler)
{

/*
Since both xmlSec and qt network manager adopt the private key and release it on their own
we have to create two PKCS11 instances - one for the signing and one for the SSL connection
*/

	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		ModalDialogBuilder::showMessage(u8"Не е открит КЕП");
		return false;
	}

	if (signer.loginRequired()) {

		auto pin = ModalDialogBuilder::getStringInput(signer.subjectName(), u8"ПИН:");

		/*if the dialog has been cancled*/
		if (!pin.has_value()) {
			return false;
		}

	
		if (!signer.login(pin.value()))
		{
			ModalDialogBuilder::showError(u8"Грешна парола или блокирана карта");
			return false;
		};
	}
			

	//creating another instance for the SSL certificate
	PKCS11 sslBuilder;


	Network::sendRequestToPis(

		XmlSigner::signSoapTemplate(
			soapToSign(soapBody), 
			signer.takePrivateKey(), 
			signer.ssl_x509cert()
		),

		sslBuilder,
		&handler
	);

	//XmlSigner::cleanup();

	return true;
	
}
