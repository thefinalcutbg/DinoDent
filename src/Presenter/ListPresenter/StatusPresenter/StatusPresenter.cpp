#include "StatusPresenter.h"
#include "../ProcedurePresenter/ProcedurePresenter.h"
#include "ToothHintCreator.h"
#include "Model/Tooth/ToothContainer.h"

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
        auto m = checkCreator.refreshModel(*selectedTeeth);
      
        view->setCheckModel(m);
        statusControl.setCheckModel(m);



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

void StatusPresenter::changeStatus(Surface surface, SurfaceType type)
{
	statusControl.changeStatus(surface, type);
    statusChanged();
}

void StatusPresenter::changeStatus(StatusAction status)
{
    if (status == StatusAction::removeBridge)
    {
        for (auto& t : *selectedTeeth)
        {
            teeth->removeBridge(t->index);
        }
        view->repaintBridges(ToothHintCreator::statusToUIBridge(*teeth));
        return;
    }

    statusControl.changeStatus(status);

    if (status == StatusAction::Bridge || status == StatusAction::Crown) {
        teeth->formatBridges(getSelectedIndexes());
    }
    else if (status == StatusAction::Temporary)
    {
        if(procedure_presenter)
        procedure_presenter->refreshProcedureView(); //updates the teeth num
    }

    statusChanged();
}

void StatusPresenter::setSelectedTeeth(const std::vector<int>& selectedIndexes)
{

    selectedTeeth->clear();
    for (int i : selectedIndexes)
    {
        selectedTeeth->push_back(&teeth->at(i));
    }


    statusControl.setSelectedTeeth(*selectedTeeth);

    CheckModel model{ *teeth, selectedIndexes };

    statusControl.setCheckModel(model);
    view->setCheckModel(model);

    const int s_teeth = selectedIndexes.size();

    if (s_teeth == 1) {
        surf_presenter.setTooth(selectedTeeth->at(0));
    }

    view->hideControlPanel(s_teeth == 0);
    view->hideSurfacePanel(s_teeth != 1);

}

#include "Presenter/DetailsPresenter/DetailsPresenter.h"
#include <QDebug>
void StatusPresenter::openDetails(int toothIndex)
{
    DetailsPresenter d;

    auto tooth = d.open(teeth->at(toothIndex));

    if (tooth.has_value())
    {
        teeth->at(toothIndex) = tooth.value();
    }

}

void StatusPresenter::openDetails()
{
    if (selectedTeeth->size() == 1) openDetails(selectedTeeth->at(0)->index);
}
