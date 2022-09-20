#pragma once

#include "View/Interfaces/ISurfacePanel.h"

#include "InputEnums.h"
#include "ButtonSurfaceMatrix.h"
#include "Presenter/ToothHintCreator.h"

enum class SurfaceClick {rightClick, leftClick};
enum class SurfaceState { none, obturation, caries, secondary };

class ListPresenter;

class SurfacePanelPresenter
{
	ISurfacePanel* view;
	ListPresenter* statusControl;
	std::array <std::tuple<int, SurfaceState>, 6> surfaceState;

	int currentIndex;
	ButtonSurfaceMatrix matrix;

public:
		SurfacePanelPresenter();

		void setView(ISurfacePanel* view);
		void setStatusControl(ListPresenter* status_presenter);
		void setTooth(Tooth* tooth);
		void buttonClicked(ButtonPos position, SurfaceClick click);
		void sideCariesClicked();
		void sideObturationClicked();

};

