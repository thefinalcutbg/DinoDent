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

    details = new QAction("Извършени процедури");
    addAction(details);
    connect(details, &QAction::triggered, [&] {if (presenter) presenter->openDetails(); });
    
    QMenu* statusMenu = addMenu("Зъбен статус");

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

    statusMenu->addAction(menuAction[Dental::Healthy]);
    statusMenu->addAction(menuAction[Dental::Temporary]);

  //  addAction(menuAction[Dental::Dsn]);

    
    QMenu* obturMenu = statusMenu->addMenu("&Възстановяване");
    QMenu* cariesMenu = statusMenu->addMenu("&Патология на ТЗТ");
    QMenu* mobilityMenu = statusMenu->addMenu("&Подвижност");

    dsn_menu = new DsnMenu();
    auto dsnMenuAction = statusMenu->addMenu(dsn_menu);
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

    statusMenu->addSeparator();
    statusMenu->addAction(menuAction[Dental::Pulpitis]);
    statusMenu->addAction(menuAction[Dental::ApicalLesion]);
    statusMenu->addAction(menuAction[Dental::RootCanal]);
    statusMenu->addAction(menuAction[Dental::Post]);
    statusMenu->addSeparator();
    statusMenu->addAction(menuAction[Dental::Missing]);
    statusMenu->addAction(menuAction[Dental::Root]);
    statusMenu->addAction(menuAction[Dental::Implant]);
    statusMenu->addAction(menuAction[Dental::Fracture]);
    statusMenu->addAction(menuAction[Dental::Impacted]);
    statusMenu->addAction(menuAction[Dental::HasSupernumeral]);
    statusMenu->addSeparator();
    statusMenu->addAction(menuAction[Dental::Periodontitis]);
    statusMenu->addAction(menuAction[Dental::Calculus]);
    statusMenu->addSeparator();
    statusMenu->addAction(menuAction[Dental::Crown]);
    statusMenu->addAction(menuAction[Dental::Bridge]);
    statusMenu->addAction(menuAction[Dental::Denture]);
    statusMenu->addAction(menuAction[Dental::Splint]);
    statusMenu->addSeparator();
    statusMenu->addAction(otherActions[OtherInputs::removeAll]);
    statusMenu->addAction(otherActions[OtherInputs::removeBridge]);
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
