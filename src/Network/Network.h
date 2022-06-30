#pragma once
#include <string>

class AbstractReplyHandler;

class PKCS11;

namespace Network
{
	//the soap string is moved and stored on the heap
	void sendRequestToPis(
		const std::string& soapRequest,
		PKCS11& token,
		AbstractReplyHandler* handler,
		const char* header
	);
	void setRequestToNra(const std::string xmlRequest, AbstractReplyHandler* handler);
	void clearAccessCache();
	void unsubscribeHandler(AbstractReplyHandler* handler);
};

