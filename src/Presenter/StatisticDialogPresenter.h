#pragma once

#include <optional>

#include "Model/Dental/DentalStatistics.h"

class StatisticDialogView;

class StatisticDialogPresenter
{
	StatisticDialogView* view{nullptr};

public:
	StatisticDialogPresenter();

	void setProcedureType(int type);

	void setView(StatisticDialogView* view);

	void parameterChanged();

	void openDialog();



};