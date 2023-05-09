#include "ContextMenu.h"
#include "Presenter/ListPresenter.h"
#include <QDebug>
ContextMenu::ContextMenu()
{

    addProcedure = new QAction("Добави манипулация");
    addAction(addProcedure);
    connect(addProcedure, &QAction::triggered, [&] {if(presenter)presenter->addProcedure(); });

    details = new QAction("Детайли");
    addAction(details);
    connect(details, &QAction::triggered, [&] {if (presenter) presenter->openDetails(); });
    
    addSeparator();

    QString otherActionNames[otherInputsCount]
    {
        "Обтурация МО", "Обтурация ДО", "Обтурация МОД",
        "Премахни"/*обтурация*/, "Премахни"/*кариес*/, "Премахни статус", "Премахни конструкция/шина"
    };


    for (int i = 0; i < statusCount; i++) //too lazy to initialize all the actions;
    {
        menuAction[i] = new QAction(statusNames[i].data());
        connect(menuAction[i], &QAction::triggered, [this, i]() { this->presenter->setMainStatus(i); });
    }

    for (int i = 0; i < otherInputsCount; i++) //too lazy to initialize all the actions;
    {
        otherActions[i] = new QAction(otherActionNames[i]);
        connect(otherActions[i], &QAction::triggered, [this, i]() { this->presenter->setOther(i); });
    }

    addAction(menuAction[StatusCode::Healthy]);
    addAction(menuAction[StatusCode::Temporary]);
    
    QMenu* ObturMenu = addMenu("&Обтурация");
    QMenu* CariesMenu = addMenu("&Кариес");
    QMenu* EndoMenu = addMenu("&Ендодонтия");
    QMenu* SurgeryMenu = addMenu("&Хирургия");
    QMenu* MobilityMenu = addMenu("&Подвижност");

    QString surfName[surfaceCount] = { "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };

    for (int i = 0; i < surfaceCount; i++)
    {
        surfObt[i] = ObturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setObturation(i); });

        surfCar[i] = CariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setCaries(i); });
    }

    for (int i = 0; i < mobilityCount; i++)
    {
        mobilityDegree[i] = MobilityMenu->addAction(mobilityNames[i].data());
        connect(mobilityDegree[i], &QAction::triggered, [this, i]() {
            this->presenter->setMobility(i); });

    }


    ObturMenu->addSeparator();
    ObturMenu->addAction(otherActions[OtherInputs::MO]);
    ObturMenu->addAction(otherActions[OtherInputs::DO]);
    ObturMenu->addAction(otherActions[OtherInputs::MOD]);
    ObturMenu->addSeparator();
    ObturMenu->addAction(otherActions[OtherInputs::removeO]);

    CariesMenu->addSeparator();
    CariesMenu->addAction(otherActions[OtherInputs::removeC]);

    EndoMenu->addAction(menuAction[StatusCode::Pulpitis]);
    EndoMenu->addAction(menuAction[StatusCode::ApicalLesion]);
    EndoMenu->addAction(menuAction[StatusCode::EndoTreatment]);
    EndoMenu->addAction(menuAction[StatusCode::Post]);

    SurgeryMenu->addAction(menuAction[StatusCode::Extraction]);
    SurgeryMenu->addAction(menuAction[StatusCode::Root]);
    SurgeryMenu->addAction(menuAction[StatusCode::Implant]);
    SurgeryMenu->addAction(menuAction[StatusCode::Fracture]);
    SurgeryMenu->addAction(menuAction[StatusCode::Dsn]);
    SurgeryMenu->addAction(menuAction[StatusCode::Impacted]);

    addAction(menuAction[StatusCode::Periodontitis]);
    addAction(menuAction[StatusCode::Calculus]);
    addSeparator();
    addAction(menuAction[StatusCode::Crown]);
    addAction(menuAction[StatusCode::Bridge]);
    addAction(menuAction[StatusCode::Denture]);
    addAction(menuAction[StatusCode::FiberSplint]);
    addSeparator();

    addAction(otherActions[OtherInputs::removeAll]);

    addSeparator();

    addAction(otherActions[OtherInputs::removeBridge]);
}

void ContextMenu::setSelection(bool single) { details->setEnabled(single); }

void ContextMenu::setModel(const CheckModel& checkModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.obturationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
    this->setModel(checkModel.mobilityStatus, mobilityDegree);
}



void ContextMenu::setPresenter(ListPresenter* presenter) { this->presenter = presenter; }

ContextMenu::~ContextMenu() {}
