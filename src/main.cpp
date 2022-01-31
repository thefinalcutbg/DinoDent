#include <QtWidgets/QApplication>
#include "View/Torque.h"
#include "View/ToothPaintDevices/SpriteSheets.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Tooth/DiagnosisContainer.h"
#include "Model/User/UserManager.h"
#include "Database/Database.h"
#include <QTextCodec>


int main(int argc, char *argv[])
{
    {Database d;} //checks the database

    QFont font("Segoe UI");
    font.setPointSize(8);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::highDpiScaleFactorRoundingPolicy();
    QApplication::setFont(font);

    QApplication a(argc, argv);
     

    //Intializing singletons
    SpriteSheets::container().initialize(); //loading textures, otherwise program will crash;
    DiagnosisContainer::initialize();
    UserManager::initialize();
  // TestToothDialog(); /*
    
    MasterNZOK::instance().loadUpdates(); //parsing json of nzok data

    Torque w;

    if (w.m_initialized == false)
        return 0;

    w.show();
    
    return a.exec();
 //  */
}

