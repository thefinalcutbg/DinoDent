#include <QtWidgets/QApplication>
#include "View/Widgets/DinoDent.h"
#include "Network/UpdateService/UpdateService.h"
#include <QtGlobal>
#include "GlobalSettings.h"

bool initFunction();

void testFn();

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setApplicationName("DinoDent");
    a.setWindowIcon(QIcon(":/icons/icon_app.png"));

    GlobalSettings::createCfgIfNotExists();

    if (UpdateService::restartForUpdate()) { return 0; };

    if (!initFunction()) {  return 0;  }

//    testFn(); return 0;

    //splash screen is destroyed at the end of DinoDent constructor
    DinoDent w;

    if (!w.m_loggedIn) return 0;

    w.show();

    return a.exec();
}

#include <QFontDatabase>

#include "Database/Database.h"
#include "DbUpdates/Updater.h"

#include "Model/User.h"
#include "Model/Specialty.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/ProcedureCode.h"
#include "Model/Dental/Diagnosis.h"
#include "Model/Prescription/Medication.h"
#include "Model/Prescription/DoseQuantity.h"
#include "Model/Prescription/WhenToTake.h"
#include "Model/ICD10.h"
#include "Model/Ekatte.h"
#include "Model/Country.h"

#include "View/Graphics/SpriteSheets.h"
#include "View/Graphics/Zodiac.h"
#include "View/Widgets/SplashScreen.h"
#include "Network/HIS/HisService.h"

bool initFunction() {

    auto args = QApplication::arguments();

    if (args.count() > 1 && args.at(1) == "-histest") {

        HisService::hisUrl = "https://ptest-api.his.bg";
		ModalDialogBuilder::showMessage("Програмата ще работи с тестовата версия на НЗИС API.\n"
			"Не използвайте реални данни!");
    }

#ifdef Q_OS_WIN
    QApplication::setStyle("windowsvista");     //"windows11", "windowsvista", "Windows", "Fusion"
#endif

#ifdef Q_OS_OSX
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
#endif

    SplashScreen::createAndShow();

    Db::setFilePath(GlobalSettings::getDbPath());

    if (!Db::createIfNotExist()) {

        SplashScreen::hideAndDestroy();

        ModalDialogBuilder::showError(
            "Неуспешно създаване на базата данни."
            "\nУверете се, че пътят към файлът е правилен и че"
            "\nимате правомощия за модифицирането му."
        );

        return false;
    };

    //Intializing static data
    SplashScreen::showMessage("Зареждане на текстурите");
    SpriteSheets::container().initialize(); //loading textures, otherwise program will crash;
    Zodiac::initialize();
    SplashScreen::showMessage("Зареждане на МКБ номенклатурите");
    ICD10::initialize();
    SplashScreen::showMessage("Зареждане на КСМП номенклатурите");
    ProcedureCode::initialize();
    NhifProcedures::initialize(); //parsing json of nhif data
    SplashScreen::showMessage("Зареждане на медикаментите");
    Medication::initialize();
    SplashScreen::showMessage("Зареждане на населените места");
    Ekatte::initialize();
    Country::initialize();
    DoseQuantity::initialize();
    Route::initialize();
    WhenToTake::initialize();
    Date::initializeHolidays();
    Specialty::initialize();
    User::initialize();

    Db::showErrorDialog(true);
    SplashScreen::showMessage("Обновяване на база данни");
    DbUpdater::updateDb();

    SplashScreen::showMessage("Стартиране на DinoDent...");

    return true;

}

#include "Tests/CrudTest.h"
void testFn()
{
    ModalDialogBuilder::showMessage(Test::crudTest());
}
