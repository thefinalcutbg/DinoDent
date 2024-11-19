#include "GlobalSettings.h"
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <json/json.h>
#include <QtGlobal>
#include <QTextStream>

#include "Model/Date.h"
#include "Model/FreeFunctions.h"
#include "Model/Time.h"

void rewriteCfg(const Json::Value& settings)
{
    QFile cfg(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.json"));
    cfg.open(QIODevice::ReadWrite);
    cfg.resize(0);
    cfg.write(Json::StyledWriter().write(settings).c_str());
}


Json::Value getSettingsAsJson()
{
    Json::Value settings;

    QFile file(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.json"));

    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    };

    QString text;

    while (!file.atEnd()) {
        text += file.readLine();
    }

    if (!Json::Reader().parse(text.toStdString(), settings)) return {};

    return settings;
}


std::string GlobalSettings::getDbBackupFilepath()
{
    auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
 
    if (!dataFolder.cd("backup")) dataFolder.mkpath("backup");

    dataFolder.cd("backup");

    auto time = Time::currentTime();

    return dataFolder.path().toStdString() + "/" +
        "backup" +
        Date::currentDate().to8601() + "T" +
        FreeFn::leadZeroes(time.hour, 2) + "-" +
        FreeFn::leadZeroes(time.minutes, 2) + "-" +
        FreeFn::leadZeroes(time.sec, 2) +
        ".db"
        ;
}

bool GlobalSettings::getDevBranch()
{
    return getSettingsAsJson()["dev_branch"].asBool();
}

void GlobalSettings::setDevBranch(bool dev)
{
    Json::Value settings = getSettingsAsJson();

    settings["dev_branch"] = dev;

    rewriteCfg(settings);
}

bool GlobalSettings::getMultiPkcs11()
{
    return getSettingsAsJson()["multi_pkcs11"].asBool();
}

void GlobalSettings::setMultiPkcs11(bool multi)
{
    Json::Value settings = getSettingsAsJson();

    settings["multi_pkcs11"] = multi;

    rewriteCfg(settings);
}

void GlobalSettings::createCfgIfNotExists()
{
    auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    //creating the user data folder
    if (!dataFolder.exists()) dataFolder.mkpath(".");

    auto settings = getSettingsAsJson();

    if (!settings.isMember("telemetry_id")) {
        settings["telemetry_id"] = FreeFn::getUuid();
    }

    if (!settings.isMember("db_path"))
    {
        settings["db_path"] = dataFolder.filePath("database.db").toUtf8().toStdString();
    }

    if (!settings.isMember("pkcs11_path"))
    {

        auto modules = getDefaultPkcs11Paths();

        for (auto& m : modules) {
            settings["pkcs11_path"].append(Json::Value(m));
        }
    }

    if (!settings.isMember("dev_branch")) {
        settings["dev_branch"] = false;
    }

    if (!settings.isMember("multi_pkcs11")) {
        settings["multi_pkcs11"] = false;
    }

    rewriteCfg(settings);
}

std::string GlobalSettings::getDbPath()
{
    QDir dir(QString::fromUtf8(getSettingsAsJson()["db_path"].asString().c_str()));

    return dir.path().toStdString();
}

std::string GlobalSettings::setDbPath()
{
    auto str = QFileDialog::getOpenFileName(
        nullptr, 
        "Изберете местонахождение на бaзата данни", 
        getDbPath().c_str(), "Файл база данни (*.db)"
    );

    if (str.isEmpty()) return std::string();

    Json::Value settings = getSettingsAsJson();

    settings["db_path"] = str.toUtf8().toStdString();

    rewriteCfg(settings);

    return getDbPath();
}

std::vector<std::string> GlobalSettings::getDefaultPkcs11Paths()
{

#ifdef Q_OS_WIN
    return std::vector<std::string>        
    {
        "C:/Windows/System32/eTPKCS11.dll",
        "C:/Program Files/OpenSC Project/OpenSC/pkcs11/opensc-pkcs11.dll",
        "C:/Program Files/SafeNet/Authentication/SAC/x64/IDPrimePKCS1164.dll",
        "C:/Windows/System32/OcsPKCS11Wrapper.dll",
        "C:/Windows/System32/bit4ipki.dll",
        "C:/Windows/System32/idprimepkcs11.dll",
        "C:/Windows/System32/cmP11.dll",
        "C:/Windows/System32/cvP11.dll",
        "C:/Windows/System32/siecap11.dll",
        "C:/Windows/System32/cmP1164.dll"
    };
#endif

#ifdef Q_OS_LINUX
    return std::vector<std::string>
        {
            "/usr/lib/x86_64-linux-gnu/opensc-pkcs11.so",
            "/usr/lib64/opensc-pkcs11.so",
            "/usr/lib/pkcs11/libeTPkcs11.so",
            "/usr/lib64/pkcs11/libeTPkcs11.so",
            "/usr/lib/libIDPrimePKCS11.so",
            "/usr/lib64/libIDPrimePKCS11.so ",
            "/usr/local/lib64/libcvP11.so",
            "/usr/lib/libbit4ipki.so",
            "/usr/lib64/libbit4ipki.so",
            "/usr/local/lib/libsiecap11.so"
        };
#endif
#ifdef Q_OS_MAC
    return std::vector<std::string>
        {
            "/Library/OpenSC/lib/opensc-pkcs11.so",
            "/Library/AWP/lib/libOcsCryptoki.dylib",
            "/Library/Frameworks/eToken.framework/Versions/A/libIDPrimePKCS11.dylib",
            "/Library/Gemalto/libidprimepkcs11.dylib",
            "/Library/CV Cryptovision/libcvp11.dylib",
            "/Library/bit4id/pkcs11/libbit4ipki.dylib",
            "/Applications/Charismathics/libcmP11.dylib"

        };
#endif
}

std::string GlobalSettings::telemetryId()
{
    return getSettingsAsJson()["telemetry_id"].asString();
}

static std::vector<std::string> s_pkcs11Paths;

const std::vector<std::string>& GlobalSettings::pkcs11PathList()
{
    if (s_pkcs11Paths.size()) return s_pkcs11Paths;

    Json::Value settings = getSettingsAsJson();

    for (auto& path : settings["pkcs11_path"]) {
        s_pkcs11Paths.push_back(path.asString());
    }
    
    return s_pkcs11Paths;

}

void GlobalSettings::setPkcs11PathList(const std::vector<std::string> list)
{

    Json::Value settings = getSettingsAsJson();

    settings["pkcs11_path"].clear();

    for (auto& m : list) { settings["pkcs11_path"].append(m); }

    rewriteCfg(settings);

    s_pkcs11Paths = list;
}


bool s_showRequests{ false };
bool s_showReplies{ false };

void GlobalSettings::setDebug(bool showRequests, bool showReplies)
{
    s_showRequests = showRequests;
    s_showReplies = showReplies;
}

bool GlobalSettings::showRequestsEnabled()
{
    return s_showRequests;
}

bool GlobalSettings::showRepliesEnabled()
{
    return s_showReplies;
}
