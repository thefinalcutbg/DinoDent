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
#include "GlobalSettings.h"

#include <qdebug.h>

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

void postRequest(const QNetworkRequest& request, AbstractReplyHandler* handler, const std::string& query) {

    if (GlobalSettings::showRequestsEnabled()) {
        ModalDialogBuilder::showMultilineDialog(query, "Заявка");
    }

    auto reply = getManager()->post(request, query.data());

    s_handlers.insert(handler);

    QApplication::setOverrideCursor(Qt::BusyCursor);

    QObject::connect(reply, &QNetworkReply::errorOccurred,
        [=](QNetworkReply::NetworkError code)
        {
            QApplication::restoreOverrideCursor();

            //ModalDialogBuilder::showError("Възникна грешка");
        }
    );

    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            QApplication::restoreOverrideCursor();

            auto replyStr = reply->readAll().toStdString();

            if (GlobalSettings::showRepliesEnabled()) {
                ModalDialogBuilder::showMultilineDialog(replyStr, "Отговор");
            }

            if (s_handlers.count(handler) == 0) return;

            handler->getReply(replyStr);

            reply->deleteLater();

        });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=] {

            QApplication::restoreOverrideCursor();

            ModalDialogBuilder::showError("Неуспешна автентификация");

            if (s_handlers.count(handler) == 0) return;

            handler->getReply("");

            reply->deleteLater();

        });
}

void NetworkManager::sendRequestToPis(
                                const std::string& soapRequest,
                                PKCS11& token,
                                AbstractReplyHandler* handler,
                                const char* soapHeader
                              )
{
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SslProtocol::TlsV1_2);
    config.setLocalCertificate(QSslCertificate(token.pem_x509cert().data()));
    config.setPrivateKey(QSslKey(Qt::HANDLE(token.takePrivateKey()), QSsl::KeyType::PrivateKey));

    QNetworkRequest request(QUrl("https://pis.nhif.bg/ws/PISService"));
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setRawHeader("SOAPAction", soapHeader);
    request.setRawHeader("accept", "\"application/xml\"");

    postRequest(request, handler, soapRequest);
        
}

void NetworkManager::sendRequestToHis(
                    AbstractReplyHandler* handler,
                    const std::string& nhifMessage,
                    const std::string& token,
                    const std::string& urlAndServicePath
)
{

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

    postRequest(request, handler, nhifMessage);

}

void NetworkManager::sendRequestToHisNoAuth(AbstractReplyHandler* handler, const std::string& nhifMessage, const std::string& urlAndServicePath)
{
    QUrl url(urlAndServicePath.c_str());

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setProtocol(QSsl::SslProtocol::TlsV1_3);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");

    postRequest(request, handler, nhifMessage);
}

void NetworkManager::sendRequestToNra(const std::string xmlRequest, AbstractReplyHandler* handler)
{
    QNetworkRequest request(QUrl("https://nraapp03.nra.bg:4445/nhifrcz/NhifStatus5Port"));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    
    postRequest(request, handler, xmlRequest);


}

void NetworkManager::sendRequestToNssi(const std::string xmlRequest, AbstractReplyHandler* handler)
{
    QNetworkRequest request(QUrl("https://wsgp.nssi.bg"));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "text/xml;charset=\"utf-8\"");
    request.setHeader(QNetworkRequest::ContentLengthHeader, "length");
    request.setRawHeader("SOAPAction", "\"http://tempuri.org/GetNSSIPensionsData\"");

    postRequest(request, handler, xmlRequest);

}

void NetworkManager::sendTelemetry(const std::string& json)
{
    return;

    QNetworkRequest request(QUrl(""));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    getManager()->post(request, json.c_str());
}

void NetworkManager::requestChallenge()
{
    auto manager = getManager();

    QUrl url("https://auth.his.bg/token");

    QNetworkRequest request(url);

    QNetworkReply* reply = manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            auto replyStr = reply->readAll().toStdString();

            if (GlobalSettings::showRepliesEnabled()) {
                ModalDialogBuilder::showMultilineDialog(replyStr, "Отговор - challange");
            }

            HisToken::setChallengeMessage(replyStr);

            reply->deleteLater();
        });
}

void NetworkManager::requestToken(const std::string& signedChallenge)
{
    if (GlobalSettings::showRequestsEnabled()) {
        ModalDialogBuilder::showMultilineDialog(signedChallenge, "Заявка за токен");
    }

    auto manager = getManager();

    QUrl url("https://auth.his.bg/token");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/xml;charset=\"utf-8\"");
    request.setRawHeader("accept", "\"application/xml\"");
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply* reply = manager->post(request, signedChallenge.c_str());
    
    QObject::connect(reply, &QNetworkReply::finished,
        [=] {

            auto replyStr = reply->readAll().toStdString();

            if (GlobalSettings::showRepliesEnabled()) {
                ModalDialogBuilder::showMultilineDialog(replyStr, "Отговор за токен");
            }

            HisToken::setAuthRepy(replyStr);

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
