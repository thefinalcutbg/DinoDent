#pragma once
#include "Network/ReplyHandlers/NotifListHandler.h"
#include "Network/ReplyHandlers/MonthNotifDataHandler.h"
#include "Model/Financial/MonthNotifRow.h"
#include <vector>
#include <optional>

class TabPresenter;

class MonthNotifLoader
{

	NotifListHandler m_listHandler;
	MonthNotifDataHandler m_dataHandler;

	TabPresenter* presenter;

	bool m_awaitingReply{ false };
	
	std::vector<MonthNotifRow> m_notifRows;

public:
	MonthNotifLoader(TabPresenter* presenter);
	void loadNotification();
	//handles the reply from replyhandler
	void setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows);
	void setMonthNotif(const std::string& monthNotif);

};

