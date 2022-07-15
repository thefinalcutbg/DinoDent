#include <QtWidgets/QApplication>
#include "View/Torque.h"
#include "View/ToothPaintDevices/SpriteSheets.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Tooth/DiagnosisContainer.h"
#include "Model/User/User.h"
#include "Database/Database.h"
#include "Model/KSMP.h"
#include "Model/CityCode.h"

int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setFont(QFont ("Segoe UI", 8));    

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icon_torque.png"));

    Db::setFilePath("TorqueDB.db");
    Db::createIfNotExist();

    //Intializing singletons
    CityCode::initialize();
    KSMP::initialize();
    User::initialize();
    SpriteSheets::container().initialize(); //loading textures, otherwise program will crash;
    DiagnosisContainer::initialize();

    MasterNZOK::instance().loadUpdates(); //parsing json of nzok data

    Torque w;

    if (!w.m_initialized)
        return 0;

    w.show();

    return a.exec();

}

