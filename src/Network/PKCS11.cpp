#include "PKCS11.h"
#include <libp11\libp11.h>
#include <vector>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <filesystem>

X509Details::X509Details(x509_st* cert, const std::string& driverPath) :
	driverPath(driverPath)
{
	//setting the x509_pem value
	BIO* bio = BIO_new(BIO_s_mem());

	if (!PEM_write_bio_X509(bio, cert)) {
		BIO_free(bio);
		return;
	}

	char* data = nullptr;
	long length = BIO_get_mem_data(bio, &data);

	x509_pem = std::string(data, length);

	OPENSSL_free(bio);

	//getting subject name
	X509_NAME* subjectName = X509_get_subject_name(cert);

	//lambda fn for extracting data from subject name
	auto getData = [](X509_NAME* name, int nid) {

		int cnIndex = X509_NAME_get_index_by_NID(name, nid, -1);

		if (cnIndex == -1) return std::string();

		X509_NAME_ENTRY* cnEntry = X509_NAME_get_entry(name, cnIndex);

		if (!cnEntry) return std::string();

		ASN1_STRING* cnASN1 = X509_NAME_ENTRY_get_data(cnEntry);

		if (!cnASN1) return std::string();

		const unsigned char* cnStr = ASN1_STRING_get0_data(cnASN1);

		if (!cnStr) return std::string();

		return std::string(reinterpret_cast<const char*>(cnStr));
	};

	name = getData(subjectName, NID_commonName);
	organization = getData(subjectName, NID_organizationName);

	X509_NAME* issuerName = X509_get_issuer_name(cert);

	issuer = getData(issuerName, NID_commonName);


	//lambda fn for extracting the 8601 dates
	auto get8601 = [](const ASN1_TIME* notAfter)
	{
		if (!notAfter) {
			return std::string();
		}

		struct tm tm = {};

		if (ASN1_TIME_to_tm(notAfter, &tm) != 1) {
			return std::string();
		}

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
		return oss.str();
	};

	//getting the valid dateTo
	validTo8601 = get8601(X509_get0_notAfter(cert));
	validFrom8601 = get8601(X509_get0_notBefore(cert));

}

bool X509Details::isValid() const
{
	if (x509_pem.empty()) { return false; }

	auto now = std::chrono::system_clock::now();

	std::string currentDateTime8601 = std::format("{:%FT%TZ}", now);

	return currentDateTime8601 < validTo8601
		&& currentDateTime8601 > validFrom8601
		;

}

PKCS11_CTX* ctx{ nullptr };

std::vector<std::string> s_driverPaths;
std::pair<X509Details, std::string> s_lastCred;

void PKCS11::setDriverPaths(const std::vector<std::string>& driverPaths)
{
	s_driverPaths.clear();

	for (auto& path : driverPaths)
	{
		if (!std::filesystem::exists(path)) continue;

		s_driverPaths.push_back(path);
	}
}

void PKCS11::cleanup()
{
	s_lastCred = std::make_pair(X509Details(), std::string());

	if (ctx)
	{
		PKCS11_CTX_unload(ctx);
		ctx = nullptr;
	}
}


//helper functions
PKCS11_cert_st* PKCS11::loadCertificate(const X509Details& cert){

	//hopefully no need for error checking
	PKCS11_CTX_load(ctx, cert.driverPath.data());
	PKCS11_enumerate_slots(ctx, &pslots, &nslots);

	current_slot = PKCS11_find_next_token(ctx, pslots, nslots, nullptr);

	//iterrating slots
	while (current_slot != NULL) {

		PKCS11_cert_st* certs{ nullptr };
		unsigned int ncerts{ 0 };

		PKCS11_enumerate_certs(current_slot->token, &certs, &ncerts);

		//finding a valid x509_pem
		if (ncerts == 0) continue;

		//iterrating certificates
		for (unsigned int i = 0; i < ncerts; i++)
		{
			auto currentCert = X509Details(certs[i].x509, cert.driverPath);

			if (currentCert == cert) {
				return &certs[i];
			}
		}

		current_slot = PKCS11_find_next_token(ctx, pslots, nslots, current_slot);
	}

	return nullptr;

}

std::vector<X509Details> PKCS11::getCertList(bool returnFirst) {

	std::vector<X509Details> certList;

	//iterrating all dirver paths
	for (auto& path : s_driverPaths)
	{
		if (PKCS11_CTX_load(ctx, path.data()) == -1) continue;

		unsigned int nslots_temp{ 0 };
		PKCS11_slot_st* pslots_temp{ nullptr };

		if (PKCS11_enumerate_slots(ctx, &pslots_temp, &nslots_temp) == -1)  continue;

		PKCS11_slot_st* slot = PKCS11_find_token(ctx, pslots_temp, nslots_temp);

		//iterrating slots
		while (slot != NULL) {

			PKCS11_cert_st* certs{ nullptr };
			unsigned int ncerts{ 0 };

			PKCS11_enumerate_certs(slot->token, &certs, &ncerts);

			//finding a valid x509_pem
			if (ncerts == 0) continue;

			//iterrating certificates
			for (unsigned int i = 0; i < ncerts; i++)
			{
				auto currentCert = X509Details(certs[i].x509, path);

				if (returnFirst && currentCert.isValid()) {
					return std::vector{ currentCert };
				}

				//if this is the certificate that is last used, chose this one
				if (s_lastCred.first == currentCert)
				{
					certList.clear();
					certList.push_back(s_lastCred.first);
					return certList;
				}

				//if the certificate is not already added
				if (std::find(certList.begin(), certList.end(), currentCert) == certList.end()) {
					certList.push_back(currentCert);
				}
			}

			slot = PKCS11_find_next_token(ctx, pslots_temp, nslots_temp, slot);
		}

		PKCS11_release_all_slots(ctx, pslots_temp, nslots_temp);
	}

	//filter the list with valid certs only?

	return certList;

}

PKCS11::PKCS11(
	std::function<std::string(const X509Details& data)> passCallback,
	std::function<int(const std::vector<X509Details>& certList)> certCallback
)
{
	if (!ctx) {
		ctx = PKCS11_CTX_new();
	}

	auto certList = getCertList(certCallback == nullptr);

	//if the list is empty, no certificates are found

	if (certList.empty()) { return; }

	int certIdx = 0;
	
	//if no certCallback is provided, we always take the first one
	if(certList.size() > 1 && certCallback)
	{
		certIdx = certCallback(certList);

		if (certIdx < 0 || certIdx >= certList.size()) { 
			m_state = NoCertificateChosen;
			return; 
		} 
	}

	m_certificate = loadCertificate(certList[certIdx]);
	m_cert_details = certList[certIdx];

	if (m_certificate == nullptr) {
		m_state = State::NoCertificate;
		return;
	}

	auto key = PKCS11_find_key(m_certificate);

	if (key) {
		m_prv_key = PKCS11_get_private_key(key);

		//if the driver gives access to prv_key because of pervious session
		if (m_prv_key) {
			m_state = State::LoggedIn;
			return;
		}
	}

	auto& [lastCert, lastPass] = s_lastCred;
	

	//try auto login with last ceredntials
	if (m_cert_details == lastCert
		&& lastPass.size()
		&& PKCS11_login(current_slot, 0, lastPass.data()) == 0
		)
	{
		m_prv_key = PKCS11_get_private_key(PKCS11_find_key(m_certificate));

		if (m_prv_key) {
			m_state = State::JustLoggedIn;
			return;
		}
	}

	//reset the last credentials
	s_lastCred = std::make_pair(X509Details(), std::string());

	if (!passCallback) {
		m_state = State::LoginAborted;
		return;
	}

	//getting from callback
	auto pass = passCallback(m_cert_details);

	if (pass.empty()) {
		m_state = LoginAborted;
		return;
	}

	if (PKCS11_login(current_slot, 0, pass.data()) == 0) {

		//caching the last used certificate
		s_lastCred = std::make_pair(m_cert_details, pass);

		m_prv_key = PKCS11_get_private_key(PKCS11_find_key(m_certificate));

		m_state = State::JustLoggedIn;

		return;
	}

	m_state = State::LoginFailed;
}

evp_pkey_st* PKCS11::takePrivateKey(bool takeOnwership)
{
	if (takeOnwership) {
		prv_key_owned = false;
	}

	return m_prv_key;
}

x509_st* PKCS11::x509ptr()
{
	{ return m_certificate->x509; }
}


PKCS11::~PKCS11()
{
//	if (!prv_key_owned) return;

//	PKCS11_release_all_slots(ctx, pslots, nslots);

}
