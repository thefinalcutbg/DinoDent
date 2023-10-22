#pragma once

#include "Network/PIS/GetMonthNotifListService.h"
#include "Network/PIS/GetMonthNotifService.h"
#include "Model/Financial/MonthNotifRow.h"
#include <vector>
#include <optional>

class IMonthNotifView;

class MonthNotifPresenter
{

	GetMonthNotifListService m_listHandler;
	GetMonthNotifService m_notifHandler;

	IMonthNotifView* view{ nullptr };
	
	static inline std::vector<MonthNotifRow> m_notifRows;

public:
	MonthNotifPresenter();
	
	void setView(IMonthNotifView* view);
	void loadFromXml();
	void loadFromPis();
	void okPressed(int currentIdx);
	//handles the reply from replyhandler
	void setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows);

	static void clearCache() {
		m_notifRows.clear();
	}

};

