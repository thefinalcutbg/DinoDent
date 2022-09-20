#include <QtWidgets/QApplication>
#include "View/Widgets/Torque.h"

void initFunction();

void testFn();


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setFont(QFont ("Segoe UI", 8));    
    QCoreApplication::setApplicationName("DinoDent");

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/icons/icon_torque.png"));

    initFunction();

   // testFn(); return 0;

    Torque w;

    if (!w.m_loggedIn)
        return 0;

    w.show();

    return a.exec();

}

#include "Database/Database.h"
#include "Model/User.h"
#include "View/Graphics/SpriteSheets.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/DiagnosisContainer.h"
#include "Model/Dental/KSMP.h"
#include "Model/Prescription/Medication.h"
#include "Model/Prescription/DoseQuantity.h"
#include "Model/Prescription/WhenToTake.h"
#include "Model/Dental/MKB.h"
#include "Model/Ekatte.h"
#include "Updates/Updater.h"

void initFunction() {

    Db::setFilePath("TorqueDB.db");
    Db::createIfNotExist();



    //Intializing static data
    SpriteSheets::container().initialize(); //loading textures, otherwise program will crash;
    DiagnosisContainer::initialize();
    NhifProcedures::initialize(); //parsing json of nhif data
    Medication::initialize();
    KSMP::initialize();
    Ekatte::initialize();
    MKB::initialize();
    User::initialize();
    DoseQuantity::initialize();
    Route::initialize();
    WhenToTake::initialize();

    DbUpdater::updateDb();

}

void testFn()
{
    /*
    auto& forms = Medication::forms();

    for (int i = 0; i < forms.size(); i++)
    {
        int mapping = DoseQuantity::CL010_to_CL035Converter(i);
        
        if (mapping == -1)
            qDebug() << forms[i].c_str() << " no mapping";
    }
    */

}