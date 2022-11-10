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
            cfg.write(dataFolder.filePath("database.db").toUtf8());
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

std::string Path::getDataPath()
{
    auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!dataFolder.exists()) dataFolder.mkpath(".");

    return dataFolder.path().toLocal8Bit().toStdString();
}

std::string Path::getDbPath()
{
    QFile cfg(PrvPath::configFilePath());

    cfg.open(QIODevice::ReadOnly);

    QByteArray path;

    while (!cfg.atEnd())
    {
       path = cfg.readLine();
    }

    cfg.close();

    QDir dir;
    
    return dir.absoluteFilePath(path).toStdString();
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
    
    cfg.write(str.toUtf8());

    return str.toStdString();
}