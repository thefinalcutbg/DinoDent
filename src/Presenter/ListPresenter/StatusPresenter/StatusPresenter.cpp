#include "StatusPresenter.h"

std::vector<Tooth*> StatusPresenter::getSelectedTeethPointers()
{
	std::vector<Tooth*> selectedTeethPointers;
	selectedTeethPointers.reserve(selectedIndexes->size());
    
	for (int index : *selectedIndexes)
	{
		selectedTeethPointers.emplace_back(&teeth->at(index));
	}

	return selectedTeethPointers;
}

void StatusPresenter::statusChanged()
{
        auto m = checkCreator.refreshModel(getSelectedTeethPointers());
        view->setCheckModel(m);
        statusControl.setCheckModel(m);

        for (int i : *selectedIndexes)
        {

            view->repaintTooth(paint_hint_generator.getToothHint(teeth->at(i)));
        }

        view->repaintBridges(paint_hint_generator.statusToUIBridge(*teeth));

        selectedIndexes->size() == 1 ?

            view->updateControlPanel(&teeth->at(selectedIndexes->at(0)))
            :
            view->updateControlPanel(nullptr);
        
        makeEdited();

}


void StatusPresenter::setData(std::array<Tooth, 32>& teeth, std::vector<int>& selectedIndexes)
{
	this->teeth = &teeth;
    this->selectedIndexes = &selectedIndexes;

    view->setSelectedTeeth(selectedIndexes);

    for (auto& t : teeth)
    {
        view->repaintTooth(paint_hint_generator.getToothHint(t));
    }

    view->repaintBridges(paint_hint_generator.statusToUIBridge(teeth));
   
    if (selectedIndexes.size() == 1) {

        view->updateControlPanel(&teeth.at(selectedIndexes.at(0)));
    }
    else {
        view->updateControlPanel(nullptr);
    }
    
}

void StatusPresenter::setView(IStatusView* view)
{
    this->view = view;
    view->setStatusControlPresenter(this);
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
        for (int i : *selectedIndexes)
        {
            bridgeController.removeBridge(i, teeth);
        }
        view->repaintBridges(paint_hint_generator.statusToUIBridge(*teeth));
        return;
    }

    statusControl.changeStatus(status);

    if (status == StatusAction::Bridge || status == StatusAction::Crown) {
        bridgeController.formatBridges(*selectedIndexes, teeth);
    }

    statusChanged();
}

void StatusPresenter::setSelectedTeeth(const std::vector<int>& SelectedIndexes)
{
  
    *selectedIndexes = SelectedIndexes;

    auto selectedTeeth = getSelectedTeethPointers();
    statusControl.setSelectedTeeth(selectedTeeth);

    auto m = checkCreator.refreshModel(selectedTeeth);

    statusControl.setCheckModel(m);
    view->setCheckModel(m);

    if (
		SelectedIndexes.size() == 1) {
        view->updateControlPanel(selectedTeeth.at(0));
    }
    else {
        view->updateControlPanel(nullptr);
    }

}
