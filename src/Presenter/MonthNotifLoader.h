#pragma once

#include "Network/NetService/GetMonthNotifListService.h"
#include "Network/NetService/GetMonthNotifService.h"
#include "Model/Financial/MonthNotifRow.h"
#include <vector>
#include <optional>

class TabPresenter;

class MonthNotifLoader
{

	GetMonthNotifListService m_listHandler;
	GetMonthNotifService m_notifHandler;

	TabPresenter* presenter;

	//bool m_awaitingReply{ false };
	
	std::vector<MonthNotifRow> m_notifRows;

public:
	MonthNotifLoader(TabPresenter* presenter);
	void loadNotification();
	//handles the reply from replyhandler
	void setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows);
	void setMonthNotif(const std::string& monthNotif);

};

