#include "ContextMenu.h"
#include "Presenter/ListPresenter.h"
#include "DsnMenu.h"
#include "View/Theme.h"

using namespace Dental;

ContextMenu::ContextMenu()
{
    setStyleSheet(Theme::getPopupMenuStylesheet());
    addProcedure = new QAction("Добави процедура");
    addAction(addProcedure);
    connect(addProcedure, &QAction::triggered, [&] {if(presenter)presenter->addProcedure(); });

    details = new QAction("Бележки и извършени процедури");
    addAction(details);
    connect(details, &QAction::triggered, [&] {if (presenter) presenter->openDetails(); });
    
    QMenu* statusMenu = addMenu("Добави статус");

    QString otherActionNames[otherInputsCount]
    {
        "Възстановяване МО", "Възстановяване ДО", "Възстановяване МОД",
        "Премахни"/*Възстановяване*/, "Премахни"/*кариес*/, "Премахни статус", "Премахни конструкция/шина"
    };


    for (int i = 0; i < StatusCount; i++) //too lazy to initialize all the actions;
    {
        menuAction[i] = new QAction(statusNames[i].data());
        connect(menuAction[i], &QAction::triggered, [this, i]() { this->presenter->setToothStatus(StatusType::General, i); });
    }

    for (int i = 0; i < otherInputsCount; i++) //too lazy to initialize all the actions;
    {
        otherActions[i] = new QAction(otherActionNames[i]);
        connect(otherActions[i], &QAction::triggered, [this, i]() { this->presenter->setOther(i); });
    }

    statusMenu->addAction(menuAction[Dental::Healthy]);
    statusMenu->addAction(menuAction[Dental::Temporary]);

  //  addAction(menuAction[Dental::Dsn]);

    
    QMenu* obturMenu = statusMenu->addMenu("&Възстановяване");
#ifndef DISABLE_NEW_DENTAL_STATUSES
    QMenu* defObturMenu = statusMenu->addMenu("&Дефектно възстановяване");
#endif
    QMenu* cariesMenu = statusMenu->addMenu("&Кариес");
#ifndef DISABLE_NEW_DENTAL_STATUSES
    QMenu* nonCariesMenu = statusMenu->addMenu("&Некариозна лезия");
#endif
    QString surfName[SurfaceCount] = { "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };

    for (int i = 0; i < SurfaceCount; i++)
    {
        surfObt[i] = obturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::Restoration, i); });
#ifndef DISABLE_NEW_DENTAL_STATUSES
        surfDefObt[i] = defObturMenu->addAction(surfName[i]);
        connect(surfDefObt[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::DefectiveRestoration, i); });
#endif
        surfCar[i] = cariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::Caries, i); });
#ifndef DISABLE_NEW_DENTAL_STATUSES
        surfNonCar[i] = nonCariesMenu->addAction(surfName[i]);
        connect(surfNonCar[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::NonCariesLesion, i); });
#endif
    }


    obturMenu->addSeparator();
    obturMenu->addAction(otherActions[OtherInputs::MO]);
    obturMenu->addAction(otherActions[OtherInputs::DO]);
    obturMenu->addAction(otherActions[OtherInputs::MOD]);
    obturMenu->addSeparator();
    obturMenu->addAction(otherActions[OtherInputs::removeO]);

    cariesMenu->addSeparator();
    cariesMenu->addAction(otherActions[OtherInputs::removeC]);

    auto endoMenu = statusMenu->addMenu("Ендодонтия");

    endoMenu->addAction(menuAction[Dental::Pulpitis]);
    endoMenu->addAction(menuAction[Dental::Necrosis]);
    endoMenu->addAction(menuAction[Dental::ApicalLesion]);
    endoMenu->addAction(menuAction[Dental::Resorption]);
    endoMenu->addAction(menuAction[Dental::RootCanal]);
    endoMenu->addAction(menuAction[Dental::Post]);

    auto surgeryMenu = statusMenu->addMenu("Хирургия");

    surgeryMenu->addAction(menuAction[Dental::Missing]);
    surgeryMenu->addAction(menuAction[Dental::Root]);
    surgeryMenu->addAction(menuAction[Dental::Implant]);
    surgeryMenu->addAction(menuAction[Dental::Fracture]);
    surgeryMenu->addAction(menuAction[Dental::Impacted]);

    dsn_menu = new DsnMenu();
    auto dsnMenuAction = statusMenu->addMenu(dsn_menu);
    dsnMenuAction->setText("Свръхброен зъб");

    auto perioMenu = statusMenu->addMenu("Пародонтология");
    perioMenu->addAction(menuAction[Dental::Periodontitis]);
    perioMenu->addAction(menuAction[Dental::Calculus]);
    
    for (int i = 0; i < MobilityCount; i++)
    {
        mobilityDegree[i] = perioMenu->addAction(mobilityNames[i].data());
        connect(mobilityDegree[i], &QAction::triggered, [this, i]() {
            this->presenter->setToothStatus(StatusType::Mobility, i);; });
    }

    auto prosthoMenu = statusMenu->addMenu("Протетика");

    prosthoMenu->addAction(menuAction[Dental::Crown]);
    prosthoMenu->addAction(menuAction[Dental::Bridge]);
    prosthoMenu->addAction(menuAction[Dental::Denture]);
    prosthoMenu->addAction(menuAction[Dental::Splint]);
    prosthoMenu->addAction(otherActions[OtherInputs::removeBridge]);

    addAction(otherActions[OtherInputs::removeAll]);
}

void ContextMenu::setSelection(bool single) { details->setEnabled(single); }

void ContextMenu::setModel(const CheckModel& checkModel, const CheckModel& dsnModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.restorationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
    this->setModel(checkModel.mobilityStatus, mobilityDegree);
#ifndef DISABLE_NEW_DENTAL_STATUSES
    this->setModel(checkModel.nonCariesStatus, surfNonCar);
    this->setModel(checkModel.defRestoStatus, surfDefObt);
#endif

    dsn_menu->setModel(dsnModel);
}

void ContextMenu::setPresenter(ListPresenter* presenter) { 
    this->presenter = presenter; 
    dsn_menu->setPresenter(presenter);
}

ContextMenu::~ContextMenu() {}
