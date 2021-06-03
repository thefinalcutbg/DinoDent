#include <QtWidgets/QApplication>
#include "View/Torque.h"
#include "View/ListView/ToothPaintDevices/SpriteSheets.h"
#include "Model/Manipulation/MasterNZOK.h"
#include "Model/Manipulation/CustomProcedures.h"

int main(int argc, char *argv[])
{

    QFont font("Segoe UI");
    font.setPointSize(8);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::highDpiScaleFactorRoundingPolicy;
    QApplication::setFont(font);

    QApplication a(argc, argv);

    SpriteSheets::container().loadSpriteSheets(); //loading textures, otherwise program will crash;
    MasterNZOK::instance().loadUpdates();
    CustomProcedures::instance().loadCustomProcedures();

    Torque w;
    w.show();
    return a.exec();
}

