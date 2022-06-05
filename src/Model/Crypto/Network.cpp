#include "Network.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslKey>
#include "PKCS11.h"
#include <qDebug>
QNetworkAccessManager* s_manager {nullptr};
void (*s_returnFn)(const std::string& reply){ nullptr };

void managerInit()
{
	if (!s_manager) {
		s_manager = new QNetworkAccessManager();

		QObject::connect(s_manager, &QNetworkAccessManager::finished,
			[=](QNetworkReply* r) {
                if (r->error()) return;

                if (s_returnFn) {
                    s_returnFn(r->readAll().toStdString());
                }
                 
                s_manager->clearAccessCache();
			});
	}
}

void Network::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                void (*_returnFn)(const std::string& reply)
                              )
{

    qDebug() << soapRequest.data();

    managerInit();
    s_returnFn = _returnFn;

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SslProtocol::TlsV1_2);
    config.setLocalCertificate(QSslCertificate(token.ssl_x509cert().data()));
    config.setPrivateKey(QSslKey(Qt::HANDLE(token.getPrivateKey()), QSsl::KeyType::PrivateKey));

    QUrl url("https://pis.nhif.bg/ws/PISService");

    QNetworkRequest request(url);
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setRawHeader("SOAPAction", "\"http://pis.technologica.com/view\"");
    request.setRawHeader("accept", "\"application/xml\"");

    s_manager->post(request, soapRequest.data());
}
