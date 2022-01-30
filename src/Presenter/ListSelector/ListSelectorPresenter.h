#pragma once

#include "Model/TableRows.h"
#include "Database/DbListOpener.h"

class TabPresenter;
class IListSelectorView;

enum class RowModelType{ AmbListRow, PerioRow, PatientRow};

class ListSelectorPresenter
{
	TabPresenter* tab_presenter{ nullptr };
	IListSelectorView* view{ nullptr };
	DbListOpener m_db;

	RowModelType m_currentModelType {RowModelType::AmbListRow};

	std::vector<int>selectedIndexes{};

	Date m_from{ 1, Date::currentMonth(), Date::currentYear() };
	Date m_to{ Date::currentDate().getMaxDateOfMonth() };
	
	std::vector<AmbRow> m_ambRows{ m_db.getAmbRows(m_from, m_to) };
	std::vector<PatientRow> m_patientRows{ m_db.getPatientRows() };
	std::vector<PerioRow> m_perioRows{ m_db.getPerioRows(m_from, m_to) };





public:
	ListSelectorPresenter();
	
	void setTabPresenter(TabPresenter* tabPresenter);

	void openDialog();

	void setView(IListSelectorView* view);

	void setDates(const Date& from, const Date& to);

	void refreshModel();

	void setListType(RowModelType type);

	void selectionChanged(std::vector<int> selectedIndexes);

	void openCurrentSelection();
	void deleteCurrentSelection();

	~ListSelectorPresenter();

};

