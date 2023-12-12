#include <string>
#include <vector>

struct evp_pkey_st;
struct PKCS11_slot_st;
struct PKCS11_cert_st;

class PKCS11
{
	PKCS11_cert_st* m_certificate{ nullptr };
	std::string m_509;
	evp_pkey_st* m_prv_key{ nullptr };

public:
	PKCS11();
	bool hsmLoaded();

	bool loginRequired();
	bool login(const std::string& pass);
	const std::string& pem_x509cert() const;
	evp_pkey_st* takePrivateKey();

	static void cleanup();

	~PKCS11();

};


