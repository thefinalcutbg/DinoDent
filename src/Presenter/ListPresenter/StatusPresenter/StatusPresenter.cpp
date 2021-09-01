#include "StatusPresenter.h"
#include "../ProcedurePresenter/ProcedurePresenter.h"
#include "ToothHintCreator.h"
#include "Model/Tooth/ToothContainer.h"

#include <QDebug>

std::vector<int> StatusPresenter::getSelectedIndexes()
{
    std::vector<int> selectedIndexes;
    selectedIndexes.reserve(selectedTeeth->size());

    for (int i = 0; i < selectedTeeth->size(); i++)
    {
        selectedIndexes.push_back(selectedTeeth->at(i)->index);
    }

    return selectedIndexes;
}

void StatusPresenter::setProcedurePresenter(ProcedurePresenter* p)
{
    procedure_presenter = p;
}

void StatusPresenter::statusChanged()
{

        m_checkModel = CheckModel(*selectedTeeth);
      
        view->setCheckModel(m_checkModel);;

        for (auto& t : *selectedTeeth)
        {
            view->repaintTooth(ToothHintCreator::getToothHint(*t));
        }

        view->repaintBridges(ToothHintCreator::statusToUIBridge(*teeth));

        if (selectedTeeth->size() == 1)
            surf_presenter.setTooth(selectedTeeth->at(0));
        
        makeEdited();
}


void StatusPresenter::setData(ToothContainer& teeth, std::vector<Tooth*>& selectedTeeth, const std::string& patientID)
{
	this->teeth = &teeth;
    this->patientID = &patientID;
    this->selectedTeeth = &selectedTeeth;

    view->setSelectedTeeth(getSelectedIndexes());

    for (int i = 0; i < teeth.size(); i++)
    {
        view->repaintTooth(ToothHintCreator::getToothHint(teeth[i]));
    }

    view->repaintBridges(ToothHintCreator::statusToUIBridge(teeth));
   
    const int s_teeth = selectedTeeth.size();

    if (s_teeth == 1) surf_presenter.setTooth(selectedTeeth.at(0));
    
    view->hideControlPanel(s_teeth == 0);
    view->hideSurfacePanel(s_teeth != 1);
}

void StatusPresenter::setView(IStatusView* view)
{
    surf_presenter.setStatusControl(this);
    this->view = view;
    view->setStatusControlPresenter(this);
    view->surfacePanel()->setPresenter(&surf_presenter);
}

void StatusPresenter::setObturation(int surface)
{
    bool state = m_checkModel.obturationStatus[surface] != CheckState::checked;

    for (auto& t : *selectedTeeth) t->setStatus(StatusType::obturation, surface, state);

    statusChanged();
}

void StatusPresenter::setCaries(int surface)
{
    bool state = m_checkModel.cariesStatus[surface] != CheckState::checked;

    for (auto& t : *selectedTeeth) t->setStatus(StatusType::caries, surface, state);

    statusChanged();
}

void StatusPresenter::setMainStatus(int code)
{

    bool state = m_checkModel.generalStatus[code] != CheckState::checked;

    for (auto& t : *selectedTeeth)
    {
        t->setStatus(StatusType::general, code, state);
    }

    if (code == StatusCode::Bridge || code == StatusCode::Crown) {
        teeth->formatBridges(getSelectedIndexes());
    }
    else if (code == StatusCode::Temporary)
    {
        if (procedure_presenter)
            procedure_presenter->refreshProcedureView(); //updates the teeth num
    }

    statusChanged();
}

void StatusPresenter::setOther(int code)
{

    auto DO = [](Tooth* t)
    {
        t->removeStatus(StatusCode::Obturation);
        t->setStatus(StatusType::obturation, Surface::Distal);
        t->setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto MO = [](Tooth* t)
    {
        t->removeStatus(StatusCode::Obturation);
        t->setStatus(StatusType::obturation, Surface::Medial);
        t->setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto MOD = [](Tooth* t)
    {
        t->removeStatus(StatusCode::Obturation);
        t->setStatus(StatusType::obturation, Surface::Medial);
        t->setStatus(StatusType::obturation, Surface::Distal);
        t->setStatus(StatusType::obturation, Surface::Occlusal);
    };


    switch (code)
    {
    case OtherInputs::DO: for (auto& t : *selectedTeeth)DO(t); break;
    case OtherInputs::MO: for (auto& t : *selectedTeeth)MO(t); break;
    case OtherInputs::MOD: for (auto& t : *selectedTeeth)MOD(t); break;
    case OtherInputs::removeC: for (auto& t : *selectedTeeth) t->removeStatus(StatusType::caries); break;
    case OtherInputs::removeO: for (auto& t : *selectedTeeth) t->removeStatus(StatusType::obturation); break;
    case OtherInputs::removeBridge: for (auto& t : *selectedTeeth) teeth->removeBridge(t->index);  teeth->formatBridges(getSelectedIndexes()); break;
    case OtherInputs::removeAll: for (auto& t : *selectedTeeth) t->removeStatus();  teeth->formatBridges(getSelectedIndexes()); break;
    }

    statusChanged();
}



void StatusPresenter::setSelectedTeeth(const std::vector<int>& selectedIndexes)
{

    *selectedTeeth = teeth->getSelectedTeethPtr(selectedIndexes);

    m_checkModel = CheckModel{*selectedTeeth};

    view->setCheckModel(m_checkModel);

    const int s_teeth = selectedIndexes.size();

    if (s_teeth == 1) {
        surf_presenter.setTooth(selectedTeeth->at(0));
    }

    view->hideControlPanel(s_teeth == 0);
    view->hideSurfacePanel(s_teeth != 1);

}

#include "Presenter/DetailsPresenter/DetailedStatusPresenter.h"

void StatusPresenter::openDetails(int toothIndex)
{
    DetailedStatusPresenter d(teeth->at(toothIndex), *patientID);

    auto result = d.open();

    if (!result.has_value()) return;

    teeth->at(toothIndex) = result.value();
    teeth->setBridgeProperties(result.value().bridge, toothIndex);

    statusChanged();


}

void StatusPresenter::openDetails()
{
    if (selectedTeeth->size() == 1) openDetails(selectedTeeth->at(0)->index);
}
