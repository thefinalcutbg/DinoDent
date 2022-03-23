#include <QtWidgets/QApplication>
#include "View/Torque.h"
#include "View/ToothPaintDevices/SpriteSheets.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Tooth/DiagnosisContainer.h"
#include "Model/User/UserManager.h"
#include "Database/Database.h"
#include <QTextCodec>
#include "Model/CityCode.h"
/*
void replaceCityWithInt()
{
    Db db;

    auto& cityVec = CityCode::getCitieStrings();

    for (int i = 0; i < cityVec.size(); i++)
    {
        db.execute("UPDATE patient SET city = " + std::to_string(i) + " "
            "WHERE city = '" + *cityVec[i] + "'");
    }
}
*/
int main(int argc, char *argv[])
{
    Db::setFilePath("DATATEST.db");
    Db::createIfNotExist();

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::highDpiScaleFactorRoundingPolicy();
    //QApplication::setFont(QFont ("Segoe UI", 8));    

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icon_torque.png"));

    //Intializing singletons
    CityCode::initialize();
    SpriteSheets::container().initialize(); //loading textures, otherwise program will crash;
    DiagnosisContainer::initialize();
    UserManager::initialize();
    MasterNZOK::instance().loadUpdates(); //parsing json of nzok data

    Torque w;

    if (!w.m_initialized)
        return 0;

    w.show();
    
    return a.exec();

}

