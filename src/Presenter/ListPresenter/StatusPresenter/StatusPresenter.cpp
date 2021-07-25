#include "StatusPresenter.h"
#include "../ProcedurePresenter/ProcedurePresenter.h"

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
            view->repaintTooth(paint_hint_generator.getToothHint(*t));
        }

        view->repaintBridges(paint_hint_generator.statusToUIBridge(*teeth));

        if (selectedTeeth->size() == 1)
            surf_presenter.setTooth(selectedTeeth->at(0));
        
        makeEdited();
}


void StatusPresenter::setData(std::array<Tooth, 32>& teeth, std::vector<Tooth*>& selectedTeeth)
{
	this->teeth = &teeth;
    this->selectedTeeth = &selectedTeeth;

    view->setSelectedTeeth(getSelectedIndexes());

    for (auto& t : teeth)
    {
        view->repaintTooth(paint_hint_generator.getToothHint(t));
    }

    view->repaintBridges(paint_hint_generator.statusToUIBridge(teeth));
   
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
            bridgeController.removeBridge(t->index, teeth);
        }
        view->repaintBridges(paint_hint_generator.statusToUIBridge(*teeth));
        return;
    }

    statusControl.changeStatus(status);

    if (status == StatusAction::Bridge || status == StatusAction::Crown) {
        bridgeController.formatBridges(getSelectedIndexes(), teeth);
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

    const auto m = checkCreator.refreshModel(*selectedTeeth);

    statusControl.setCheckModel(m);
    view->setCheckModel(m);

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
