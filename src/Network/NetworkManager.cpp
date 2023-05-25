#include "NetworkManager.h"

#include <set>

#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslKey>

#include "PKCS11.h"
#include "AbstractReplyHandler.h"
#include "Network/HIS/HisToken.h"
#include "View/ModalDialogBuilder.h"


QNetworkAccessManager* s_manager{ nullptr };
std::set<AbstractReplyHandler*> s_handlers;
int s_timeout = 15000;

QNetworkAccessManager* getManager() {

    if (!s_manager) {
        s_manager = new QNetworkAccessManager();
        s_manager->setTransferTimeout(s_timeout);

      /*
            QObject::connect(s_manager, &QNetworkAccessManager::sslErrors, [=] {
                qDebug() << "ERRORRRR";
           });
       */
    }

    return s_manager;
}



void NetworkManager::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                AbstractReplyHandler* handler,
                                const char* soapHeader
                              )
{

    auto manager = getManager();

    s_handlers.insert(handler);

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

    auto reply = manager->post(request, soapRequest.data());

    

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

            if (s_handlers.count(handler) == 0) return;

            std::string replyString = reply->readAll().toStdString();

            //the html error reply from PIS begins with <!DOCTYPE ...
            //otherwise it begins with <?xml ....
            
            if (replyString.size() && replyString[1] == '!') {
                replyString.clear(); 
            }
            
            handler->getReply(replyString);

            reply->deleteLater();
            
        });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

        QApplication::restoreOverrideCursor();

       // ModalDialogBuilder::showError("Неуспешна автентификация");

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
    auto manager = getManager();

    QApplication::setOverrideCursor(Qt::BusyCursor);

    s_handlers.insert(handler);

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

    QNetworkReply* reply = manager->post(request, nhifMessage.data());

    QObject::connect(reply, &QNetworkReply::finished,
        [=]() {

            if (s_handlers.count(handler) == 0) return;

            handler->getReply(reply->readAll().toStdString());

            reply->deleteLater();


        });
}

void NetworkManager::sendRequestToHisNoAuth(AbstractReplyHandler* handler, const std::string& nhifMessage, const std::string& urlAndServicePath)
{
    
    auto manager = getManager();

    QApplication::setOverrideCursor(Qt::BusyCursor);

    s_handlers.insert(handler);

    QUrl url(urlAndServicePath.c_str());

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setProtocol(QSsl::SslProtocol::TlsV1_3);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply* reply = manager->post(request, nhifMessage.data());

    QObject::connect(reply, &QNetworkReply::finished,
        [=]() {

            if (s_handlers.count(handler) == 0) return;

            handler->getReply(reply->readAll().toStdString());

            reply->deleteLater();


        });
}

void NetworkManager::sendRequestToNra(const std::string xmlRequest, AbstractReplyHandler* handler)
{
    auto manager = getManager();

    s_handlers.insert(handler);

    QNetworkRequest request(QUrl("https://nraapp03.nra.bg:4445/nhifrcz/NhifStatus5Port"));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    
    auto reply = manager->post(request, xmlRequest.data());
    

    QApplication::setOverrideCursor(Qt::BusyCursor);

    QObject::connect(reply, &QNetworkReply::errorOccurred,
        [=](QNetworkReply::NetworkError code)
        {
            qDebug() << "ReplyError: " << code;
        }
    );

    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            if (s_handlers.count(handler) == 0) return;

            unsubscribeHandler(handler);
            
            std::string replyString = reply->readAll().toStdString();

            if (replyString.empty() || replyString[1] == '!') {
                replyString.clear();
            }

            handler->getReply(replyString);

            reply->deleteLater();

        });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

            ModalDialogBuilder::showError("Неуспешна автентификация");
            handler->getReply("");
            NetworkManager::unsubscribeHandler(handler);
            reply->deleteLater();

        });


}

void NetworkManager::sendRequestToNssi(const std::string xmlRequest, AbstractReplyHandler* handler)
{
    auto manager = getManager();

    s_handlers.insert(handler);

    QNetworkRequest request(QUrl("https://wsgp.nssi.bg"));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setHeader(QNetworkRequest::ContentLengthHeader, "length");
    request.setRawHeader("SOAPAction", "\"http://tempuri.org/GetNSSIPensionsData\"");

    auto reply = manager->post(request, xmlRequest.data());


    QApplication::setOverrideCursor(Qt::BusyCursor);

    QObject::connect(reply, &QNetworkReply::errorOccurred,
        [=](QNetworkReply::NetworkError code)
        {
            qDebug() << "ReplyError: " << code;
        }
    );

    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            if (s_handlers.count(handler) == 0) return;
            unsubscribeHandler(handler);

            std::string replyString = reply->readAll().toStdString();

            handler->getReply(replyString);

            reply->deleteLater();

        });

        QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

            ModalDialogBuilder::showError("Неуспешна автентификация");
            handler->getReply("");
            NetworkManager::unsubscribeHandler(handler);
            reply->deleteLater();

        });


}

void NetworkManager::requestChallenge()
{

    auto manager = getManager();

    QUrl url("https://auth.his.bg/token");

    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished,
        [=] {
            HisToken::setChallengeMessage(reply->readAll().toStdString());

            reply->deleteLater();
        });
}

void NetworkManager::requestToken(const std::string& signedChallenge)
{
    auto manager = getManager();

    QUrl url("https://auth.his.bg/token");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply* reply = manager->post(request, signedChallenge.c_str());
    
    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            HisToken::setAuthRepy(reply->readAll().toStdString());

            reply->deleteLater();

        });
}


void NetworkManager::clearAccessCache()
{
    getManager()->clearAccessCache();
}

void NetworkManager::unsubscribeHandler(AbstractReplyHandler* handler)
{
    if (s_handlers.count(handler)) {
        s_handlers.erase(handler);
    }

    if (s_handlers.empty()) {
        QApplication::restoreOverrideCursor();
    }
}


QNetworkReply* NetworkManager::simpleRequest(const char* url)
{
    QNetworkRequest request(QUrl{url});
    
    return getManager()->get(request);
}

void NetworkManager::setTimeout(int seconds)
{
    s_timeout = seconds ? seconds * 1000 : 15000;
    
    if (s_manager) {
        s_manager->setTransferTimeout(s_timeout);
    }
}
