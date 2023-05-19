#pragma once

#include <optional>

#include "Model/Dental/DentalStatistics.h"
#include "View/Interfaces/IStatisticDialogView.h"

class StatisticDialogPresenter
{

	IStatisticDialogView* view{nullptr};

public:
	StatisticDialogPresenter();

	void setProcedureType(int type);

	void setView(IStatisticDialogView* view);

	void parameterChanged();

	void openDialog();



};