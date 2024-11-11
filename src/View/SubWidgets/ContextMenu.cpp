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

    details = new QAction("Детайли");
    addAction(details);
    connect(details, &QAction::triggered, [&] {if (presenter) presenter->openDetails(); });
    
    addSeparator();

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

    addAction(menuAction[Dental::Healthy]);
    addAction(menuAction[Dental::Temporary]);

  //  addAction(menuAction[Dental::Dsn]);

    
    QMenu* obturMenu = addMenu("&Възстановяване");
    QMenu* cariesMenu = addMenu("&Патология на ТЗТ");
    QMenu* endoMenu = addMenu("&Ендодонтия");
    QMenu* surgeryMenu = addMenu("&Хирургия");
    QMenu* mobilityMenu = addMenu("&Подвижност");

    dsn_menu = new DsnMenu();
    auto dsnMenuAction = addMenu(dsn_menu);
    dsnMenuAction->setText("Свръхброен зъб");

    QString surfName[SurfaceCount] = { "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };

    for (int i = 0; i < SurfaceCount; i++)
    {
        surfObt[i] = obturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::Restoration, i); });

        surfCar[i] = cariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::Caries, i); });
    }

    for (int i = 0; i < MobilityCount; i++)
    {
        mobilityDegree[i] = mobilityMenu->addAction(mobilityNames[i].data());
        connect(mobilityDegree[i], &QAction::triggered, [this, i]() {
            this->presenter->setToothStatus(StatusType::Mobility, i);; });

    }


    obturMenu->addSeparator();
    obturMenu->addAction(otherActions[OtherInputs::MO]);
    obturMenu->addAction(otherActions[OtherInputs::DO]);
    obturMenu->addAction(otherActions[OtherInputs::MOD]);
    obturMenu->addSeparator();
    obturMenu->addAction(otherActions[OtherInputs::removeO]);

    cariesMenu->addSeparator();
    cariesMenu->addAction(otherActions[OtherInputs::removeC]);

    endoMenu->addAction(menuAction[Dental::Pulpitis]);
    endoMenu->addAction(menuAction[Dental::ApicalLesion]);
    endoMenu->addAction(menuAction[Dental::RootCanal]);
    endoMenu->addAction(menuAction[Dental::Post]);

    surgeryMenu->addAction(menuAction[Dental::Missing]);
    surgeryMenu->addAction(menuAction[Dental::Root]);
    surgeryMenu->addAction(menuAction[Dental::Implant]);
    surgeryMenu->addAction(menuAction[Dental::Fracture]);
    surgeryMenu->addAction(menuAction[Dental::HasSupernumeral]);
    surgeryMenu->addAction(menuAction[Dental::Impacted]);

    addAction(menuAction[Dental::Periodontitis]);
    addAction(menuAction[Dental::Calculus]);
    addSeparator();
    addAction(menuAction[Dental::Crown]);
    addAction(menuAction[Dental::Bridge]);
    addAction(menuAction[Dental::Denture]);
    addAction(menuAction[Dental::Splint]);

    addSeparator();
    addAction(otherActions[OtherInputs::removeAll]);

    addSeparator();

    addAction(otherActions[OtherInputs::removeBridge]);
}

void ContextMenu::setSelection(bool single) { details->setEnabled(single); }

void ContextMenu::setModel(const CheckModel& checkModel, const CheckModel& dsnModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.restorationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
    this->setModel(checkModel.mobilityStatus, mobilityDegree);

    dsn_menu->setModel(dsnModel);
}



void ContextMenu::setPresenter(ListPresenter* presenter) { 
    this->presenter = presenter; 
    dsn_menu->setPresenter(presenter);
}

ContextMenu::~ContextMenu() {}
