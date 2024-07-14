#include "PKCS11.h"
#include <libp11.h>
#include <vector>
#include "Base64Convert.h"
#include <filesystem>
#include <QSslCertificate>
#include "GlobalSettings.h"

PKCS11_CTX* ctx{ nullptr };
unsigned int nslots{ 0 };
PKCS11_slot_st* pslots{ nullptr };
PKCS11_slot_st* current_slot{ nullptr };

bool isValidCertificate(PKCS11_cert_st* cert)
{
	int length = i2d_X509(cert->x509, 0);

	std::vector<char> vec;
	vec.resize(length);
	char* data = vec.data();

	char** dataP = &data;
	unsigned char** dataPu = (unsigned char**)dataP;

	if (i2d_X509(cert->x509, dataPu) < 0)
	{
		return false;
	}

	std::string certResult = "-----BEGIN CERTIFICATE-----\n";

	certResult.append(Base64Convert::encode(vec.data(), vec.size()));

	certResult.append("\n-----END CERTIFICATE-----");

	QSslCertificate qCert(certResult.data());

	const QDateTime currentTime = QDateTime::currentDateTime();

	return
		!qCert.isNull() &&
		currentTime <= qCert.expiryDate() &&
		currentTime >= qCert.effectiveDate()
		;
}

PKCS11::PKCS11()
{
	if (!ctx) {
		ctx = PKCS11_CTX_new();
	}

	for (auto& module : GlobalSettings::pkcs11PathList())
	{   
		if (!std::filesystem::exists(module)) continue;

		if (PKCS11_CTX_load(ctx, module.data()) == -1) continue;

		if (PKCS11_enumerate_slots(ctx, &pslots, &nslots) == -1)  continue;

		current_slot = PKCS11_find_token(ctx, pslots, nslots);

		if (current_slot == nullptr) continue;

		driver = std::filesystem::path(module).filename().string();

		PKCS11_cert_st* certs{ nullptr };
		unsigned int ncerts{ 0 };

		PKCS11_enumerate_certs(current_slot->token, &certs, &ncerts);

		//finding a valid certificate
		if (ncerts == 0) continue;

        for (unsigned int i = 0; i < ncerts; i++)
		{
			m_certificate = &certs[i];

			if (isValidCertificate(m_certificate)) break;
		}

		if (m_certificate) {
			break;
		}
	
	}

	if (m_certificate == nullptr) return;

	int length = i2d_X509(m_certificate->x509, 0);

	std::vector<char> vec;
	vec.resize(length);
	char* data = vec.data();

	char** dataP = &data;
	unsigned char** dataPu = (unsigned char**)dataP;

	if (i2d_X509(m_certificate->x509, dataPu) < 0)
	{
		m_509 = std::string();
		return;
	}

	m_509 = "-----BEGIN CERTIFICATE-----\n" + Base64Convert::encode(vec.data(), vec.size()) + +"\n-----END CERTIFICATE-----";

	auto key = PKCS11_find_key(m_certificate);

	if(key) m_prv_key = PKCS11_get_private_key(key);
}

bool PKCS11::hsmLoaded()
{
	return m_certificate != nullptr;
}

bool PKCS11::loginRequired()
{
	return m_prv_key == nullptr;
}

bool PKCS11::login(const std::string& pass)
{
	if (!loginRequired()) return true;

	bool success = PKCS11_login(current_slot, 0, pass.data()) == 0;

	if (success) {
		m_prv_key = PKCS11_get_private_key(PKCS11_find_key(m_certificate));
	}

	return success;
}

const std::string& PKCS11::pem_x509cert() const
{
	return m_509;
}

evp_pkey_st* PKCS11::takePrivateKey()
{
	return m_prv_key;
}

x509_st* PKCS11::x509ptr()
{
	return m_certificate->x509;
}

void PKCS11::cleanup()
{
	if (nslots) {
		PKCS11_release_all_slots(ctx, pslots, nslots);
		nslots = 0;
		pslots = 0;
	}

	current_slot = nullptr;

	if (ctx)
	{
		PKCS11_CTX_unload(ctx);
	}
}

PKCS11::~PKCS11()
{
	/*
		both xmlsec and qt take ownership of the private key
		so releasing the slots causes a crash
	*/
	//PKCS11_release_all_slots(ctx, m_slots, nslots);
}
