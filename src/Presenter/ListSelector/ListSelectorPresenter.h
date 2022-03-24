#pragma once

#include "Model/TableRows.h"
#include "Database/DbListOpener.h"
#include <set>
class TabPresenter;
class IListSelectorView;

enum class RowModelType{ AmbListRow, PerioRow, PatientRow};

class ListSelectorPresenter
{
	TabPresenter* tab_presenter{ nullptr };
	IListSelectorView* view{ nullptr };

	RowModelType m_currentModelType {RowModelType::AmbListRow};

	std::set<int>selectedIndexes{};

	Date m_from{ 1, Date::currentMonth(), Date::currentYear() };
	Date m_to{ Date::currentDate().getMaxDateOfMonth() };
	
	std::vector<AmbRow> m_ambRows{ DbListOpener::getAmbRows(m_from, m_to) };
	std::vector<PatientRow> m_patientRows{ DbListOpener::getPatientRows() };
	std::vector<PerioRow> m_perioRows{ DbListOpener::getPerioRows(m_from, m_to) };

public:
	ListSelectorPresenter();
	
	void setTabPresenter(TabPresenter* tabPresenter);

	void openDialog();

	void setView(IListSelectorView* view);

	void setDates(const Date& from, const Date& to);

	void refreshModel();

	void setListType(RowModelType type);

	void selectionChanged(std::set<int> selectedIndexes);

	void openCurrentSelection();
	void deleteCurrentSelection();

	~ListSelectorPresenter();

};

