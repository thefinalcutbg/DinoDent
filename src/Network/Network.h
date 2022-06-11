#pragma once
#include <string>

class AbstractReplyHandler;

class PKCS11;

namespace Network
{
	void sendRequestToPis(
		const std::string& soapRequest, 
		PKCS11& token, 
		AbstractReplyHandler *handler
	);

	void unsubscribeHandler(AbstractReplyHandler* handler);
};

