﻿#include "DsnMenu.h"
#include "Presenter/ListPresenter.h"
#include "View/Theme.h"

using namespace Dental;

DsnMenu::DsnMenu()
{

    setStyleSheet(Theme::getPopupMenuStylesheet());

    for (int i = 0; i < Dental::StatusCount; i++)
    {
        if (
            i == static_cast<int>(Dental::Bridge) ||
            i == static_cast<int>(Dental::Splint) ||
            i == static_cast<int>(Dental::Implant) ||
            i == static_cast<int>(Dental::Denture) ||
            i == static_cast<int>(Dental::HasSupernumeral)
        )
        {
            continue;
        }

        menuAction[i] = new QAction(Dental::statusNames[i].data());

        connect(menuAction[i], &QAction::triggered, [this, i]() { this->presenter->setToothStatus(StatusType::General, i, true); });
    }

    addAction(menuAction[Dental::Healthy]);
    addAction(menuAction[Dental::Temporary]);

    QMenu* ObturMenu = addMenu("&Възстановяване");
    QMenu* CariesMenu = addMenu("&Патология на ТЗТ");
    QMenu* MobilityMenu = addMenu("&Подвижност");

    QString surfName[SurfaceCount] = { "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };

    for (int i = 0; i < SurfaceCount; i++)
    {
        surfObt[i] = ObturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::Restoration, i, true); });

        surfCar[i] = CariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::Caries, i, true); });
    }

    for (int i = 0; i < MobilityCount; i++)
    {
        mobilityDegree[i] = MobilityMenu->addAction(mobilityNames[i].data());
        connect(mobilityDegree[i], &QAction::triggered, [this, i]() {
            this->presenter->setToothStatus(StatusType::Mobility, i, true); });

    }

    addSeparator();
   
    addAction(menuAction[Dental::Pulpitis]);
    addAction(menuAction[Dental::ApicalLesion]);
    addAction(menuAction[Dental::RootCanal]);
    addAction(menuAction[Dental::Post]);
    addAction(menuAction[Dental::Missing]);
    addAction(menuAction[Dental::Root]);
    addAction(menuAction[Dental::Fracture]);
    addAction(menuAction[Dental::Impacted]);
    addAction(menuAction[Dental::Periodontitis]);
    addAction(menuAction[Dental::Calculus]);
    addAction(menuAction[Dental::Crown]);
    
    auto removeDsn = addAction("Премахни");
    connect(removeDsn, &QAction::triggered, [this]() {this->presenter->setOther(OtherInputs::removeDsn); });

}

void DsnMenu::setModel(const CheckModel& checkModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.obturationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
    this->setModel(checkModel.mobilityStatus, mobilityDegree);
}