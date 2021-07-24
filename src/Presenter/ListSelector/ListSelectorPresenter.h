#pragma once

#include "Model/AmbListRow.h"
#include "Database/DbAmbList.h"

class TabPresenter;
class IListSelectorView;

class ListSelectorPresenter
{
	TabPresenter* tab_presenter{ nullptr };
	IListSelectorView* view{ nullptr };
	DbAmbList amb_db;

	std::vector<int>selectedIndexes{};

	Date _from{ 1, Date::currentMonth(), Date::currentYear() };
	Date _to{ Date::currentDate().getMaxDateOfMonth() };
	
	std::vector<AmbListRow> rows_{ amb_db.getAmbListRows(_from, _to) };



public:
	ListSelectorPresenter();
	
	void setTabPresenter(TabPresenter* tabPresenter);

	void openDialog();

	void setView(IListSelectorView* view);

	void setDates(const Date& from, const Date& to);

	void refreshModel();

	void selectionChanged(std::vector<int> selectedIndexes);

	void openAmbList();
	void deleteAmbList();

	~ListSelectorPresenter();

};

