#pragma once
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Model/Tooth/Tooth.h"
#include <optional>

class IDetailsView;

class DetailsPresenter
{
	StatusType m_category{ StatusType::general };
	int m_code{ -1 };
	CheckModel m_checkModel;

	Tooth tooth;

	IDetailsView* view{ nullptr };
	std::optional<Tooth> _result {};

	void setDynamicStatus();
	void setDynamicDisable();

public:
	DetailsPresenter(const Tooth& tooth);


	void statusSelected(int category, int code);
	void checkStateChanged(bool checked);
	void okPressed();
	void setView(IDetailsView* view);

	std::optional<Tooth> open();

};

