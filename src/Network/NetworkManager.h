#pragma once
#include <string>

class AbstractReplyHandler;
class PKCS11;
class QNetworkReply;
class QNetworkAccessManager;

namespace NetworkManager
{

	void sendRequestToPis(
		const std::string& soapRequest,
		PKCS11& token,
		AbstractReplyHandler* handler,
		const char* header
	);

	void sendRequestToHis(
		AbstractReplyHandler* handler,
		const std::string& nhifMessage,
		const std::string& token,
		const std::string& urlAndServicePath
	);

	void sendRequestToHisNoAuth(
		AbstractReplyHandler* handler,
		const std::string& nhifMessage,
		const std::string& urlAndServicePath
	);

	void sendRequestToNra(
		const std::string xmlRequest, 
		AbstractReplyHandler* handler
	);

	void sendRequestToNssi(
		const std::string xmlRequest,
		AbstractReplyHandler* handler
	);

	void sendTelemetry(const std::string& json);

	void requestChallenge();
	void requestToken(const std::string& signedChallenge);

	void clearAccessCache();
	void unsubscribeHandler(AbstractReplyHandler* handler);

	QNetworkReply* simpleRequest(const char* url);

	QNetworkAccessManager* getManager();

	void setTimeout(int seconds);
};

