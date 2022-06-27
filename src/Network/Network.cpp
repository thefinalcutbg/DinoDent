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

std::unordered_set<AbstractReplyHandler*> handlers;


void Network::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                AbstractReplyHandler* handler,
                                int timeout
                              )
{


    if (!s_manager) {
        s_manager = new QNetworkAccessManager();
       // s_manager->setAutoDeleteReplies(true); //produces crashes sometimes lol
        QObject::connect(s_manager, &QNetworkAccessManager::sslErrors, [=] {
                 qDebug() << "ERRORRRR";
            });
    }

    qDebug() << "inserting handler";
    handlers.insert(handler);
    qDebug() << "HANDLERS SIZE: " << handlers.size();

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    //config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SslProtocol::TlsV1_2);
    config.setLocalCertificate(QSslCertificate(token.pem_x509cert().data()));
    config.setPrivateKey(QSslKey(Qt::HANDLE(token.takePrivateKey()), QSsl::KeyType::PrivateKey));

    QNetworkRequest request(QUrl("https://pis.nhif.bg/ws/PISService"));
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setRawHeader("SOAPAction", "\"http://pis.technologica.com/view\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setTransferTimeout(timeout);



    auto reply = s_manager->post(request, soapRequest.data());

    QApplication::setOverrideCursor(Qt::BusyCursor);

    QObject::connect(reply, &QNetworkReply::errorOccurred,
        [=](QNetworkReply::NetworkError code)
        {
            qDebug() << "ReplyError: " << code;
        }
    );

    QObject::connect(reply, &QNetworkReply::finished, 
        [=] {
            
            QApplication::restoreOverrideCursor();

            if (handlers.count(handler) == 0) return;
            unsubscribeHandler(handler);

            std::string replyString = reply->readAll().toStdString();

           // qDebug() << "REPLY: ";
           // qDebug() << replyString.c_str();

            //the html error reply from the server begins with <!DOCTYPE ...
            //otherwise it begins with <?xml ....
            if (replyString.empty() || replyString[1] == '!') {
                replyString.clear();
            }

            handler->getReply(replyString);

            reply->deleteLater();
            
        });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

        QApplication::restoreOverrideCursor();

        ModalDialogBuilder::showError(u8"Неуспешна автентификация");
        handler->getReply("");
        Network::unsubscribeHandler(handler);
        reply->deleteLater();

    });


    
}

void Network::clearAccessCache()
{
    if (s_manager == nullptr) return;

    s_manager->clearAccessCache();
}

void Network::unsubscribeHandler(AbstractReplyHandler* handler)
{
    if (handlers.count(handler)) {
        handlers.erase(handler);
    }
}
