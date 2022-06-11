#include "Network.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslKey>
#include "PKCS11.h"
#include <qDebug>
#include "ReplyHandlers/AbstractReplyHandler.h"
#include <unordered_set>
#include "View/ModalDialogBuilder.h"
#include <QApplication>



QNetworkAccessManager* s_manager {nullptr};
void (*s_returnFn)(const std::string& reply){ nullptr };

std::unordered_set<AbstractReplyHandler*> handlers;


void Network::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                AbstractReplyHandler* handler
                              )
{


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

    QApplication::setOverrideCursor(Qt::BusyCursor);

    QObject::connect(reply, &QNetworkReply::finished, [=]{
            
        QApplication::setOverrideCursor(Qt::ArrowCursor);

        if (handlers.count(handler) == 0) return;

        std::string replyString = reply->readAll().toStdString();

        if (replyString.empty())
        {
            ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
        }
        else
        {
            handler->getReply(replyString);
        }

        Network::unsubscribeHandler(handler);
        s_manager->clearAccessCache();

        });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

        QApplication::setOverrideCursor(Qt::ArrowCursor);

        ModalDialogBuilder::showError(u8"Неуспешна автентификация");
        Network::unsubscribeHandler(handler);
        s_manager->clearAccessCache();

    });

    
}

void Network::unsubscribeHandler(AbstractReplyHandler* handler)
{

    if (handlers.count(handler)) {
        handlers.erase(handler);
    }

    
}
