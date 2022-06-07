#include "XmlSigner.h"

#include <stdlib.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define XMLSEC_NO_XSLT

#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#include <libxslt/security.h>
#endif /* XMLSEC_NO_XSLT */

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include <xmlsec/openssl/evp.h>
#include <xmlsec/transforms.h>
#include <libxml/valid.h>

#include <QDebug>

#include "PKCS11.h"



bool init{ false };

#ifndef XMLSEC_NO_XSLT
xsltSecurityPrefsPtr xsltSecPrefs = NULL;
#endif /* XMLSEC_NO_XSLT */

bool initialize()
{

#ifndef XMLSEC_NO_XSLT
    xmlIndentTreeOutput = 1;
#endif /* XMLSEC_NO_XSLT */

    /* Init libxml and libxslt libraries */
    xmlInitParser();

    LIBXML_TEST_VERSION
        xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);

#ifndef XMLSEC_NO_XSLT
    /* disable everything */
    xsltSecPrefs = xsltNewSecurityPrefs();
    xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_READ_FILE, xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_WRITE_FILE, xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_READ_NETWORK, xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_WRITE_NETWORK, xsltSecurityForbid);
    xsltSetDefaultSecurityPrefs(xsltSecPrefs);
#endif /* XMLSEC_NO_XSLT */

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



std::string XmlSigner::signXML(const std::string& document, evp_pkey_st* prvKey, const std::string& x509)
{

    if (!init && !initialize()) {
        return "xmlsec could not be initialized";
    }
    init = true;

    if (prvKey == nullptr) return{};
    if (x509.empty()) return{};

    std::string result;


    xmlChar* signedBuffer{ nullptr };
    int signedLength{ 0 };

    /* load doc file */
    xmlDocPtr doc = xmlParseMemory(document.data(), document.size());
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)) {
        return "Error: unable to parse file";
    }

    /* create signature template for RSA-SHA1 enveloped signature */
    xmlNodePtr signNode = xmlSecTmplSignatureCreate(doc, xmlSecTransformExclC14NId,
        xmlSecTransformRsaSha256Id, NULL);
    if (signNode == NULL) {
        return "Error: failed to create signature template\n";
    }

    /* add <dsig:Signature/> node to the doc */
    xmlAddChild(xmlDocGetRootElement(doc), signNode);

    /* add reference */
    xmlNodePtr refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha256Id,
        NULL, NULL, NULL);
    if (refNode == NULL) {
        return "Error: failed to add reference to signature template\n";
    }

    /* add enveloped transform */
    if (xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformEnvelopedId) == NULL) {
        return "Error: failed to add enveloped transform to reference\n";
    }

    /* add <dsig:KeyInfo/> and <dsig:X509Data/> */
    xmlNodePtr keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo(signNode, NULL);
    if (keyInfoNode == NULL) {
        return "Error: failed to add key info\n";

    }

    xmlNodePtr x509DataNode = xmlSecTmplKeyInfoAddX509Data(keyInfoNode);
    if (x509DataNode == NULL) {
        return "Error: failed to add X509Data node\n";

    }

    if (xmlSecTmplX509DataAddSubjectName(x509DataNode) == NULL) {
        return "Error: failed to add X509SubjectName node\n";

    }

    if (xmlSecTmplX509DataAddCertificate(x509DataNode) == NULL) {
        return "Error: failed to add X509Certificate node\n";

    }

    /* create signature context, we don't need keys manager in this example */
    xmlSecDSigCtxPtr dsigCtx = xmlSecDSigCtxCreate(NULL);
    if (dsigCtx == NULL) {
        return "Error: failed to create signature context\n";
    }


    //load the private key:

    dsigCtx->signKey = xmlSecKeyCreate();

    dsigCtx->signKey->value = xmlSecOpenSSLEvpKeyAdopt(prvKey);

    if (
        xmlSecOpenSSLAppKeyCertLoadMemory(
            dsigCtx->signKey,
            reinterpret_cast<const unsigned char*>(x509.data()),
            x509.size(),
            xmlSecKeyDataFormatPem) < 0
        )

    {
        return "Failed to load certificate";
    }

     /* sign the template */
    if (xmlSecDSigCtxSign(dsigCtx, signNode) < 0) {
        return "Error: signature failed\n";

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

std::string XmlSigner::signSoapTemplate(const std::string& document, evp_pkey_st* prvKey, const std::string& x509)
{

    if (!init && !initialize()) {
        return "xmlsec could not be initialized";
    }
    init = true;
   
    if (prvKey == nullptr) return{};
    if (x509.empty()) return{};

    std::string result;

    xmlChar* signedBuffer{ nullptr };
    int signedLength{ 0 };

    /* load doc file */
    xmlDocPtr doc = xmlParseMemory(document.data(), document.size());
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)) {
        return "Error: unable to parse file";
    }
    
    xmlAttrPtr attr = xmlDocGetRootElement(doc)->children->next->properties;

    xmlIDPtr ptr = xmlAddID(NULL, doc, (const xmlChar*)"signedContent", attr);

    xmlNodePtr signNode = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
    if (signNode == NULL) {
        return "Error: start node not found";
    }

    /* create signature context */
    xmlSecDSigCtxPtr dsigCtx = xmlSecDSigCtxCreate(NULL);
    if (dsigCtx == NULL) {
        return "Error: failed to create signature context\n";
    }

    //dsigCtx->enabledReferenceUris = xmlSecTransformUriTypeSameDocument;

    dsigCtx->signKey = xmlSecKeyCreate();

    dsigCtx->signKey->value = xmlSecOpenSSLEvpKeyAdopt(prvKey);

    if (
        xmlSecOpenSSLAppKeyCertLoadMemory(
            dsigCtx->signKey,
            reinterpret_cast<const unsigned char*>(x509.data()),
            x509.size(),
            xmlSecKeyDataFormatPem) < 0
        )

    { return "Failed to load certificate"; }
    

     /* sign the template */
    if (xmlSecDSigCtxSign(dsigCtx, signNode) < 0) {
        return "Error: signature failed\n";

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

    /* Shutdown libxslt/libxml */
#ifndef XMLSEC_NO_XSLT
    xsltFreeSecurityPrefs(xsltSecPrefs);
    xsltCleanupGlobals();
#endif /* XMLSEC_NO_XSLT */

    xmlCleanupParser();
}



