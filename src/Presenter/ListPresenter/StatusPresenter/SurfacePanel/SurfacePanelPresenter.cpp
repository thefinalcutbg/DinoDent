#include "SurfacePanelPresenter.h"

#include "../StatusPresenter.h"

SurfacePanelPresenter::SurfacePanelPresenter() : statusControl(nullptr), view(nullptr), currentIndex(-1)
{
}

void SurfacePanelPresenter::setView(ISurfacePanel* view)
{
	this->view = view;
}

void SurfacePanelPresenter::setStatusControl(StatusPresenter* s_ctrl)
{
	this->statusControl = s_ctrl;
}


void SurfacePanelPresenter::sideButtonClicked(SurfaceType surf_type)
{
	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

	statusControl->changeStatus(surface, surf_type);
}



void SurfacePanelPresenter::buttonClicked(ButtonPos position, SurfaceClick click)
{
	auto [surface, state] = surfaceState[static_cast<int>(position)];

	if (click == SurfaceClick::leftClick)
		switch (state)
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
		switch (state)
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

	std::array<std::string, 6> sateLabel;
	std::array<std::string, 6> surfaceName;
	auto unorderedSurfaces = utilities.getSurfaceNames(currentIndex);

	for (int i = 0; i<6; i++)
	{
		auto surface = matrix.getSurface(currentIndex, static_cast<ButtonPos>(i));
		surfaceName[i] = unorderedSurfaces[static_cast<int>(surface)];

		if (tooth->obturation.exists(surface) && tooth->caries.exists(surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::secondary);
			sateLabel[i] = "вторичен кариес";
		}
		else if (tooth->obturation.exists(surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::obturation);
			sateLabel[i] = "обтурация";
		}

		else if (tooth->caries.exists(surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::caries);
			sateLabel[i] = "кариес";
		}
		else
			surfaceState[i] = std::make_tuple(surface, SurfaceState::none);
	}

	view->setStatuses(sateLabel);
	view->setLabels(surfaceName);

}
