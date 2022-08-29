#pragma once

#include "Network/PIS/GetMonthNotifListService.h"
#include "Network/PIS/GetMonthNotifService.h"
#include "Model/Financial/MonthNotifRow.h"
#include <vector>
#include <optional>

class TabPresenter;
class IMonthNotifView;

class MonthNotifPresenter
{

	GetMonthNotifListService m_listHandler;
	GetMonthNotifService m_notifHandler;

	TabPresenter* tab_presenter;
	IMonthNotifView* view{ nullptr };
	
	std::vector<MonthNotifRow> m_notifRows;

public:
	MonthNotifPresenter(TabPresenter* presenter);
	
	void setView(IMonthNotifView* view);
	void loadFromXml();
	void loadFromPis();
	void okPressed(int currentIdx);
	//handles the reply from replyhandler
	void setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows);


};

