﻿#include "SurfacePanelPresenter.h"
#include "Model/Dental/ToothUtils.h"
#include "Presenter/ListPresenter.h"

SurfacePanelPresenter::SurfacePanelPresenter() : statusControl(nullptr), view(nullptr), currentIndex(-1)
{
}

void SurfacePanelPresenter::setView(ISurfacePanel* view)
{
	this->view = view;
}

void SurfacePanelPresenter::setStatusControl(ListPresenter* s_ctrl)
{
	this->statusControl = s_ctrl;
}


void SurfacePanelPresenter::buttonClicked(ButtonPos position, SurfaceClick click)
{
	auto [surface, state] = surfaceState[static_cast<int>(position)];

	if (click == SurfaceClick::leftClick)
		switch (state)
		{
		case SurfaceState::none:
			statusControl->setObturation(surface);
			break;
		case SurfaceState::obturation:
			statusControl->setCaries(surface);
			statusControl->setObturation(surface);
			break;
		case SurfaceState::caries:
			statusControl->setObturation(surface);
			break;
		case SurfaceState::secondary:
			statusControl->setCaries(surface);
			break;
		}
	else
		switch (state)
		{ 
		case::SurfaceState::obturation:
			statusControl->setObturation(surface);
			break;
		case::SurfaceState::caries:
			statusControl->setCaries(surface);
			break;
		case::SurfaceState::secondary:
			statusControl->setObturation(surface);
			statusControl->setCaries(surface);
			break;
		case SurfaceState::none:
			break;
		};

}

void SurfacePanelPresenter::sideCariesClicked() {	
statusControl->setCaries(matrix.getSurface(currentIndex, ButtonPos::side));
}

void SurfacePanelPresenter::sideObturationClicked(){
statusControl->setObturation(matrix.getSurface(currentIndex, ButtonPos::side));
}

void SurfacePanelPresenter::setTooth(Tooth* tooth)
{
	currentIndex = tooth->index;

	view->paintTooth(ToothHintCreator::getToothHint(*tooth));
	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

	view->setSideButtonsClicked(
		tooth->obturation.exists(static_cast<int>(surface)),
		tooth->caries.exists(static_cast<int>(surface))
	);

	std::array<std::string, 6> sateLabel;
	std::array<std::string, 6> surfaceName;
	auto unorderedSurfaces = ToothUtils::getSurfaceNames(currentIndex);

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