#include "Google.h"

#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include "Presenter/CalendarPresenter.h"
#include "Credentials.h"

QString timeZoneOffset;
CalendarPresenter* s_reciever;


void Google::setReciever(CalendarPresenter* p)
{
    s_reciever = p;
}

CalendarPresenter* getReciever() {
    return s_reciever;
}


QOAuth2AuthorizationCodeFlow* getAuth() {
	
    static QOAuth2AuthorizationCodeFlow* auth =  nullptr;

    if (auth) return auth;

    auth = new QOAuth2AuthorizationCodeFlow();

    auth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    auth->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
    auth->setClientIdentifier(Credentials::ClientIdentifier);
    auth->setClientIdentifierSharedKey(Credentials::ClientIdentifierSharedKey);
    const QUrl redirectUri("http://localhost");
    const auto port = static_cast<quint16>(redirectUri.port());

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, auth);
    auth->setReplyHandler(replyHandler);

    auth->setScope("https://www.googleapis.com/auth/calendar");

    QObject::connect(auth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, auth, [&](const QUrl& url) { QDesktopServices::openUrl(url); });
    QObject::connect(auth, &QOAuth2AuthorizationCodeFlow::granted, auth, [&]() {
        if (!getReciever()) return;
        getReciever()->authorizationSuccessful(auth->refreshToken().toStdString());
    });

    return auth;

}


void Google::grantAccess(const std::string& refreshToken)
{
    if (refreshToken.empty()) {
        getAuth()->grant();
        return;
    }

    getAuth()->setRefreshToken(refreshToken.c_str());
    getAuth()->refreshAccessToken();
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

         if (!getReciever()) return;
         
         getReciever()->setReply(reply->readAll().toStdString(), callbackIdx);

    });
}

