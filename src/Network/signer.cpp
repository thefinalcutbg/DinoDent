#include "Signer.h"

#include "crypto.h"
#include "pkcs11.h"

std::string Signer::getSignature(const std::string& xml, evp_pkey_st* pkey, x509_st* cert, const std::string& URI, bool XAdES)
{
	std::string xadesNode;

	if (XAdES) {

		xadesNode =
			"<xades:SignedProperties Id=\"xadesNode\">"
			"<xades:SignedSignatureProperties>"
			"<xades:SigningTime>"
			+ Crypto::get8601timestamp() +
			"</xades:SigningTime>"
			"<xades:SigningCertificateV2>"
			"<xades:Cert>"
			"<xades:CertDigest>"
			"<DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\"/>"
			"<DigestValue>"
			+
			Crypto::getSHA256DigestBase64(cert)
			+
			"</DigestValue>"
			"</xades:CertDigest>"
			"</xades:Cert>"
			"</xades:SigningCertificateV2>"
			"<xades:SignatureProductionPlaceV2>"
			"<xades:City/>"
			"<xades:StateOrProvince/>"
			"<xades:PostalCode/>"
			"<xades:CountryName>"
			+
			Crypto::get_country_from_x509(cert)
			+
			"</xades:CountryName>"
			"</xades:SignatureProductionPlaceV2>"
			"<xades:SignerRoleV2>"
			"<xades:ClaimedRoles>"
			"<xades:ClaimedRole>SIGNED BY</xades:ClaimedRole>"
			"</xades:ClaimedRoles>"
			"</xades:SignerRoleV2>"
			"</xades:SignedSignatureProperties>"
			"<xades:SignedDataObjectProperties>"
			"<xades:DataObjectFormat ObjectReference=\"#r-id-1\">"
			"<xades:MimeType>text/xml</xades:MimeType>"
			"</xades:DataObjectFormat>"
			"</xades:SignedDataObjectProperties>"
			"</xades:SignedProperties>";
	}

	const std::string signatureNs = "http://www.w3.org/2000/09/xmldsig#";

	std::string signedInfo =
		"<SignedInfo>"
		"<CanonicalizationMethod Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"/>"
		"<SignatureMethod Algorithm=\"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256\"/>"
		"<Reference Id=\"r-id-1\" URI=\"" + URI +"\">"
		"<Transforms>"
		"<Transform Algorithm=\"http://www.w3.org/2000/09/xmldsig#enveloped-signature\"/>"
		"<Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"/>"
		"</Transforms>"
		"<DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\"/>"
		"<DigestValue>"
		//digest value of the document
		+
		Crypto::base64Encode(
			Crypto::calculateSHA256Digest(
				Crypto::canonicalizeXML(xml)
			)
		)
		+
		"</DigestValue>"
		"</Reference>"
		;

	//insert the XAdES ref
	if (XAdES) {

		signedInfo +=

			"<Reference Type=\"http://uri.etsi.org/01903#SignedProperties\" URI=\"#xadesNode\">"
			"<Transforms>"
			"<Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"/>"
			"</Transforms>"
			"<DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\"/>"
			"<DigestValue>"
			//digest value of the Signed properties
			+
			Crypto::base64Encode(
				Crypto::calculateSHA256Digest(
					Crypto::canonicalizeXML(
						Crypto::addNamespacesToRoot(xadesNode, { {"xades", "http://uri.etsi.org/01903/v1.3.2#"}, {"", signatureNs } })
					)
				)
			)
			+
			"</DigestValue>"
			"</Reference>"
			;
	}

	signedInfo += "</SignedInfo>";


	std::string signature =
		"<Signature xmlns=\"" + signatureNs + "\">" +
		signedInfo +
		"<SignatureValue>"
		+
		Crypto::calculateSignature(
			Crypto::canonicalizeXML(
				Crypto::addNamespacesToRoot( //since we use exclusive C14, only the signatureNs is required
					signedInfo, NSList{ { "", signatureNs} }
				)
			), pkey
		)
		+
		"</SignatureValue>" +
		"<KeyInfo><X509Data><X509Certificate>" +
		Crypto::base64Encode(cert) +
		"</X509Certificate></X509Data></KeyInfo>"
		;

	if (XAdES) {
		//insert XAdES object
		signature +=

			"<Object>"
			"<xades:QualifyingProperties xmlns:xades=\"http://uri.etsi.org/01903/v1.3.2#\">" +
			xadesNode +
			"</xades:QualifyingProperties>"
			"</Object>"
			;
	}

	signature += "</Signature>";

	return signature;
}

std::string Signer::signEnveloped(const std::string& xml, evp_pkey_st* pkey, x509_st* cert, bool XAdES)
{
	auto signature = getSignature(xml, pkey, cert, std::string(), XAdES);

	auto result = xml;

	int insertPosition = result.size() - 1;

	for (;
		result[insertPosition] != '<' &&
		insertPosition != 0;
		insertPosition--
		) {
	};

	if (!insertPosition) return "not an xml (no closing tag)";

	result.insert(insertPosition, signature);

	return result;
}