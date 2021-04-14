#include "SurfacePanelPresenter.h"

void SurfacePanelPresenter::setViewLabels(const Tooth* tooth)
{
	std::array<std::string, 6> unorderedStatus;

	for (int i = 0; i < 6; i++)
	{
		if (tooth->o_surf[i].exists() && tooth->c_surf[i].exists())
		{
			unorderedStatus[i] = " (вторичен кариес)";
		}
		else if (tooth->o_surf[i].exists())
		{
			unorderedStatus[i] = " (обтурация)";
		}
		else if (tooth->c_surf[i].exists())
		{
			unorderedStatus[i] = " (кариес)";
		}
	}

	auto unordererSurfaces = utilities.getSurfaceNames(currentIndex);
	


	std::array<std::string, 6> orderedSurfaces;
	std::array<std::string, 6> orderedStatus;

	for (int i = 0; i < 6; i++) {
		auto intPos = static_cast<int>(matrix.getSurface(currentIndex, static_cast<ButtonPos>(i)));
		orderedSurfaces[i] = unordererSurfaces[intPos];
		orderedStatus[i] = unorderedStatus[intPos];
	}

	
	view->setStatuses(orderedStatus);
	view->setLabels(orderedSurfaces);
	
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

	setViewLabels(tooth);


	view->showPanel(true);
}

