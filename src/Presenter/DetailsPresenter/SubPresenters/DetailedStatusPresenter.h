#pragma once
#include "View/DetailsView/SubWidgets/IDetailedStatusView.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Model/Tooth/Tooth.h"
#include "../StatusController.h"
#include <optional>
#include <memory>

class DetailedStatusPresenter
{
	Tooth& tooth;

	StatusType m_category{ StatusType::general };
	int m_code{ -1 };

	CheckModel m_checkModel;

	std::unique_ptr<DetailedStatusController> controller;
	
	IDetailedStatusView* view{nullptr};

public:

	DetailedStatusPresenter(Tooth& tooth);
	void setView(IDetailedStatusView* view);
	void checkStateChanged(bool checked);
	void stateChanged();
	void statusSelected(int category, int code);

	void setDynamicDisable();

};

