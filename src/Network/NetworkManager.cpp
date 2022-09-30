#include "NetworkManager.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslKey>
#include "PKCS11.h"
#include "AbstractReplyHandler.h"
#include "Network/HIS/HisToken.h"
#include <set>
#include "View/ModalDialogBuilder.h"
#include <QApplication>

QNetworkAccessManager* s_manager {nullptr};

std::set<AbstractReplyHandler*> handlers;


void NetworkManager::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                AbstractReplyHandler* handler,
                                const char* soapHeader
                              )
{


    if (!s_manager) {
        s_manager = new QNetworkAccessManager();
       // s_manager->setAutoDeleteReplies(true); //produces crashes sometimes lol
        QObject::connect(s_manager, &QNetworkAccessManager::sslErrors, [=] {
                 qDebug() << "ERRORRRR";
            });
    }

    handlers.insert(handler);

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    //config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SslProtocol::TlsV1_2);
    config.setLocalCertificate(QSslCertificate(token.pem_x509cert().data()));
    config.setPrivateKey(QSslKey(Qt::HANDLE(token.takePrivateKey()), QSsl::KeyType::PrivateKey));

    QNetworkRequest request(QUrl("https://pis.nhif.bg/ws/PISService"));
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setRawHeader("SOAPAction", soapHeader);
    request.setRawHeader("accept", "\"application/xml\"");

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

            std::string replyString = reply->readAll().toStdString();

            //the html error reply from PIS begins with <!DOCTYPE ...
            //otherwise it begins with <?xml ....
            if (replyString[1] == '!') {
                replyString.clear(); 
            }
            
            handler->getReply(replyString);

            reply->deleteLater();
            
        });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

        QApplication::restoreOverrideCursor();

        ModalDialogBuilder::showError(u8"Неуспешна автентификация");

        handler->getReply("");

        reply->deleteLater();

    });


    
}
void NetworkManager::sendRequestToHis(
                    AbstractReplyHandler* handler,
                    const std::string& nhifMessage,
                    const std::string& token,
                    const std::string& urlAndServicePath
)
{
    if (!s_manager) {s_manager = new QNetworkAccessManager(); }

    QApplication::setOverrideCursor(Qt::BusyCursor);

    handlers.insert(handler);

    QUrl url(urlAndServicePath.c_str());

    QString authValue = "Bearer ";
    authValue.append(token.c_str());

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setProtocol(QSsl::SslProtocol::TlsV1_3);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("Authorization",  authValue.toUtf8());

    QNetworkReply* reply = s_manager->post(request, nhifMessage.data());

    QObject::connect(reply, &QNetworkReply::finished,
        [=]() {

            QApplication::restoreOverrideCursor();

            if (handlers.count(handler) == 0) return;

            handler->getReply(reply->readAll().toStdString());

            reply->deleteLater();


        });
}

void NetworkManager::sendRequestToHisNoAuth(AbstractReplyHandler* handler, const std::string& nhifMessage, const std::string& urlAndServicePath)
{

    if (!s_manager) { s_manager = new QNetworkAccessManager(); }

    QApplication::setOverrideCursor(Qt::BusyCursor);

    handlers.insert(handler);

    QUrl url(urlAndServicePath.c_str());

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setProtocol(QSsl::SslProtocol::TlsV1_3);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply* reply = s_manager->post(request, nhifMessage.data());

    QObject::connect(reply, &QNetworkReply::finished,
        [=]() {

            QApplication::restoreOverrideCursor();

            if (handlers.count(handler) == 0) return;

            handler->getReply(reply->readAll().toStdString());

            reply->deleteLater();


        });
}

void NetworkManager::sendRequestToNra(const std::string xmlRequest, AbstractReplyHandler* handler)
{
    if (!s_manager) {
        s_manager = new QNetworkAccessManager();
        // s_manager->setAutoDeleteReplies(true); //produces crashes sometimes lol
        QObject::connect(s_manager, &QNetworkAccessManager::sslErrors, [=] {
            qDebug() << "ERRORRRR";
            });
    }

    handlers.insert(handler);

    QNetworkRequest request(QUrl("https://nraapp03.nra.bg:4445/nhifrcz/NhifStatus5Port"));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    
    auto reply = s_manager->post(request, xmlRequest.data());
    

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
            NetworkManager::unsubscribeHandler(handler);
            reply->deleteLater();

        });


}

void NetworkManager::requestChallenge()
{

    if (!s_manager) {
        s_manager = new QNetworkAccessManager();
        // s_manager->setAutoDeleteReplies(true); //produces crashes sometimes lol
        QObject::connect(s_manager, &QNetworkAccessManager::sslErrors, [=] {
            qDebug() << "ERRORRRR";
            });
    }


    QUrl url("https://auth.his.bg/token");

    QNetworkRequest request(url);
    QNetworkReply* reply = s_manager->get(request);

    QApplication::setOverrideCursor(Qt::BusyCursor);

    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            QApplication::restoreOverrideCursor();

            HisToken::setChallengeMessage(reply->readAll().toStdString());

            reply->deleteLater();

        });
}

void NetworkManager::requestToken(const std::string& signedChallenge)
{
    QUrl url("https://auth.his.bg/token");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply* reply = s_manager->post(request, signedChallenge.c_str());
    
    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            QApplication::restoreOverrideCursor();

            HisToken::setAuthRepy(reply->readAll().toStdString());

            reply->deleteLater();

        });
}


void NetworkManager::clearAccessCache()
{
    if (s_manager == nullptr) return;

    s_manager->clearAccessCache();
}

void NetworkManager::unsubscribeHandler(AbstractReplyHandler* handler)
{
    if (handlers.count(handler)) {
        handlers.erase(handler);
    }
}
