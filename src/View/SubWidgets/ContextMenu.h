#pragma once

#include <QIcon>
#include <QMenu>

#include <type_traits>

#include "Presenter/InputEnums.h"
#include "Model/Dental/Dental.h"
#include "Presenter/CheckModel.h"

class DsnMenu;
class ListPresenter;
class CheckModel;

class ContextMenu : public QMenu
{
    Q_OBJECT

    ListPresenter* presenter{ nullptr };
    DsnMenu* dsn_menu;

    QAction* addProcedure;
    QAction* details;
    std::array<QAction*, Dental::SurfaceCount> surfObt;
    std::array<QAction*, Dental::SurfaceCount> surfCar;
    std::array<QAction*, Dental::SurfaceCount> surfDefObt;
    std::array<QAction*, Dental::SurfaceCount> surfNonCar;
    std::array<QAction*, Dental::StatusCount> menuAction;
    std::array<QAction*, Dental::MobilityCount> mobilityDegree;
    std::array<QAction*, otherInputsCount> otherActions;


    template <typename ModelArray, typename ActionArray, int CheckStateSize = std::tuple_size<ModelArray>::value>
    void setModel(const ModelArray &, ActionArray &);

public:
    ContextMenu();
    void setSelection(bool single);
    void setModel(const CheckModel& checkModel, const CheckModel& dsnModel);
    void setPresenter(ListPresenter* presenter);

    ~ContextMenu();
};


template <typename ModelArray, typename ActionArray, int CheckStateSize>
inline void ContextMenu::setModel(const ModelArray& model, ActionArray& action)
{

    for (int i = 0; i < CheckStateSize; i++) {

        switch (model[i])
        {
        case CheckState::checked:
            action[i]->setIcon(QIcon(QPixmap(":/icons/icon_checked.png")));
            break;
        case CheckState::partially_checked:
            action[i]->setIcon(QIcon(QPixmap(":/icons/icon_tristate.png")));
            break;
        default:
            action[i]->setIcon(QIcon());
        }
    }
}

