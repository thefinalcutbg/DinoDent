#include "PKCS11.h"
#include <libp11/libp11.h>
#include <vector>
#include "Base64Convert.h"
#include <iostream>
#include <exception>

PKCS11_CTX* ctx{ nullptr };

std::vector<std::string> modules{
	"c:/dev/bit4xpki.dll",
	"c:/dev/IDPrimePKCS1164.dll"
};

bool loadModuleWithToken()
{
	if (!ctx) {
		ctx = PKCS11_CTX_new();
	}

	bool success = false;

	for (int i = 0; i < modules.size(); i++)
	{

		std::cout << modules[i] << std::endl;

		if (PKCS11_CTX_load(ctx, modules[i].data()))
		{
			PKCS11_CTX_unload(ctx);
			continue;
		}

		PKCS11_SLOT* testSlot, * testSlots;
		unsigned int testNSlots;

		if (PKCS11_enumerate_slots(ctx, &testSlots, &testNSlots))
		{
			PKCS11_CTX_unload(ctx);
			continue;
		}

		testSlot = PKCS11_find_token(ctx, testSlots, testNSlots);

		if (testSlot == NULL || testSlot->token == NULL) {

			std::cout << "no valid token" << std::endl;
			PKCS11_release_all_slots(ctx, testSlots, testNSlots);
			PKCS11_CTX_unload(ctx);
			continue;
		}

		PKCS11_CERT* testCert, * testCerts;
		unsigned int testNCerts;

		if (PKCS11_enumerate_certs(testSlot->token, &testCerts, &testNCerts) || testNCerts <= 0)
		{
			PKCS11_release_all_slots(ctx, testSlots, testNSlots);
			PKCS11_CTX_unload(ctx);
			continue;
		}

		PKCS11_release_all_slots(ctx, testSlots, testNSlots);

		success = true;
		break;
	}

	return success;

}

PKCS11::PKCS11()
{
	if (PKCS11_enumerate_slots(ctx, &m_slots, &nslots) == -1) {

		if (!loadModuleWithToken()) {

			throw std::exception("no token found");
		}

		PKCS11_enumerate_slots(ctx, &m_slots, &nslots);

	}

	m_slot = PKCS11_find_token(ctx, m_slots, nslots);

	if (m_slot == nullptr)
		throw std::exception("no token found2");

	PKCS11_enumerate_certs(m_slot->token, &certs, &ncerts);

	m_certificate = &certs[0];

	if (!m_certificate || ncerts <= 0) {
		throw std::exception("no certificate found");
	}

	//getting the certificate

	int length = i2d_X509(m_certificate->x509, 0);

	std::string result;

	std::vector<char> vec;
	vec.resize(length);
	char* data = vec.data();

	char** dataP = &data;
	unsigned char** dataPu = (unsigned char**)dataP;

	if (i2d_X509(m_certificate->x509, dataPu) < 0)
	{
		m_509 = std::string();
	}

	m_509 = Base64Convert::encode(vec.data(), vec.size());

	PKCS11_is_logged_in(m_slot, 0, &isLoggedIn);

	if (!loginRequired())
		m_prv_key = PKCS11_get_private_key(PKCS11_find_key(m_certificate));

}

bool PKCS11::loginRequired()
{
	return !isLoggedIn;
}

bool PKCS11::login(std::string pass)
{
	if (!loginRequired()) return false;

	bool success = PKCS11_login(m_slot, 0, pass.data()) == 0;

	if (success) {
		m_prv_key = PKCS11_get_private_key(PKCS11_find_key(m_certificate));
	}

	return success;
}

std::string PKCS11::x509certBase64() const
{
	return m_509;
}



std::string PKCS11::ssl_x509cert() const
{
	return "-----BEGIN CERTIFICATE-----\n" + m_509 + "-----END CERTIFICATE-----\n";
}

evp_pkey_st* PKCS11::getPrivateKey()
{
	return m_prv_key;
}

std::string PKCS11::sha1Digest(const std::string& data)
{
	auto canon = data.c_str();
	auto canonLength = data.size();

	unsigned char digest_value[EVP_MAX_MD_SIZE]; //the digest result value
	unsigned int dv_length; //the digest result length
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); //create msg digest context
	EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);  // Initialise the context (engine null value?)
	EVP_DigestUpdate(mdctx, canon, canonLength); //digesting
	EVP_DigestFinal_ex(mdctx, digest_value, &dv_length); //finalizing
	EVP_MD_CTX_free(mdctx); //free the context

	return std::string(reinterpret_cast<const char*>(&digest_value), dv_length);
}



std::string PKCS11::sha1digest64(const std::string& canonized)
{
	return toBase64(sha1Digest(canonized));
}

std::string PKCS11::toBase64(const std::string& data)
{
	return Base64Convert::encode(data.data(), data.size());
}

std::string PKCS11::getSignedValue64(const std::string& digestValue)
{
	auto signedInfo =
		"<SignedInfo xmlns=\"http://www.w3.org/2000/09/xmldsig#\">"
		"<CanonicalizationMethod Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"></CanonicalizationMethod>"
		"<SignatureMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#rsa-sha1\">""</SignatureMethod>"
		"<Reference URI=\"#signedContent\">"
		"<DigestMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\"></DigestMethod>"
		"<DigestValue>" + digestValue + "</DigestValue>"
		"</Reference>"
		"</SignedInfo>"
		;

	unsigned char* EncMsg{ nullptr };
	size_t MsgLenEnc{ 0 };

	auto pkey = getPrivateKey();

	if (!pkey) {
		return std::string();
	}

	RSA* rsa = EVP_PKEY_get1_RSA(pkey);

	EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();

	EVP_PKEY_assign_RSA(pkey, rsa);
	if (EVP_DigestSignInit(m_RSASignCtx, NULL, EVP_sha1(), NULL, pkey) <= 0) {
		return std::string();
	}
	if (EVP_DigestSignUpdate(m_RSASignCtx, signedInfo.c_str(), signedInfo.size()) <= 0) {
		return std::string();
	}
	if (EVP_DigestSignFinal(m_RSASignCtx, NULL, &MsgLenEnc) <= 0) {
		return std::string();
	}
	EncMsg = (unsigned char*)malloc(MsgLenEnc);
	if (EVP_DigestSignFinal(m_RSASignCtx, EncMsg, &MsgLenEnc) <= 0) {
		return std::string();
	}
	EVP_MD_CTX_destroy(m_RSASignCtx);

	return Base64Convert::encode(reinterpret_cast<const char*>(EncMsg), MsgLenEnc);
}

PKCS11::~PKCS11()
{
	//PKCS11_release_all_slots(ctx, m_slots, nslots);
}
