#include "Network.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslKey>
#include "PKCS11.h"
#include <qDebug>
#include "ReplyHandler.h"
#include <unordered_set>
#include "View/ModalDialogBuilder.h"

QNetworkAccessManager* s_manager {nullptr};
void (*s_returnFn)(const std::string& reply){ nullptr };

std::unordered_set<ReplyHandler*> handlers;


void Network::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                ReplyHandler* handler
                              )
{
    qDebug() << soapRequest.data();

    if (!s_manager) {
        s_manager = new QNetworkAccessManager();
    }


    handlers.insert(handler);

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SslProtocol::TlsV1_2);
    config.setLocalCertificate(QSslCertificate(token.ssl_x509cert().data()));
    config.setPrivateKey(QSslKey(Qt::HANDLE(token.takePrivateKey()), QSsl::KeyType::PrivateKey));
    
    QUrl url("https://pis.nhif.bg/ws/PISService");

    QNetworkRequest request(url);
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setRawHeader("SOAPAction", "\"http://pis.technologica.com/view\"");
    request.setRawHeader("accept", "\"application/xml\"");
    
    auto reply = s_manager->post(request, soapRequest.data());

    QObject::connect(reply, &QNetworkReply::finished, [=]{
            
                if (handlers.count(handler) == 0) return;

                handler->getReply(reply->readAll().data());
                Network::unsubscribeHandler(handler);
                s_manager->clearAccessCache();

        });

    QObject::connect(reply, &QNetworkReply::error, [=] {

        ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
        Network::unsubscribeHandler(handler);
        s_manager->clearAccessCache();

    });

    
}

void Network::unsubscribeHandler(ReplyHandler* handler)
{
    if (handlers.count(handler)) {
        handlers.erase(handler);
    }
}
