#include "SurfacePanelPresenter.h"
#include "Model/Dental/ToothUtils.h"
#include "Presenter/ListPresenter.h"
#include "View/Graphics/PaintHint.h"

using namespace Dental;

SurfacePanelPresenter::SurfacePanelPresenter() : view(nullptr), statusControl(nullptr), currentIndex(-1)
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
			statusControl->setToothStatus(StatusType::Restoration, surface);
			break;
		case SurfaceState::restoration:
			statusControl->setToothStatus(StatusType::Caries, surface);
			statusControl->setToothStatus(StatusType::Restoration, surface);
			break;
		case SurfaceState::caries:
			statusControl->setToothStatus(StatusType::Restoration, surface);
			break;
		case SurfaceState::secondary:
			statusControl->setToothStatus(StatusType::Caries, surface);
			break;
		}
	}
	else
	{
		switch (state)
		{
		case::SurfaceState::restoration:
			statusControl->setToothStatus(StatusType::Restoration, surface);
			break;
		case::SurfaceState::caries:
			statusControl->setToothStatus(StatusType::Caries, surface);
			break;
		case::SurfaceState::secondary:
			statusControl->setToothStatus(StatusType::Restoration, surface);
			statusControl->setToothStatus(StatusType::Caries, surface);
			break;
		case SurfaceState::none:
			break;
		};
	}

}

void SurfacePanelPresenter::sideCariesClicked() {	
statusControl->setToothStatus(StatusType::Caries, (matrix.getSurface(currentIndex, ButtonPos::side)));
}

void SurfacePanelPresenter::sideRestorationClicked(){
statusControl->setToothStatus(StatusType::Restoration, (matrix.getSurface(currentIndex, ButtonPos::side)));
}

void SurfacePanelPresenter::setTooth(const Tooth& tooth)
{
	currentIndex = tooth.index();

	view->paintTooth(ToothPaintHint(tooth));
	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

	view->setSideButtonsClicked(
		tooth.hasStatus(Dental::Restoration, static_cast<int>(surface)),
		tooth.hasStatus(Dental::Caries, static_cast<int>(surface))
	);

	std::array<std::string, 6> stateLabel;
	std::array<std::string, 6> surfaceName;
	auto unorderedSurfaces = ToothUtils::getSurfaceNames(currentIndex);

	for (int i = 0; i<6; i++)
	{
		auto surface = matrix.getSurface(currentIndex, static_cast<ButtonPos>(i));
		surfaceName[i] = unorderedSurfaces[static_cast<int>(surface)];

		if (tooth.hasStatus(Dental::Restoration, surface) && tooth.hasStatus(Dental::Caries, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::secondary);
			stateLabel[i] = "Дефектно възстановяване";
		}
		else if (tooth.hasStatus(Dental::Restoration, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::restoration);
			stateLabel[i] = "Възстановяване";
		}

		else if (tooth.hasStatus(Dental::Caries, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::caries);
			stateLabel[i] = "Патология на ТЗТ (кариес или др.)";
		}
		else 
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::none);
			stateLabel[i] = "Интактна повърхност";
		}
	}

	view->setStatuses(stateLabel);
	view->setLabels(surfaceName);

}
