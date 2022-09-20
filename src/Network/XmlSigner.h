#pragma once

#include <string>

struct evp_pkey_st;

namespace XmlSigner
{
	std::string signPisQuery(const std::string& query, evp_pkey_st* prvKey, const std::string& x509);
	std::string signNhifMessage(const std::string& message, evp_pkey_st* prvKey, const std::string x509);
	void cleanup();

};

