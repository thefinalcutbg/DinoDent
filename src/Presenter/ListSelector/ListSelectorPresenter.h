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

	int
	_month{ Date::currentMonth() },
	_year{ Date::currentYear() };
	
	std::vector<AmbListRow> rows_{ amb_db.getAmbListRows(_month, _year) };



public:
	ListSelectorPresenter();
	
	void setTabPresenter(TabPresenter* tabPresenter);

	void openDialog();

	void setView(IListSelectorView* view);

	void setDate(int month, int year);

	void refreshModel();

	void selectionChanged(std::vector<int> selectedIndexes);

	void openAmbList();
	void deleteAmbList();

	~ListSelectorPresenter();

};

