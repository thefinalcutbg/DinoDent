#include <QtWidgets/QApplication>
#include "View/Torque.h"
#include "View/ListView/ToothPaintDevices/SpriteSheets.h"
#include <QDebug>


int main(int argc, char *argv[])
{


    QFont font("Segoe UI");
    font.setPointSize(8);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::highDpiScaleFactorRoundingPolicy;
    QApplication::setFont(font);

    QApplication a(argc, argv);

    SpriteSheets::container().LoadSpriteSheets(); //loading textures, otherwise program will crash;

    Torque w;
    w.show();
    return a.exec();
}

