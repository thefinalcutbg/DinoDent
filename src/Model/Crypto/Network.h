#pragma once
#include <string>

class PKCS11;

namespace Network
{
	void sendRequestToPis(
		const std::string& soapRequest, 
		PKCS11& token, 
		void (*_returnFn)(const std::string& reply)
	);
};

