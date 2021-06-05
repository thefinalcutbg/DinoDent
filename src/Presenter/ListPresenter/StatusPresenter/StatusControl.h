#pragma once
#include <array>
#include <vector>
#include <tuple>

#include "Model/CheckState.h"
#include "Model/Tooth/ToothController/GeneralStatusControler.h"
#include "Model/Tooth/ToothController/SurfaceController.h"
#include "InputEnums.h"


class StatusControl
{
    std::vector<Tooth*> selectedTeeth;

    CheckModel checkModel;

    GeneralStatusController general_controller;
    ObturationController obturation_controller;
    CariesController caries_controller;

    template <typename Container>
    typename std::enable_if<(std::tuple_size<Container>::value > 0)>::type changeAnyStatus(int statusIndex, Container& arrayModel, ToothController& controller);

public:
    StatusControl();

    void setSelectedTeeth(const std::vector<Tooth*>& selectedTeeth);
    void setCheckModel(const CheckModel& checkModel);

    void changeStatus(StatusAction code);
    void changeStatus(Surface surface, SurfaceType type);

};

template <typename Container>
inline typename std::enable_if<(std::tuple_size<Container>::value > 0)>::type StatusControl::changeAnyStatus(int statusIndex, Container& arrayModel, ToothController& controller)
{
    if (arrayModel[statusIndex] == CheckState::checked)
    {
        for (Tooth *tooth : selectedTeeth)
        {

            // removing status
            controller.setTooth(tooth);
            controller.removeStatus(statusIndex);
        }
    }
    else
    {
        for (Tooth *tooth : selectedTeeth)
        {
            // applying status
            controller.setTooth(tooth);
            controller.addStatus(statusIndex);
        }
    }
}

