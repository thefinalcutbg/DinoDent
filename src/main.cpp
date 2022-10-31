#include <QtWidgets/QApplication>
#include "View/Widgets/Torque.h"
#include "Network/UpdateService/UpdateService.h"

bool initFunction();

void testFn();


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setFont(QFont ("Segoe UI", 8));    
    QCoreApplication::setApplicationName("DinoDent");

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/icons/icon_torque.png"));
    
    if (UpdateService::restartForUpdate()) { return 0; };

    if (!initFunction()) { return 0; }

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
#include "Path.h"

bool initFunction() {

    Db::setFilePath(Path::getDbPath());

    if (!Db::createIfNotExist()) {

        ModalDialogBuilder::showError(
            u8"Неуспешно създаване на базата данни."
            u8"\nУверете се, че пътят към файлът е правилен и че"
            u8"\nимате правомощия за модифицирането му."
        );
    };

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

    Db::showErrorDialog(true);
    DbUpdater::updateDb();

    return true;

}

void testFn()
{
    Path::getDbPath();

}