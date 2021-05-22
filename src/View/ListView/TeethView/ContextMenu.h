#pragma once

#include <QIcon>

#include <QMenu>
#include "Presenter/ListPresenter/IStatusControl.h"
#include "Model/CheckState.h"

#include <type_traits>

class ContextMenu : public QMenu
{
    Q_OBJECT

    IStatusControl* statusControl;

    std::array<QAction*, 6>surfObt;
    std::array<QAction*, 6> surfCar;
    std::array<QAction*, ActionCount> menuAction;


    // int CheckStateSize, int QActionSize
    // const std::array <CheckState, CheckStateSize>& model, std::array<QAction*, QActionSize>& action
    template <typename ModelArray, typename ActionArray, int CheckStateSize = std::tuple_size<ModelArray>::value>
    void setModel(const ModelArray &, ActionArray &);

public:
    ContextMenu(IStatusControl* statusControl);
    void setModel(const CheckModel& checkModel);
    ~ContextMenu();
};


template <typename ModelArray, typename ActionArray, int CheckStateSize>
inline void ContextMenu::setModel(const ModelArray& model, ActionArray& action)
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

