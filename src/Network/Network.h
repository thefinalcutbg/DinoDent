#pragma once
#include <string>

class ReplyHandler;

class PKCS11;

namespace Network
{
	void sendRequestToPis(
		const std::string& soapRequest, 
		PKCS11& token, 
		ReplyHandler *handler
	);

	void unsubscribeHandler(ReplyHandler* handler);
};

