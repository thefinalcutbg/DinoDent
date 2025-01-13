#include "signer.h"

#include "crypto.h"

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
			"<ds:DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\"/>"
			"<ds:DigestValue>"
			+
			Crypto::getSHA256DigestBase64(cert)
			+
			"</ds:DigestValue>"
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
		"<ds:SignedInfo>"
		"<ds:CanonicalizationMethod Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"/>"
		"<ds:SignatureMethod Algorithm=\"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256\"/>"
		"<ds:Reference Id=\"r-id-1\" URI=\"" + URI + "\">"
		"<ds:Transforms>";

	signedInfo += XAdES ?
		"<ds:Transform Algorithm=\"http://www.w3.org/TR/1999/REC-xpath-19991116\"><ds:XPath>not(ancestor-or-self::ds:Signature)</ds:XPath></ds:Transform>"
		:
		"<ds:Transform Algorithm=\"http://www.w3.org/2000/09/xmldsig#enveloped-signature\"/>";

	signedInfo +=
		"<ds:Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"/>"
		"</ds:Transforms>"
		"<ds:DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\"/>"
		"<ds:DigestValue>"
		//digest value of the document
		+
		Crypto::base64Encode(
			Crypto::calculateSHA256Digest(
				Crypto::canonicalizeXML(xml)
			)
		)
		+
		"</ds:DigestValue>"
		"</ds:Reference>"
		;

	//insert the XAdES ref
	if (XAdES) {

		signedInfo +=

			"<ds:Reference Type=\"http://uri.etsi.org/01903#SignedProperties\" URI=\"#xadesNode\">"
			"<ds:Transforms>"
			"<ds:Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"/>"
			"</ds:Transforms>"
			"<ds:DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\"/>"
			"<ds:DigestValue>"
			//digest value of the Signed properties
			+
			Crypto::base64Encode(
				Crypto::calculateSHA256Digest(
					Crypto::canonicalizeXML(
						Crypto::addNamespacesToRoot(xadesNode, { {"xades", "http://uri.etsi.org/01903/v1.3.2#"}, {"ds", signatureNs } })
					)
				)
			)
			+
			"</ds:DigestValue>"
			"</ds:Reference>"
			;
	}

	signedInfo += "</ds:SignedInfo>";


	std::string signature =
		"<ds:Signature xmlns:ds=\"" + signatureNs + "\" Id=\"signatureNode\">" +
		signedInfo +
		"<ds:SignatureValue Id=\"value-signatureNode\">"
		+
		Crypto::calculateSignature(
			Crypto::canonicalizeXML(
				Crypto::addNamespacesToRoot( //since we use exclusive C14, only the signatureNs is required
					signedInfo, NSList{ { "ds", signatureNs} }
				)
			), pkey
		)
		+
		"</ds:SignatureValue>" +
		"<ds:KeyInfo><ds:X509Data><ds:X509Certificate>" +
		Crypto::base64Encode(cert) +
		"</ds:X509Certificate></ds:X509Data></ds:KeyInfo>"
		;

	if (XAdES) {
		//insert XAdES object
		signature +=

			"<ds:Object>"
			"<xades:QualifyingProperties xmlns:xades=\"http://uri.etsi.org/01903/v1.3.2#\" Target=\"#signatureNode\">" +
			xadesNode +
			"</xades:QualifyingProperties>"
			"</ds:Object>"
			;
	}

	signature += "</ds:Signature>";

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
