#include <QtWidgets/QApplication>
#include "View/Widgets/DinoDent.h"
#include "Network/UpdateService/UpdateService.h"
#include <QTextCodec>
#include "Model/FreeFunctions.h"
#include <QSplashScreen>

bool initFunction();

void testFn();

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setFont(QFont ("Segoe UI", 8));    
    QCoreApplication::setApplicationName("DinoDent");
    
    QApplication a(argc, argv);

   // testFn(); return 0;

    a.setWindowIcon(QIcon(":/icons/icon_torque.png"));

    if (UpdateService::restartForUpdate()) { return 0; };

    if (!initFunction()) { return 0; }

    DinoDent w;

    if (!w.m_loggedIn) return 0;

    w.show();

    return a.exec();

}

#include "Database/Database.h"
#include "Model/User.h"
#include "View/Graphics/SpriteSheets.h"
#include "View/Graphics/Zodiac.h"
#include "Model/Specialty.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/ProcedureCode.h"
#include "Model/Dental/Diagnosis.h"
#include "Model/Dental/KSMP.h"
#include "Model/Prescription/Medication.h"
#include "Model/Prescription/DoseQuantity.h"
#include "Model/Prescription/WhenToTake.h"
#include "Model/Dental/MKB.h"
#include "Model/Ekatte.h"
#include "Model/Country.h"
#include "DbUpdates/Updater.h"
#include "GlobalSettings.h"

bool initFunction() {

    GlobalSettings::createCfgIfNotExists();

    Db::setFilePath(GlobalSettings::getDbPath());

    if (!Db::createIfNotExist()) {

        ModalDialogBuilder::showError(
            "Неуспешно създаване на базата данни."
            "\nУверете се, че пътят към файлът е правилен и че"
            "\nимате правомощия за модифицирането му."
        );

        return false;
    };

    //showing splash screen
    QSplashScreen splash(QPixmap(":/other/splash.png"));
    splash.show();
    splash.showMessage("DinoDent зарежда, моля изчакайте...", Qt::AlignLeft | Qt::AlignBottom, Qt::darkGray);
    QApplication::instance()->processEvents();

    //Intializing static data
    SpriteSheets::container().initialize(); //loading textures, otherwise program will crash;
    Zodiac::initialize();
    ProcedureCode::initialize();
    Diagnosis::initialize();
    NhifProcedures::initialize(); //parsing json of nhif data
    Medication::initialize();
    //KSMP::initialize();
    Ekatte::initialize();
    Country::initialize();
    MKB::initialize();
    DoseQuantity::initialize();
    Route::initialize();
    WhenToTake::initialize();
    Date::initializeHolidays();
    Specialty::initialize();
    User::initialize();

    splash.hide();

    Db::showErrorDialog(true);
    DbUpdater::updateDb();

    return true;

}

void testFn()
{
   
    
}