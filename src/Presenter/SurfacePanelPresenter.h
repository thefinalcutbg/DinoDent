#pragma once

#include "View/Interfaces/ISurfacePanel.h"

#include "InputEnums.h"
#include "ButtonSurfaceMatrix.h"

class Tooth;

enum class SurfaceClick {rightClick, leftClick};
enum class SurfaceState { none, restoration, caries, secondary };

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
		void setTooth(const Tooth& tooth);
		void buttonClicked(ButtonPos position, SurfaceClick click);
		void sideCariesClicked();
		void sideRestorationClicked();

};

