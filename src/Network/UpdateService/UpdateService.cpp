#include "UpdateService.h"
#include "Network/NetworkManager.h"
#include <QNetworkReply>
#include "View/ModalDialogBuilder.h"
#include <QEventLoop>
#include <QDebug>
#include <JsonCpp/json.h>
#include "View/Widgets/UpdateDownloader.h"
#include "Version.h"

bool UpdateService::restartForUpdate()
{
	auto m_reply = NetworkManager::simpleRequest(
        "https://raw.githubusercontent.com/thefinalcutbg/DinoDent/main/verrr"
    );

    QEventLoop loop;
    QObject::connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    auto result = m_reply->readAll().toStdString();

    m_reply->deleteLater();

    if (result.empty()) return  false;

    Json::Value updateInfo;

    bool parsingSuccessful = Json::Reader().parse(result, updateInfo);

    if (!parsingSuccessful || !updateInfo.isObject()) {
        return  false;
    }

    updateInfo = updateInfo["win64"];

    auto latestVersion = Version::fromStr(updateInfo["ver"].asString());

    if (Version::current().isLessThan(latestVersion))
    {
        std::string changeLog("Версия ");
        changeLog.append(updateInfo["ver"].asString());

        for (auto& change : updateInfo["changeLog"])
        {
            changeLog.append("\n - ");
            changeLog.append(change.asString());
        }

        if (!ModalDialogBuilder::updatePrompt(changeLog)) return false;

        UpdateDownloader d(updateInfo["dl"].asString().c_str());
        d.exec();

        return d.installerDownloaded();
    }

    return false;
    
}
