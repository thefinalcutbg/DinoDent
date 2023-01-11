#include "UpdateService.h"
#include "Network/NetworkManager.h"
#include <QNetworkReply>
#include "View/ModalDialogBuilder.h"
#include <QEventLoop>
#include <QDebug>
#include <JsonCpp/json.h>
#include "View/Widgets/UpdateDownloader.h";

constexpr int programVersion = 3;

bool UpdateService::restartForUpdate()
{
	auto m_reply = NetworkManager::simpleRequest(
        "https://raw.githubusercontent.com/thefinalcutbg/DinoDent/main/version"
    );

    QEventLoop loop;
    QObject::connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    auto result = m_reply->readAll().toStdString();

    m_reply->deleteLater();

    if (result.empty()) return  false;

    Json::Value updateInfo;

    bool parsingSuccessful = Json::Reader().parse(result, updateInfo);

    if (!parsingSuccessful) {
        return  false;
    }

    updateInfo = updateInfo["win64"];

    int version = updateInfo["ver"].asInt();

    if (programVersion < version)
    {

        if (ModalDialogBuilder::askDialog(
            "Открита е нова версия на програмата.\n"
            "Желаете ли да я изтеглите?") == false
            ) {

            return false;;
        }

        UpdateDownloader d(updateInfo["dl"].asString().c_str());
        d.exec();

        return d.installerDownloaded();
    }

    return false;
    
}
