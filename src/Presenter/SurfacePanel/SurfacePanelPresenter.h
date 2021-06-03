#pragma once

#include "View/ListView/SurfacePanel/ISurfacePanel.h"
#include "../ListPresenter/StatusPresenter/IStatusControl.h"
#include "InputEnums.h"
#include "ButtonSurfaceMatrix.h"
#include "Model/Tooth/ToothUtils.h"

#include <QDebug>

enum class SurfaceClick {add, remove};
enum class SurfaceState {none, obturation, caries, secondary};

class SurfacePanelPresenter
{
	ISurfacePanel* view;
	IStatusControl* statusControl;

	std::array <SurfaceState, 5> surfaceState;

	int currentIndex;
	ButtonSurfaceMatrix matrix;
	
	ToothUtils utilities;

	void setViewLabels(const Tooth* tooth);
public:
		SurfacePanelPresenter(ISurfacePanel* view);
		void setStatusControl(IStatusControl* listPresenter);

		void buttonClicked(ButtonPos position, SurfaceType type);
		void setTooth(const Tooth* tooth);
};

