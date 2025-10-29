#pragma once

#include "InputEnums.h"
#include "ButtonSurfaceMatrix.h"
#include "Model/Dental/Dental.h"

class Tooth;

enum class SurfaceClick {rightClick, leftClick};
enum class SurfaceState { none, restoration, caries, secondary, defective, nonCaries };

class ListPresenter;
class SurfacePanel;

class SurfacePanelPresenter
{
	SurfacePanel* view;
	ListPresenter* statusControl;
	std::array <std::tuple<int, SurfaceState>, 6> surfaceState;

	int currentIndex;
	ButtonSurfaceMatrix matrix;

public:
	SurfacePanelPresenter();
	void notesClicked();
	void setView(SurfacePanel* view);
	void setStatusControl(ListPresenter* status_presenter);
	void setTooth(const Tooth& tooth, bool hasNotes);
	void buttonClicked(ButtonPos position, SurfaceClick click);
	void sideButtonClicked(Dental::StatusType stat);
};

