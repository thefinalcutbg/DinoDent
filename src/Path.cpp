#include "Path.h"
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>

namespace PrvPath
{
    QString configFilePath() {

        auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

        if (!dataFolder.exists()) dataFolder.mkpath(".");

        auto cfgPath = dataFolder.filePath("dinocfg.txt");

        QFile cfg(cfgPath);

        if (!cfg.exists()) {
            
            cfg.open(QIODevice::ReadWrite);
            cfg.write(dataFolder.filePath("database.db").toLocal8Bit());
            cfg.close();


        }

        return cfgPath;

    }

    QString dataFolderPath() {

        auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

        if (!dataFolder.exists()) dataFolder.mkpath(".");

        return dataFolder.path();

    }

}

std::string Path::getDbPath()
{

    QFile cfg(PrvPath::configFilePath());

    cfg.open(QIODevice::ReadOnly);

    std::string result;

    while (!cfg.atEnd())
    {
       result = cfg.readLine().constData();
    }

    return result;
}


std::string Path::setDbPath()
{
    auto str = QFileDialog::getOpenFileName(
        nullptr, 
        u8"Изберете местонахождение на бaзата данни", 
        getDbPath().c_str(), u8"Файл база данни (*.db)"
    );

    if (str.isEmpty()) return std::string();

    QFile cfg(PrvPath::configFilePath());

    cfg.remove();

    cfg.open(QIODevice::ReadWrite);
    
    cfg.write(str.toLocal8Bit());

    return str.toStdString();
}