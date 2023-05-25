#pragma once

#include <QIcon>
#include <QMenu>

#include <type_traits>

#include "Presenter/InputEnums.h"
#include "Model/Dental/Enums.h"
#include "Presenter/CheckModel.h"

class ListPresenter;
class CheckModel;

class DsnMenu : public QMenu
{
    Q_OBJECT

    ListPresenter* presenter{ nullptr };

    std::array<QAction*, statusCount> menuAction{ nullptr };
    std::array<QAction*, surfaceCount>surfObt{ nullptr };
    std::array<QAction*, surfaceCount> surfCar{ nullptr };
    std::array<QAction*, mobilityCount> mobilityDegree{ nullptr };


    template <typename ModelArray, typename ActionArray, int CheckStateSize = std::tuple_size<ModelArray>::value>
    void setModel(const ModelArray& model, ActionArray& action)
    {
        for (int i = 0; i < CheckStateSize; i++) {

            if (action[i] == nullptr) continue;

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

public:
    DsnMenu();

    void setModel(const CheckModel& checkModel);
    void setPresenter(ListPresenter * presenter) { this->presenter = presenter; };

    ~DsnMenu(){}
};



