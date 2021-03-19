#include "ContextMenu.h"

ContextMenu::ContextMenu(IStatusControl* statusControl)
	:  statusControl(statusControl)
{

    QMenu* ObturMenu = addMenu("&Обтурация");
    QMenu* CariesMenu = addMenu("&Кариес");
    
    QString surfName[6] = { "&Оклузално", "&Медиално", "&Дистално", "&Букално", "&Лингвално", "&Цервикално" };

    for (int i = 0; i < 6; i++)
    {
        surfObt[i] = ObturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->statusControl->changeStatus(static_cast<Surface>(i), SurfaceType::obturation);});

        surfCar[i] = CariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->statusControl->changeStatus(static_cast<Surface>(i), SurfaceType::caries);});
    }

    QString actionNames[23] = { "Временен зъб", "Обтурация", "Кариес",  "Пулпит", "Периодонтит",
                           "Ендодонтско лечение", "Корен", "Фрактура", "Екстракция",
                            "Пародонтит", "Първа степен", "Втора степен", "Трета степен",
                           "Корона", "Мост/Блок корони",  "Имплант", "Свръхброен зъб",
                            "Обтурация МО", "Обтурация ДО", "Обтурация МОД",
                            "Премахни"/*обтурация*/, "Премахни"/*кариес*/, "Премахни статус"};

    for (int i = 0; i < 23; i++) //too lazy to initialize all the actions;
    {
        menuAction[i] = new QAction(actionNames[i]);
        connect(menuAction[i], &QAction::triggered, [this, i]() { this->statusControl->changeStatus(static_cast<StatusAction>(i)); });
    }

    ObturMenu->addSeparator();
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::MO)]);
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::DO)]);
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::MOD)]);
    ObturMenu->addSeparator();
    ObturMenu->addAction(menuAction[static_cast<int>(StatusAction::removeO)]);

    CariesMenu->addSeparator();
    CariesMenu->addAction(menuAction[static_cast<int>(StatusAction::removeC)]);

    addAction(menuAction[static_cast<int>(StatusAction::ApicalLesion)]);
    addAction(menuAction[static_cast<int>(StatusAction::Pulpitis)]);
    addAction(menuAction[static_cast<int>(StatusAction::EndoTreatment)]);
    addSeparator();

    addAction(menuAction[static_cast<int>(StatusAction::Root)]);
    addAction(menuAction[static_cast<int>(StatusAction::Extraction)]);
    addAction(menuAction[static_cast<int>(StatusAction::Implant)]);

    addSeparator();

    addAction(menuAction[static_cast<int>(StatusAction::Periodontitis)]);
    QMenu* MobiMenu = addMenu("Подвижност");
    MobiMenu->addAction(menuAction[static_cast<int>(StatusAction::Mobility1)]);
    MobiMenu->addAction(menuAction[static_cast<int>(StatusAction::Mobility2)]);
    MobiMenu->addAction(menuAction[static_cast<int>(StatusAction::Mobility3)]);
    addSeparator();
    addAction(menuAction[static_cast<int>(StatusAction::Crown)]);
    addAction(menuAction[static_cast<int>(StatusAction::Bridge)]);
    addSeparator();

    addAction(menuAction[static_cast<int>(StatusAction::Temporary)]);
    addAction(menuAction[static_cast<int>(StatusAction::Dsn)]);

    addSeparator();
    QAction* bridgeRemove = addAction("&Премахни мост");
    connect(bridgeRemove, &QAction::triggered, [this]() {  });
    addAction(menuAction[static_cast<int>(StatusAction::removeAll)]);

}

void ContextMenu::setModel(const CheckModel& checkModel)
{
    setModel(checkModel.generalStatus, menuAction);
    setModel(checkModel.obturationStatus, surfObt);
    setModel(checkModel.cariesStatus, surfCar);
}

template<int CheckStateSize, int QActionSize>
void ContextMenu::setModel(const std::array<CheckState, CheckStateSize>& model, std::array<QAction*, QActionSize>& action)
{

    for (int i = 0; i < CheckStateSize; i++) {

        switch (model[i])
        {
        case CheckState::checked:
            action[i]->setIcon(QIcon(QPixmap("checked.png")));
            break;
        case CheckState::partially_checked:
            action[i]->setIcon(QIcon(QPixmap("tristate.png")));
            break;
        default:
            action[i]->setIcon(QIcon());
        }
    }
}

ContextMenu::~ContextMenu()
{
}
