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
	bool m_loaded{ false };
	std::string m_subjectName;
	std::string m_issuer;


public:
	PKCS11();
	bool hsmLoaded();
	const std::string& subjectName();
	const std::string& issuer();
	bool loginRequired();
	bool login(std::string pass);
	std::string x509certBase64() const;
	std::string pem_x509cert() const;
	std::string getSignedValue64(const std::string& digestValue);
	//assumes the prv_key ownership has been passed on
	evp_pkey_st* takePrivateKey();

	static void unloadModule();
	static std::string sha1digest64(const std::string& canonicalized);
	static std::string sha1Digest(const std::string& data);
	static std::string toBase64(const std::string& data);

	~PKCS11();


};

