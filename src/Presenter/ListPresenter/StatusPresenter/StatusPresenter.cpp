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
    qDebug() << "refreshing status";

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


void StatusPresenter::setData(ToothContainer& teeth, std::vector<Tooth*>& selectedTeeth)
{
	this->teeth = &teeth;
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
    qDebug() << "changing main status";

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
    case OtherInputs::removeAll: for (auto& t : *selectedTeeth) t->removeStatus(); break;
    case OtherInputs::removeC: for (auto& t : *selectedTeeth) t->removeStatus(StatusType::caries); break;
    case OtherInputs::removeO: for (auto& t : *selectedTeeth) t->removeStatus(StatusType::obturation); break;
    case OtherInputs::removeBridge: for (auto& t : *selectedTeeth) teeth->removeBridge(t->index); break;
    }

    statusChanged();
}



void StatusPresenter::setSelectedTeeth(const std::vector<int>& selectedIndexes)
{
    qDebug() << "setting selected teeth";

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

#include "Presenter/DetailsPresenter/DetailsPresenter.h"

void StatusPresenter::openDetails(int toothIndex)
{
    DetailsPresenter d(teeth->at(toothIndex));

    auto tooth = d.open();

    if (tooth.has_value())
    {
        teeth->at(toothIndex) = tooth.value();
    }

}

void StatusPresenter::openDetails()
{
    if (selectedTeeth->size() == 1) openDetails(selectedTeeth->at(0)->index);
}
