#include "GlobalSettings.h"
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <JsonCpp/json.h>
#include <array>
#include <fstream>
#include "Model/Date.h"
#include "Model/FreeFunctions.h"
#include "Model/Time.h"

namespace PrvPath
{
    std::string configFilePath() {
        return QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.json").toStdString();
    }
}

void rewriteCfg(const Json::Value& settings)
{
    QFile cfg(PrvPath::configFilePath().c_str());
    cfg.open(QIODevice::ReadWrite);
    cfg.resize(0);
    cfg.write(Json::StyledWriter().write(settings).c_str());
    cfg.close();
}


Json::Value getSettingsAsJson()
{
    Json::Value settings;

    std::ifstream f(PrvPath::configFilePath());

    QDir dir;

    Json::Reader().parse(f, settings);

    return settings;
}


std::string GlobalSettings::getDbBackupFilepath()
{
    auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!dataFolder.exists()) dataFolder.mkpath(".");

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


void GlobalSettings::createCfgIfNotExists()
{
    auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    //creating the user data folder
    if (!dataFolder.exists()) dataFolder.mkpath(".");


    QFile cfg(dataFolder.filePath("config.json"));

    //creating the config file
    if (!cfg.exists()) {

        std::string dbPath = dataFolder.filePath("database.db").toUtf8().toStdString();

                //getting db from the old cfg
                QFile old_cfg(dataFolder.filePath("dinocfg.txt"));

                if (old_cfg.exists()) {

                    old_cfg.open(QIODevice::ReadOnly);

                    while (!old_cfg.atEnd())
                    {
                        dbPath = old_cfg.readLine().toStdString();
                    }

                    old_cfg.close();
                }

        Json::Value settings;
        
        settings["db_path"] = dbPath;

        settings["pkcs11_path"] = Json::arrayValue;

        auto modules = getDefaultPkcs11Paths();

        for (auto& m : modules)  settings["pkcs11_path"].append(m.data());

        cfg.open(QIODevice::ReadWrite);
        cfg.write(Json::StyledWriter().write(settings).c_str());
        cfg.close();
    }

    //check integrity
    
    auto settings = getSettingsAsJson();

    if (!settings.isMember("telemetry_id")) {
        settings["telemetry_id"] = FreeFn::getUuid();
    }
    
    rewriteCfg(settings);
}



std::string GlobalSettings::getDbPath()
{
    return getSettingsAsJson()["db_path"].asString();
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

    return str.toUtf8().toStdString();
}

std::vector<std::string> GlobalSettings::getDefaultPkcs11Paths()
{
    return std::vector<std::string>        
    {
            "C:/Program Files/SafeNet/Authentication/SAC/x64/IDPrimePKCS1164.dll",
            "C:/Windows/System32/OcsPKCS11Wrapper.dll",
            "C:/Windows/System32/idprimepkcs11.dll",
            "C:/Windows/System32/bit4ipki.dll",
            "C:/Windows/System32/cmP11.dll",
            "C:/Windows/System32/cvP11.dll",
            "C:/Windows/System32/siecap11.dll",
            "C:/Windows/System32/cmP1164.dll"
    };
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
