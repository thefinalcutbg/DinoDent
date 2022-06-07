#pragma once

#include <string>

struct evp_pkey_st;

namespace XmlSigner
{
	std::string signXML(const std::string& document, evp_pkey_st* prvKey, const std::string& x509);
	//not working, because I don't know how to register the URI
	std::string signSoapTemplate(const std::string& document, evp_pkey_st* prvKey, const std::string& x509);
	void cleanup();

};

