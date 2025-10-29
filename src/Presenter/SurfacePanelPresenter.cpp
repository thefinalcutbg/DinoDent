#include "SurfacePanelPresenter.h"

#include "Model/Dental/ToothUtils.h"

#include "Presenter/ListPresenter.h"

#include "View/Graphics/PaintHint.h"
#include "View/SubWidgets/SurfacePanel.h"

using namespace Dental;

SurfacePanelPresenter::SurfacePanelPresenter() : view(nullptr), statusControl(nullptr), currentIndex(-1)
{
}

void SurfacePanelPresenter::notesClicked()
{
	statusControl->openDetails();
}

void SurfacePanelPresenter::setView(SurfacePanel* view)
{
	this->view = view;
}

void SurfacePanelPresenter::setStatusControl(ListPresenter* s_ctrl)
{
	this->statusControl = s_ctrl;
}


void SurfacePanelPresenter::buttonClicked(ButtonPos position, SurfaceClick click)
{
	auto& [surface, state] = surfaceState[static_cast<int>(position)];

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
			statusControl->setToothStatus(StatusType::Restoration, surface);
			statusControl->setToothStatus(StatusType::DefectiveRestoration, surface);
			break;
		case SurfaceState::defective:
			statusControl->setToothStatus(StatusType::DefectiveRestoration, surface);
			statusControl->setToothStatus(StatusType::NonCariesLesion, surface);
			break;
		case SurfaceState::nonCaries:
			statusControl->setToothStatus(StatusType::NonCariesLesion, surface);
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
		case SurfaceState::defective:
			statusControl->setToothStatus(StatusType::DefectiveRestoration, surface);
			break;
		case SurfaceState::nonCaries:
			statusControl->setToothStatus(StatusType::NonCariesLesion, surface);
			break;
		case SurfaceState::none:
			break;
		};
	}

}

void SurfacePanelPresenter::sideButtonClicked(Dental::StatusType stat)
{
	statusControl->setToothStatus(stat, (matrix.getSurface(currentIndex, ButtonPos::side)));
}

void SurfacePanelPresenter::setTooth(const Tooth& tooth, bool hasNotes)
{
	currentIndex = tooth.index();

	view->paintTooth(ToothPaintHint(tooth), hasNotes);
	auto surface = matrix.getSurface(currentIndex, ButtonPos::side);

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
			stateLabel[i] = "Вторичен кариес";
		}
		else if (tooth.hasStatus(Dental::Restoration, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::restoration);
			stateLabel[i] = "Възстановяване";
		}
		else if (tooth.hasStatus(Dental::Caries, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::caries);
			stateLabel[i] = "Кариес";
		}
		else if (tooth.hasStatus(Dental::DefectiveRestoration, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::defective);
            stateLabel[i] = "Дефектно възстанов.";
		}
		else if (tooth.hasStatus(Dental::NonCariesLesion, surface))
		{
			surfaceState[i] = std::make_tuple(surface, SurfaceState::nonCaries);
			stateLabel[i] = "Некариозна лезия";
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
