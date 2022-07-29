#pragma once
#include <string>

class AbstractReplyHandler;
class PKCS11;

namespace NetworkManager
{

	void sendRequestToPis(
		const std::string& soapRequest,
		PKCS11& token,
		AbstractReplyHandler* handler,
		const char* header
	);

	void sendRequestToHis(
		const std::string& nhifMessage,
		AbstractReplyHandler* handler,
		const std::string& bearerToken
	);

	void sendRequestToNra(
		const std::string xmlRequest, 
		AbstractReplyHandler* handler
	);

	void requestChallenge();
	void requestToken(const std::string& signedChallenge);

	void clearAccessCache();
	void unsubscribeHandler(AbstractReplyHandler* handler);
};

