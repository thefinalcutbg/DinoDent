#pragma once

#include "Model/TableRows.h"
#include "Model/PlainTable.h"
#include "Database/DbBrowser.h"
#include <set>

class IBrowserDialog;

class BrowserPresenter
{
	IBrowserDialog* view{ nullptr };

	std::unordered_set<long long> sentToHis;

	static inline TabType m_currentModelType { TabType::PatientSummary};

	static inline Date m_from{ 1, Date::currentMonth(), Date::currentYear() };
	static inline Date m_to{ Date::currentDate().getMaxDateOfMonth() };
	
	std::vector<RowInstance> rowidData;
	PlainTable tableData;

	std::vector<RowInstance*> m_selectedInstances;

public:

	void setView(IBrowserDialog* view);

	void setDates(const Date& from, const Date& to);

	void refreshModel();
	void setListType(TabType type);
	void selectionChanged(const std::set<int>& selectedIndexes);
	void openNewDocument(TabType type);
	void openCurrentSelection();
	void deleteCurrentSelection();


};

