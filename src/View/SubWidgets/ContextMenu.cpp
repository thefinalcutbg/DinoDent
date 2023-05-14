#include "ContextMenu.h"
#include "Presenter/ListPresenter.h"
#include "DsnMenu.h"
#include <QDebug>
#include "View/Theme.h"

ContextMenu::ContextMenu()
{
    setStyleSheet(Theme::getPopupMenuStylesheet());
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
        connect(menuAction[i], &QAction::triggered, [this, i]() { this->presenter->setToothStatus(StatusType::general, i); });
    }

    for (int i = 0; i < otherInputsCount; i++) //too lazy to initialize all the actions;
    {
        otherActions[i] = new QAction(otherActionNames[i]);
        connect(otherActions[i], &QAction::triggered, [this, i]() { this->presenter->setOther(i); });
    }

    addAction(menuAction[StatusCode::Healthy]);
    addAction(menuAction[StatusCode::Temporary]);

  //  addAction(menuAction[StatusCode::Dsn]);

    
    QMenu* obturMenu = addMenu("&Обтурация");
    QMenu* cariesMenu = addMenu("&Кариес");
    QMenu* endoMenu = addMenu("&Ендодонтия");
    QMenu* surgeryMenu = addMenu("&Хирургия");
    QMenu* mobilityMenu = addMenu("&Подвижност");

    dsn_menu = new DsnMenu();
    auto dsnMenuAction = addMenu(dsn_menu);
    dsnMenuAction->setText("Свръхброен зъб");

    QString surfName[surfaceCount] = { "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };

    for (int i = 0; i < surfaceCount; i++)
    {
        surfObt[i] = obturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::obturation, i); });

        surfCar[i] = cariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setToothStatus(StatusType::caries, i); });
    }

    for (int i = 0; i < mobilityCount; i++)
    {
        mobilityDegree[i] = mobilityMenu->addAction(mobilityNames[i].data());
        connect(mobilityDegree[i], &QAction::triggered, [this, i]() {
            this->presenter->setToothStatus(StatusType::mobility, i);; });

    }


    obturMenu->addSeparator();
    obturMenu->addAction(otherActions[OtherInputs::MO]);
    obturMenu->addAction(otherActions[OtherInputs::DO]);
    obturMenu->addAction(otherActions[OtherInputs::MOD]);
    obturMenu->addSeparator();
    obturMenu->addAction(otherActions[OtherInputs::removeO]);

    cariesMenu->addSeparator();
    cariesMenu->addAction(otherActions[OtherInputs::removeC]);

    endoMenu->addAction(menuAction[StatusCode::Pulpitis]);
    endoMenu->addAction(menuAction[StatusCode::ApicalLesion]);
    endoMenu->addAction(menuAction[StatusCode::EndoTreatment]);
    endoMenu->addAction(menuAction[StatusCode::Post]);

    surgeryMenu->addAction(menuAction[StatusCode::Extraction]);
    surgeryMenu->addAction(menuAction[StatusCode::Root]);
    surgeryMenu->addAction(menuAction[StatusCode::Implant]);
    surgeryMenu->addAction(menuAction[StatusCode::Fracture]);
    surgeryMenu->addAction(menuAction[StatusCode::Dsn]);
    surgeryMenu->addAction(menuAction[StatusCode::Impacted]);

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

void ContextMenu::setModel(const CheckModel& checkModel, const CheckModel& dsnModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.obturationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
    this->setModel(checkModel.mobilityStatus, mobilityDegree);
    dsn_menu->setModel(dsnModel);
}



void ContextMenu::setPresenter(ListPresenter* presenter) { 
    this->presenter = presenter; 
    dsn_menu->setPresenter(presenter);
}

ContextMenu::~ContextMenu() {}
