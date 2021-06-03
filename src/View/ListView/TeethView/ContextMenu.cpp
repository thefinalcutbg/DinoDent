#include "ContextMenu.h"


ContextMenu::ContextMenu()
{

    QString actionNames[ActionCount] =
                        { "Временен зъб", "Обтурация", "Кариес",  "Пулпит", "Периодонтит",
                           "Ендодонтско лечение", "Радикуларен щифт", "Корен", "Фрактура", "Екстракция",
                            "Пародонтит", "Първа степен", "Втора степен", "Трета степен",
                           "Корона", "Мост/Блок корони",  "Имплант", "Свръхброен зъб",
                            "Обтурация МО", "Обтурация ДО", "Обтурация МОД",
                            "Премахни"/*обтурация*/, "Премахни"/*кариес*/, "Премахни статус", "Премахни мост"};

    for (int i = 0; i < ActionCount; i++) //too lazy to initialize all the actions;
    {
        menuAction[i] = new QAction(actionNames[i]);
        connect(menuAction[i], &QAction::triggered, [this, i]() { this->presenter->changeStatus(static_cast<StatusAction>(i)); });
    }
    addAction(menuAction[static_cast<int>(StatusAction::Temporary)]);

    QMenu* ObturMenu = addMenu("&Обтурация");
    QMenu* CariesMenu = addMenu("&Кариес");
    QMenu* EndoMenu = addMenu("&Ендодонтия");
    QMenu* SurgeryMenu = addMenu("&Хирургия");

    QString surfName[6] = { "&Оклузално", "&Медиално", "&Дистално", "&Букално", "&Лингвално", "&Цервикално" };

    for (int i = 0; i < 6; i++)
    {
        surfObt[i] = ObturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->changeStatus(static_cast<Surface>(i), SurfaceType::obturation); });

        surfCar[i] = CariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->changeStatus(static_cast<Surface>(i), SurfaceType::caries); });
    }


    ObturMenu->addSeparator();
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::MO)]);
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::DO)]);
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::MOD)]);
    ObturMenu->addSeparator();
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::removeO)]);

    CariesMenu->addSeparator();
    CariesMenu->addAction(menuAction[static_cast<int>(StatusAction::removeC)]);

    EndoMenu->addAction(menuAction[static_cast<int>(StatusAction::Pulpitis)]);
    EndoMenu->addAction(menuAction[static_cast<int>(StatusAction::ApicalLesion)]);
    EndoMenu->addAction(menuAction[static_cast<int>(StatusAction::EndoTreatment)]);
    EndoMenu->addAction(menuAction[static_cast<int>(StatusAction::Post)]);

    SurgeryMenu->addAction(menuAction[static_cast<int>(StatusAction::Extraction)]);
    SurgeryMenu->addAction(menuAction[static_cast<int>(StatusAction::Root)]);
    SurgeryMenu->addAction(menuAction[static_cast<int>(StatusAction::Implant)]);
    SurgeryMenu->addAction(menuAction[static_cast<int>(StatusAction::Fracture)]);
    SurgeryMenu->addAction(menuAction[static_cast<int>(StatusAction::Dsn)]);

    addAction(menuAction[static_cast<int>(StatusAction::Periodontitis)]);
    QMenu* MobiMenu = addMenu("Подвижност");
    MobiMenu->addAction(menuAction[static_cast<int>(StatusAction::Mobility1)]);
    MobiMenu->addAction(menuAction[static_cast<int>(StatusAction::Mobility2)]);
    MobiMenu->addAction(menuAction[static_cast<int>(StatusAction::Mobility3)]);
    addSeparator();
    addAction(menuAction[static_cast<int>(StatusAction::Crown)]);
    addAction(menuAction[static_cast<int>(StatusAction::Bridge)]);
    addSeparator();




    addSeparator();

//    QAction* bridgeRemove = addAction("&Премахни мост");
//    connect(bridgeRemove, &QAction::triggered, [this]() {  });
    addAction(menuAction[static_cast<int>(StatusAction::removeAll)]);

    addSeparator();

    addAction(menuAction[static_cast<int>(StatusAction::removeBridge)]);
}

void ContextMenu::setModel(const CheckModel& checkModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.obturationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
}

void ContextMenu::setStatusControl(IStatusControl* presenter)
{
    this->presenter = presenter;
}


ContextMenu::~ContextMenu()
{
}
