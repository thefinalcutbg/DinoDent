#include "SurfacePanelPresenter.h"
#include "../StatusControl.h"



SurfacePanelPresenter::SurfacePanelPresenter() : statusControl(nullptr), view(nullptr), currentIndex(-1)
{}

void SurfacePanelPresenter::setView(ISurfacePanel* view)
{
	this->view = view;
}

void SurfacePanelPresenter::setStatusControl(IStatusControl* s_ctrl)
{
	this->statusControl = s_ctrl;
}


void SurfacePanelPresenter::setViewLabels(const Tooth* tooth)
{
	std::array<std::string, 6> unorderedStatus;

	for (int i = 0; i < 6; i++)
	{
		if (tooth->obturation.exists(i) && tooth->caries.exists(i))
		{
			unorderedStatus[i] = " (вторичен кариес)";
		}
		else if (tooth->obturation.exists(i))
		{
			unorderedStatus[i] = " (обтурация)";
		}
		else if (tooth->caries.exists(i))
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



void SurfacePanelPresenter::sideButtonClicked(SurfaceType surf_type)
{
	Surface surface = matrix.getSurface(currentIndex, ButtonPos::side);

	statusControl->changeStatus(surface, surf_type);
}



void SurfacePanelPresenter::buttonClicked(ButtonPos position, SurfaceClick click)
{
	Surface surface = matrix.getSurface(currentIndex, position);

	if (click == SurfaceClick::leftClick)
		switch (surfaceState[static_cast<int>(position)])
		{
		case SurfaceState::none:
			statusControl->changeStatus(surface, SurfaceType::obturation);
			break;
		case SurfaceState::obturation:
			statusControl->changeStatus(surface, SurfaceType::caries);
			statusControl->changeStatus(surface, SurfaceType::obturation);
			break;
		case SurfaceState::caries:
			statusControl->changeStatus(surface, SurfaceType::obturation);
			break;
		case SurfaceState::secondary:
			statusControl->changeStatus(surface, SurfaceType::caries);
			break;
		}
	else
		switch (surfaceState[static_cast<int>(position)])
		{ 
		case::SurfaceState::obturation:
			statusControl->changeStatus(surface, SurfaceType::obturation);
			break;
		case::SurfaceState::caries:
			statusControl->changeStatus(surface, SurfaceType::caries);
			break;
		case::SurfaceState::secondary:
			statusControl->changeStatus(surface, SurfaceType::obturation);
			statusControl->changeStatus(surface, SurfaceType::caries);
			break;
		case SurfaceState::none:
			break;
		};

}

void SurfacePanelPresenter::setTooth(Tooth* tooth)
{
	currentIndex = tooth->index;
	view->paintTooth(paint_hint_generator.getToothHint(*tooth));
	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

	view->setSideButtonsClicked(
		tooth->obturation.exists(static_cast<int>(surface)),
		tooth->caries.exists(static_cast<int>(surface))
	);

	for (int i = 0; i<5; i++)
	{
		auto surface = static_cast<int>(matrix.getSurface(currentIndex, static_cast<ButtonPos>(i)));

		if (tooth->obturation.exists(surface) && tooth->caries.exists(surface))
			surfaceState[i] = SurfaceState::secondary;

		else if (tooth->obturation.exists(surface))
			surfaceState[i] = SurfaceState::obturation;

		else if (tooth->caries.exists(surface))
			surfaceState[i] = SurfaceState::caries;

		else
			surfaceState[i] = SurfaceState::none;

	}

	setViewLabels(tooth);

}

