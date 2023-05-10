#include "DsnMenu.h"
#include "Presenter/ListPresenter.h"

DsnMenu::DsnMenu()
{

    for (int i = 0; i < statusCount; i++)
    {
        if (
            i == static_cast<int>(StatusCode::Bridge) ||
            i == static_cast<int>(StatusCode::FiberSplint) ||
            i == static_cast<int>(StatusCode::Implant) ||
            i == static_cast<int>(StatusCode::Denture) ||
            i == static_cast<int>(StatusCode::Dsn)
        )
        {
            continue;
        }

        menuAction[i] = new QAction(statusNames[i].data());

        connect(menuAction[i], &QAction::triggered, [this, i]() { this->presenter->setDsnStatus(StatusType::general, i); });
    }

    addAction(menuAction[StatusCode::Healthy]);
    addAction(menuAction[StatusCode::Temporary]);

    QMenu* ObturMenu = addMenu("&Обтурация");
    QMenu* CariesMenu = addMenu("&Кариес");
    QMenu* MobilityMenu = addMenu("&Подвижност");

    QString surfName[surfaceCount] = { "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };

    for (int i = 0; i < surfaceCount; i++)
    {
        surfObt[i] = ObturMenu->addAction(surfName[i]);
        connect(surfObt[i], &QAction::triggered, [this, i]() {this->presenter->setDsnStatus(StatusType::obturation, i); });

        surfCar[i] = CariesMenu->addAction(surfName[i]);
        connect(surfCar[i], &QAction::triggered, [this, i]() {this->presenter->setDsnStatus(StatusType::caries, i); });
    }

    for (int i = 0; i < mobilityCount; i++)
    {
        mobilityDegree[i] = MobilityMenu->addAction(mobilityNames[i].data());
        connect(mobilityDegree[i], &QAction::triggered, [this, i]() {
            this->presenter->setDsnStatus(StatusType::mobility, i); });

    }

    addSeparator();
   
    addAction(menuAction[StatusCode::Pulpitis]);
    addAction(menuAction[StatusCode::ApicalLesion]);
    addAction(menuAction[StatusCode::EndoTreatment]);
    addAction(menuAction[StatusCode::Post]);
    addAction(menuAction[StatusCode::Extraction]);
    addAction(menuAction[StatusCode::Root]);
    addAction(menuAction[StatusCode::Fracture]);
    addAction(menuAction[StatusCode::Impacted]);
    addAction(menuAction[StatusCode::Periodontitis]);
    addAction(menuAction[StatusCode::Calculus]);
    addAction(menuAction[StatusCode::Crown]);

}

void DsnMenu::setModel(const CheckModel& checkModel)
{
    this->setModel(checkModel.generalStatus, menuAction);
    this->setModel(checkModel.obturationStatus, surfObt);
    this->setModel(checkModel.cariesStatus, surfCar);
    this->setModel(checkModel.mobilityStatus, mobilityDegree);
}