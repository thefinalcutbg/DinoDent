#pragma once

#include "Model/TableRows.h"
#include "Database/DbListOpener.h"
#include <set>
class TabPresenter;
class IListSelectorView;

class ListSelectorPresenter
{
	TabPresenter* tab_presenter{ nullptr };
	IListSelectorView* view{ nullptr };

	TabType m_currentModelType { TabType::PatientSummary};

	Date m_from{ 1, Date::currentMonth(), Date::currentYear() };
	Date m_to{ Date::currentDate().getMaxDateOfMonth() };
	
	std::vector<AmbRow> m_ambRows;
	std::vector<PatientRow> m_patientRows;
	std::vector<PerioRow> m_perioRows;
	std::vector<FinancialRow> m_financialRows;
	std::vector<PrescriptionRow> m_perscriptionRows;

	std::vector<RowInstance*> m_selectedInstances;

public:
	ListSelectorPresenter();
	
	void setTabPresenter(TabPresenter* tabPresenter);

	void openDialog();

	void setView(IListSelectorView* view);

	void setDates(const Date& from, const Date& to);

	void refreshModel();

	void setListType(TabType type);

	void selectionChanged(std::set<int> selectedIndexes);
	void openNewDocument(TabType type);
	void openCurrentSelection();
	void deleteCurrentSelection();
	~ListSelectorPresenter();

};

