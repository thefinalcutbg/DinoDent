#include "SurfacePanelPresenter.h"
#include "Model/Dental/ToothUtils.h"
#include "Presenter/ListPresenter.h"
#include "View/Graphics/PaintHint.h"

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
	{
		switch (state)
		{
		case SurfaceState::none:
			statusControl->setToothStatus(StatusType::obturation, surface);
			break;
		case SurfaceState::obturation:
			statusControl->setToothStatus(StatusType::caries, surface);
			statusControl->setToothStatus(StatusType::obturation, surface);
			break;
		case SurfaceState::caries:
			statusControl->setToothStatus(StatusType::obturation, surface);
			break;
		case SurfaceState::secondary:
			statusControl->setToothStatus(StatusType::caries, surface);
			break;
		}
	}
	else
	{
		switch (state)
		{
		case::SurfaceState::obturation:
			statusControl->setToothStatus(StatusType::obturation, surface);
			break;
		case::SurfaceState::caries:
			statusControl->setToothStatus(StatusType::caries, surface);
			break;
		case::SurfaceState::secondary:
			statusControl->setToothStatus(StatusType::obturation, surface);
			statusControl->setToothStatus(StatusType::caries, surface);
			break;
		case SurfaceState::none:
			break;
		};
	}

}

void SurfacePanelPresenter::sideCariesClicked() {	
//statusControl->setToothStatus(StatusType::caries, (matrix.getSurface(currentIndex, ButtonPos::side)));
}

void SurfacePanelPresenter::sideObturationClicked(){
//statusControl->setToothStatus(StatusType::obturation, (matrix.getSurface(currentIndex, ButtonPos::side)));
}

void SurfacePanelPresenter::setTooth(const Tooth& tooth)
{
	currentIndex = tooth.index;

	view->paintTooth(ToothPaintHint(tooth));
/*	
	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

	view->setSideButtonsClicked(
		tooth.obturation.exists(static_cast<int>(surface)),
		tooth.caries.exists(static_cast<int>(surface))
	);
*/
	std::array<std::string, 10> label;
	auto unorderedSurfaces = ToothUtils::getSurfaceNames(currentIndex);

	for (int i = 0; i<10; i++)
	{
		auto surface = matrix.getSurface(currentIndex, static_cast<ButtonPos>(i));
		label[i] = unorderedSurfaces[static_cast<int>(surface)];

		if (tooth.obturation.exists(surface) && tooth.caries.exists(surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::secondary);
			label[i] += ": вторичен кариес";
		}
		else if (tooth.obturation.exists(surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::obturation);
			label[i] += ": обтурация";
		}

		else if (tooth.caries.exists(surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::caries);
			label[i] += ": кариес";
		}
		else
			surfaceState[i] = std::make_tuple(surface, SurfaceState::none);
	}

	view->setLabels(label);

}
