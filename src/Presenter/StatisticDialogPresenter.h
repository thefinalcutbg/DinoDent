#pragma once

#include <optional>

#include "Model/Dental/DentalStatistics.h"
#include "View/Interfaces/IStatisticDialogView.h"

class StatisticDialogPresenter
{

	std::optional<DentalStatistic> m_statistic;

	IStatisticDialogView* view{nullptr};

public:
	StatisticDialogPresenter(const DentalStatistic& s);
	StatisticDialogPresenter();

	void setProcedureType(int type);

	void setView(IStatisticDialogView* view);

	void okPressed();

	std::optional<DentalStatistic> openDialog();



};