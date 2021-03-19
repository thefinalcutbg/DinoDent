#pragma once

#include "View/ListView/SurfacePanel/ISurfacePanel.h"
#include "../ListPresenter/IStatusControl.h"
#include "InputEnums.h"
#include "ButtonSurfaceMatrix.h"
#include "Model/Tooth/ToothUtils.h"

class SurfacePanelPresenter
{
	ISurfacePanel* view;
	IStatusControl* statusControl;

	int currentIndex;
	ButtonSurfaceMatrix matrix;
	
	ToothUtils utilities;

	void setViewLabels();
public:
		SurfacePanelPresenter(ISurfacePanel* view);
		void getStatusControl(IStatusControl* listPresenter);

		void buttonClicked(ButtonPos position, SurfaceType type);
		void setTooth(const Tooth* tooth);
};

