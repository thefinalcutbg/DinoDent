#include "ContextMenu.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include <QDebug>
ContextMenu::ContextMenu()
{

    addProcedure = new QAction(u8"Добави манипулация");
    addAction(addProcedure);
    connect(addProcedure, &QAction::triggered, [&] {if(presenter)presenter->addProcedure(); });

    details = new QAction(u8"Детайли");
    addAction(details);
    connect(details, &QAction::triggered, [&] {if (presenter) presenter->openDetails(); });
    
    addSeparator();

    QString otherActionNames[otherInputsCount]
    {
        u8"Обтурация МО", u8"Обтурация ДО", u8"Обтурация МОД",
        u8"Премахни"/*обтурация*/, u8"Премахни"/*кариес*/, u8"Премахни статус", u8"Премахни мост"
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

    addAction(menuAction[StatusCode::Temporary]);

    QMenu* ObturMenu = addMenu(u8"&Обтурация");
    QMenu* CariesMenu = addMenu(u8"&Кариес");
    QMenu* EndoMenu = addMenu(u8"&Ендодонтия");
    QMenu* SurgeryMenu = addMenu(u8"&Хирургия");

    QString surfName[surfaceCount] = { u8"Оклузално", u8"Медиално", u8"Дистално", u8"Букално", u8"Лингвално", u8"Цервикално" };

    for (int i = 0; i < surfaceCount; i++)
    {
        surfObt[i] = ObturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setObturation(i);  qDebug() << "presenter ptr:" << presenter; });

        surfCar[i] = CariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setCaries(i); });
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
    QMenu* MobiMenu = addMenu("Подвижност");
    MobiMenu->addAction(menuAction[StatusCode::Mobility1]);
    MobiMenu->addAction(menuAction[StatusCode::Mobility2]);
    MobiMenu->addAction(menuAction[StatusCode::Mobility3]);
    addSeparator();
    addAction(menuAction[StatusCode::Crown]);
    addAction(menuAction[StatusCode::Bridge]);
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
}



void ContextMenu::setPresenter(ListPresenter* presenter) { this->presenter = presenter; }

ContextMenu::~ContextMenu() {}
