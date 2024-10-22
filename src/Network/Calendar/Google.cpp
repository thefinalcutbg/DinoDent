#include "Google.h"

#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include "Network/NetworkManager.h"
#include "Presenter/CalendarPresenter.h"
#include "Credentials.h" //comment this out
#include <QTimer>
QString timeZoneOffset;
CalendarPresenter* s_reciever;
QTimer* timer;

void Google::setReciever(CalendarPresenter* p)
{
    s_reciever = p;
}

CalendarPresenter* getReciever() {
    return s_reciever;
}

QOAuth2AuthorizationCodeFlow* getAuth(bool reinitialize = false) {

    static QOAuth2AuthorizationCodeFlow* auth = nullptr;

    if (auth) {

        if (reinitialize) {
            delete auth;
            auth = nullptr;
            timer = nullptr;
            return getAuth();
        }

        return auth;
    }

    auth = new QOAuth2AuthorizationCodeFlow();

    timer = new QTimer(auth);

  //  auth->setNetworkAccessManager(NetworkManager::getManager());

    auth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    auth->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

    //fill out these after you register in google developer console:
    auth->setClientIdentifier(Credentials::ClientIdentifier);
    auth->setClientIdentifierSharedKey(Credentials::ClientIdentifierSharedKey);

    const QUrl redirectUri("http://localhost");
    const auto port = 1234;

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, auth);
    
    auth->setReplyHandler(replyHandler);
    
    auth->setScope("https://www.googleapis.com/auth/calendar");
    QObject::connect(replyHandler, &QOAuthHttpServerReplyHandler::tokenRequestErrorOccurred, replyHandler,
        [&](QAbstractOAuth::Error error, const QString& errorString) {
            
            auto reciever = getReciever();

            if (!reciever) return;

            if (errorString.startsWith("Error transferring")) 
            {
                reciever->restoreCredentials();
                
                return;
            }

            reciever->disconnected();
            
     });
    QObject::connect(auth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, auth, [&](const QUrl& url) { QDesktopServices::openUrl(url); });
    QObject::connect(auth, &QOAuth2AuthorizationCodeFlow::granted, auth, [&]() {

        timer->start(auth->expirationAt() - QDateTime::currentDateTime());

        if (!getReciever()) return;

        getReciever()->authorizationSuccessful(auth->refreshToken().toStdString());
    });

    QObject::connect(timer, &QTimer::timeout, [&] {

        auto r = getReciever();

        if (!r) return;

        r->grantAccessRequested();

    });

    return auth;
}

void Google::grantAccess(const std::string& refreshToken)
{
    qDebug() << "calling auth";

    auto auth = getAuth(true);

    if (refreshToken.empty()) {
        auth->grant();
        return;
    }
    
    auth->setRefreshToken(refreshToken.c_str());
    auth->refreshAccessToken();
}

void Google::query(const QString& urlStr, const QVariantMap& param, const QString& verb, const QString& requestBody, int callbackIdx)
{
    QUrl url = QUrl(urlStr);

    QUrlQuery query;

    for (auto it = param.begin(), end = param.end(); it != end; ++it) {
        query.addQueryItem(it.key(), it.value().toString());
    }

    url.setQuery(query);

    QNetworkRequest req(url);

    auto auth = getAuth();

    auth->prepareRequest(&req, verb.toUtf8(), verb.toUtf8());

    auto reply = auth->networkAccessManager()->sendCustomRequest(req, verb.toUtf8(), requestBody.toUtf8());

    QObject::connect(reply, &QNetworkReply::finished, reply, [=]() {

        reply->deleteLater();

        if (!getReciever()) return;

        if ((int)reply->error() == 99) //no internet connection
        {
            getReciever()->disconnected();

            return;
        }

        QString replyStr = reply->readAll();

        getReciever()->setReply(replyStr.toStdString(), callbackIdx);

    });
}

