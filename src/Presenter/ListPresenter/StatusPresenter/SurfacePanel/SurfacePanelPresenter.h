#pragma once

#include "View/ListView/SurfacePanel/ISurfacePanel.h"

#include "InputEnums.h"
#include "ButtonSurfaceMatrix.h"
#include "Model/Tooth/ToothUtils.h"
#include "Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h"
#include "../IStatusControl.h"
#include <QDebug>

enum class SurfaceClick {rightClick, leftClick};
enum class SurfaceState { none, obturation, caries, secondary };

class IStatusControl;

class SurfacePanelPresenter
{
	ISurfacePanel* view;
	IStatusControl* statusControl;
	ToothHintCreator paint_hint_generator;
	std::array <SurfaceState, 5> surfaceState;

	int currentIndex;
	ButtonSurfaceMatrix matrix;
	ToothUtils utilities;

	void setViewLabels(const Tooth* tooth);
public:
		SurfacePanelPresenter();

		void setView(ISurfacePanel* view);

		void setStatusControl(IStatusControl* s_ctrl);

		void setTooth(Tooth* tooth);

		void buttonClicked(ButtonPos position, SurfaceClick click);

		void sideButtonClicked(SurfaceType surf_type);

};

