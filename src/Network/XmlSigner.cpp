#include "XmlSigner.h"

#include <stdlib.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include <xmlsec/openssl/evp.h>
#include <xmlsec/transforms.h>
#include <libxml/valid.h>

#include "PKCS11.h"

bool init{ false };


bool initialize()
{


    /* Init libxml and libxslt libraries */
    xmlInitParser();

    LIBXML_TEST_VERSION
        xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);



    if (xmlSecInit() < 0 ||                 /* Init xmlsec library */
        xmlSecCheckVersion() != 1 ||        /* Check loaded library version */
        xmlSecCryptoAppInit(NULL) < 0 ||    /* Init crypto library */
        xmlSecCryptoInit() < 0              /* Init xmlsec-crypto library */
        )
    {
        return false;
    }

    return true;
}

std::string XmlSigner::signPisQuery(const std::string& bodyContent, evp_pkey_st* prvKey, const std::string& pem_x509)
{

    auto result = 
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
			"<e:Body id=\"signedContent\">" //look at line 124
				+ bodyContent + //the soap body
			"</e:Body>"
		"</e:Envelope>";


    if (!init && !initialize()) {
        return {};//"xmlsec could not be initialized";
    }
    init = true;
   
    if (prvKey == nullptr) return{};
    if (pem_x509.empty()) return{};

    xmlChar* signedBuffer{ nullptr };
    int signedLength{ 0 };

    /* load doc file */
    xmlDocPtr doc = xmlParseMemory(result.data(), result.size());
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)) {
        return {};//"Error: unable to parse file";
    }

    xmlAttrPtr attr = xmlDocGetRootElement(doc)->children->next->properties;

    xmlIDPtr ptr = xmlAddID(NULL, doc, (const xmlChar*)"signedContent", attr);

    xmlNodePtr signNode = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
    if (signNode == NULL) {
        return {};//"Error: start node not found";
    }

    /* create signature context */
    xmlSecDSigCtxPtr dsigCtx = xmlSecDSigCtxCreate(NULL);
    if (dsigCtx == NULL) {
        return {};//"Error: failed to create signature context\n";
    }

    //dsigCtx->enabledReferenceUris = xmlSecTransformUriTypeSameDocument;

    dsigCtx->signKey = xmlSecKeyCreate();

    dsigCtx->signKey->value = xmlSecOpenSSLEvpKeyAdopt(prvKey);

    if (
        xmlSecOpenSSLAppKeyCertLoadMemory(
            dsigCtx->signKey,
            reinterpret_cast<const unsigned char*>(pem_x509.data()),
            pem_x509.size(),
            xmlSecKeyDataFormatPem) < 0
        )

    {
        return {};//"Failed to load certificate"; }
    }

     /* sign the template */
    if (xmlSecDSigCtxSign(dsigCtx, signNode) < 0) {
        
        return {};//"Error: signature failed\n";

    }

    /* print signed document to stdout */
    xmlDocDumpMemory(doc, &signedBuffer, &signedLength);

    result = std::string(reinterpret_cast<char*>(signedBuffer), signedLength);

    /* success */


    /* cleanup */
    if (dsigCtx != NULL) {
        xmlSecDSigCtxDestroy(dsigCtx);
    }

    if (doc != NULL) {
        xmlFreeDoc(doc);
    }

    return result;
}

std::string XmlSigner::signNhifMessage(const std::string& document, evp_pkey_st* prvKey, const std::string pem_x509)
{


    std::string_view signatureTemplate{

        "<Signature xmlns=\"http://www.w3.org/2000/09/xmldsig#\">"
            "<SignedInfo>"
                "<CanonicalizationMethod Algorithm=\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\" />"
                "<SignatureMethod Algorithm=\"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256\" />"
                "<Reference URI=\"\">"
                    "<Transforms>"
                        "<Transform Algorithm=\"http://www.w3.org/2000/09/xmldsig#enveloped-signature\" />"
                    "</Transforms>"
                    "<DigestMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#sha256\" />"
                        "<DigestValue></DigestValue>"
                "</Reference>"
            "</SignedInfo>"
            "<SignatureValue></SignatureValue>"
            "<KeyInfo>"
                "<X509Data>"
                    "<X509Certificate></X509Certificate>"
                "</X509Data>"
            "</KeyInfo>"
        "</Signature>"

    };

    std::string result{};

    result.reserve(document.size() + signatureTemplate.size());

    result = document;

    int insertPosition = result.size() - 1;

    for (;
        result[insertPosition] != '<' &&
        insertPosition != 0;
        insertPosition--
        ) {
    };

    if (!insertPosition) return ""; //not an xml (no closing tag)


    result.insert(insertPosition, signatureTemplate);

    if (!init && !initialize()) {
        return {};//"xmlsec could not be initialized";
    }
    init = true;

    if (prvKey == nullptr) return{};
    if (pem_x509.empty()) return{};

    xmlChar* signedBuffer{ nullptr };
    int signedLength{ 0 };

    /* load doc file */
    xmlDocPtr doc = xmlParseMemory(result.data(), result.size());
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)) {
        return {};//"Error: unable to parse file";
    }

    xmlAttrPtr attr = xmlDocGetRootElement(doc)->properties;

    xmlIDPtr ptr = xmlAddID(NULL, doc, (const xmlChar*)"", attr);

    xmlNodePtr signNode = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
    if (signNode == NULL) {
        return {};//"Error: start node not found";
    }

    /* create signature context */
    xmlSecDSigCtxPtr dsigCtx = xmlSecDSigCtxCreate(NULL);
    if (dsigCtx == NULL) {
        return {};//"Error: failed to create signature context\n";
    }

    //dsigCtx->enabledReferenceUris = xmlSecTransformUriTypeSameDocument;

    dsigCtx->signKey = xmlSecKeyCreate();

    dsigCtx->signKey->value = xmlSecOpenSSLEvpKeyAdopt(prvKey);

    if (
        xmlSecOpenSSLAppKeyCertLoadMemory(
            dsigCtx->signKey,
            reinterpret_cast<const unsigned char*>(pem_x509.data()),
            pem_x509.size(),
            xmlSecKeyDataFormatPem) < 0
        )

    {
        return {};//"Failed to load certificate"; }
    }

    /* sign the template */
    if (xmlSecDSigCtxSign(dsigCtx, signNode) < 0) {
        return {};//"Error: signature failed\n";

    }

    /* print signed document to stdout */
    xmlDocDumpMemory(doc, &signedBuffer, &signedLength);

    result = std::string(reinterpret_cast<char*>(signedBuffer), signedLength);

    /* success */


    /* cleanup */
    if (dsigCtx != NULL) {
        xmlSecDSigCtxDestroy(dsigCtx);
    }

    if (doc != NULL) {
        xmlFreeDoc(doc);
    }

    return result;
}



void XmlSigner::cleanup()
{
    /* Shutdown xmlsec-crypto library */
    xmlSecCryptoShutdown();


    /* Shutdown crypto library */
    xmlSecCryptoAppShutdown();

    /* Shutdown xmlsec library */
    xmlSecShutdown();

    xmlCleanupParser();

    init = false;
}

