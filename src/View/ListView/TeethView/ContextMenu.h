#pragma once

#include <QIcon>
#include <QMenu>

#include <type_traits>

#include "Model/CheckState.h"


class ProcedurePresenter;
class StatusPresenter;

class ContextMenu : public QMenu
{
    Q_OBJECT

    StatusPresenter* status_presenter{ nullptr };
    ProcedurePresenter* procedure_presenter{ nullptr };

    QAction* addProcedure;
    QAction* details;
    std::array<QAction*, 6>surfObt;
    std::array<QAction*, 6> surfCar;
    std::array<QAction*, 25> menuAction;


    template <typename ModelArray, typename ActionArray, int CheckStateSize = std::tuple_size<ModelArray>::value>
    void setModel(const ModelArray &, ActionArray &);

public:
    ContextMenu();
    void setSelection(bool single);
    void setModel(const CheckModel& checkModel);
    void setStatusPresenter(StatusPresenter* presenter);
    void setProcedurePresenter(ProcedurePresenter* presenter);
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

