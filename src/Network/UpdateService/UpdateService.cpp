#include "UpdateService.h"
#include "Network/NetworkManager.h"
#include <QNetworkReply>
#include "View/ModalDialogBuilder.h"
#include <QEventLoop>
#include <QDebug>
#include <json/json.h>
#include "View/Widgets/UpdatePromptDialog.h"
#include "View/Widgets/UpdateDownloader.h"
#include "Version.h"
#include <QDesktopServices>
bool UpdateService::restartForUpdate()
{
	auto m_reply = NetworkManager::simpleRequest(
        "https://raw.githubusercontent.com/thefinalcutbg/DinoDent/main/ver"
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

    switch (Version::branch())
    {
        case Version::Win64:
            updateInfo = updateInfo["win64"];
            break;
        case Version::Win64Dev:
            updateInfo = updateInfo["win64dev"];
            break;
        default: return false;
    }
    
    auto latestVersion = Version::fromStr(updateInfo["ver"].asString());

    if (!Version::current().isLessThan(latestVersion)) {
        return false;
    }

    auto linkAddress = updateInfo["dl"].asString();

    std::string changeLog;

    changeLog.append("Версия ");
    changeLog.append(updateInfo["ver"].asString());

    for (auto& change : updateInfo["changeLog"])
    {
        changeLog.append("<br> - ");
        changeLog.append(change.asString());
    }

    switch (UpdatePromptDialog(changeLog).exec())
    {
        case QDialog::Rejected: return false;

        case QDialog::Accepted:
        {
            UpdateDownloader d(linkAddress.c_str());
            d.exec();

            return d.installerDownloaded();
        }

        case 2: //link clicked
            return QDesktopServices::openUrl(QUrl(linkAddress.c_str()));

        default: 
            return false;
    }

    
}
