#include "SurfacePanelPresenter.h"

void SurfacePanelPresenter::setViewLabels()
{
	auto unorderedNames = utilities.getSurfaceNames(currentIndex);

	std::array<std::string, 6> orderedNames;

	for (int i = 0; i < 6; i++) {
		auto intPos = static_cast<int>(matrix.getSurface(currentIndex, static_cast<ButtonPos>(i)));
		orderedNames[i] = unorderedNames[intPos];
	}
	view->setLabels(orderedNames);
	
}

SurfacePanelPresenter::SurfacePanelPresenter(ISurfacePanel* view) : statusControl(NULL), currentIndex(-1)
{
	this->view = view;
}

void SurfacePanelPresenter::getStatusControl(IStatusControl* statusControl)
{
	this->statusControl = statusControl;
}

void SurfacePanelPresenter::buttonClicked(ButtonPos position, SurfaceType type)
{
	Surface surface = matrix.getSurface(currentIndex, position);

	statusControl->changeStatus(surface, type);
}

void SurfacePanelPresenter::setTooth(const Tooth* tooth)
{
	if (tooth == NULL) {
		view->showPanel(false);
		return;
	}

	view->paintTooth(tooth);

	currentIndex = tooth->index;

	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

	view->setSideButtonsClicked(
		tooth->o_surf[static_cast<int>(surface)].exists() && tooth->obturation,
		tooth->c_surf[static_cast<int>(surface)].exists() && tooth->caries
	);

	setViewLabels();

	view->showPanel(true);
}

