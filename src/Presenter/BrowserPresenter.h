#pragma once

#include "Model/TableRows.h"
#include "Model/PlainTable.h"
#include <set>

class IBrowserDialog;

struct BrowserUiState {
	Date from{ 1, Date::currentMonth(), Date::currentYear() };
	Date to{ Date::currentDate().getMaxDateOfMonth() };
	TabType model_type{ TabType::PatientSummary };
	bool showDetails{ false };
	bool showProcedures{ false };
};

class BrowserPresenter
{
	IBrowserDialog* view{ nullptr };

	static inline BrowserUiState ui_state;

	std::vector<RowInstance> rowidData;
	std::vector<RowInstance> patientDocRowid;

	std::unordered_set<long long> sentToHis;
	PlainTable tableData;

	std::vector<RowInstance*> m_selectedInstances;

	void refreshModel();
	void refreshPreview();

public:

	void setView(IBrowserDialog* view);

	void setDates(const Date& from, const Date& to);

	void showDetailsPane(bool show);
	void showProcedureDetails(bool show);

	void setListType(TabType type);
	void selectionChanged(const std::set<int>& selectedIndexes);
	void openNewDocument(TabType type);
	void openCurrentSelection();
	void deleteCurrentSelection();
	void openPatientDocuments(const std::set<int>& selectedIndexes);

};

