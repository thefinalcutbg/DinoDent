#include <QtWidgets/QApplication>
#include "View/Torque.h"
#include "View/ListView/ToothPaintDevices/SpriteSheets.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Procedure/CustomProcedures.h"
#include "Model/Tooth/DiagnosisContainer.h"
#include "Database/Database.h"
#include <QTextCodec>
#include "TestToothDialog.h"

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
    SpriteSheets::container().loadTextures(); //loading textures, otherwise program will crash;
    DiagnosisContainer::initialize();

   //TestToothDialog(); /*
    
    MasterNZOK::instance().loadUpdates(); //parsing json of nzok data
    CustomProcedures::instance().loadCustomProcedures(); //parsing json of custom procedures

    Torque w;
    w.show();
    
    return a.exec();
  // */
}

