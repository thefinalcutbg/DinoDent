#include <string>

struct evp_pkey_st;
struct PKCS11_slot_st;
struct PKCS11_cert_st;

class PKCS11
{
	unsigned int nslots, ncerts{ 0 };
	PKCS11_slot_st* m_slots{ nullptr };
	PKCS11_cert_st* certs{ nullptr };

	PKCS11_slot_st* m_slot{ nullptr };
	PKCS11_cert_st* m_certificate{ nullptr };
	std::string m_509;
	evp_pkey_st* m_prv_key{ nullptr };

	int isLoggedIn{ false };


public:
	PKCS11();
	bool loginRequired();
	bool login(std::string pass);
	std::string x509certBase64() const;
	std::string ssl_x509cert() const;
	std::string getSignedValue64(const std::string& digestValue);
	evp_pkey_st* getPrivateKey();


	static std::string sha1digest64(const std::string& canonicalized);
	static std::string sha1Digest(const std::string& data);
	static std::string toBase64(const std::string& data);

	~PKCS11();


};

