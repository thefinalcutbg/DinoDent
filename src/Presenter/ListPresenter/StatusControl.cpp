#include "StatusControl.h"

StatusControl::StatusControl() {}


void StatusControl::setSelectedTeeth(const std::vector<Tooth*>& selectedTeeth)
{
    this->selectedTeeth = selectedTeeth;
}

void StatusControl::setCheckModel(const CheckModel& checkModel)
{
    this->checkModel = checkModel;
}




void StatusControl::changeStatus(StatusAction templ)
{
    if (templ == StatusAction::Bridge && selectedTeeth.size() < 2) return;

    if (int(templ) < 18)
    {
        changeAnyStatus(int(templ), checkModel.generalStatus, general_controller);
        return;
    }

    for (Tooth *tooth : selectedTeeth)
    {
        general_controller.setTooth(tooth);
        obturation_controller.setTooth(tooth);
        caries_controller.setTooth(tooth);

        switch (templ)
        {
        case StatusAction::DO:
            obturation_controller.removeStatus();
            obturation_controller.addStatus(static_cast<int>(Surface::Distal));
            obturation_controller.addStatus(static_cast<int>(Surface::Occlusal));
            break;
        case StatusAction::MO:
            obturation_controller.removeStatus();
            obturation_controller.addStatus(static_cast<int>(Surface::Medial));
            obturation_controller.addStatus(static_cast<int>(Surface::Occlusal));
            break;
        case StatusAction::MOD:
            obturation_controller.removeStatus();
            obturation_controller.addStatus(static_cast<int>(Surface::Medial));
            obturation_controller.addStatus(static_cast<int>(Surface::Occlusal));
            obturation_controller.addStatus(static_cast<int>(Surface::Distal));
            break;
        case StatusAction::removeAll:
            general_controller.removeStatus();
            break;
        case StatusAction::removeO:
            obturation_controller.removeStatus();
            break;
        case StatusAction::removeC:
            caries_controller.removeStatus();
            break;
        default:
            throw;
        }
    }

}

void StatusControl::changeStatus(Surface surface, SurfaceType type)
{
    switch (type)
    {
    case SurfaceType::obturation:
        changeAnyStatus(static_cast<int>(surface), checkModel.obturationStatus, obturation_controller);
        break;
    case SurfaceType::caries:
        changeAnyStatus(static_cast<int>(surface), checkModel.cariesStatus, caries_controller);
        break;
    }
}


