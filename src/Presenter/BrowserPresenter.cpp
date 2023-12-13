#include "BrowserPresenter.h"
#include "View/Interfaces/IBrowserDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"
#include "Presenter/TabPresenter.h"
#include "Database/DbPrescription.h"
#include "Database/DbInvoice.h"
#include "Database/DbProcedure.h"
#include <map>

void BrowserPresenter::setView(IBrowserDialog* view)
{
	this->view = view;

	if (!view) return;

	refreshModel();
	
}

void BrowserPresenter::setDates(const Date& from, const Date& to)
{
	m_from = from;
	m_to = to;
	refreshModel();

}

void BrowserPresenter::refreshModel()
{
	
	m_ambRows = DbBrowser::getAmbRows(m_from, m_to);
	m_perioRows = DbBrowser::getPerioRows(m_from, m_to);
	m_patientRows = DbBrowser::getPatientRows();
	m_financialRows = DbBrowser::getFinancialRows(m_from, m_to);
	m_prescriptionRows = DbBrowser::getPrescriptionRows(m_from, m_to);
	
	if (view != nullptr)
	{
		setListType(m_currentModelType);
		view->setDates(m_from, m_to);
	}

}

void BrowserPresenter::setListType(TabType type)
{
	view->setPreview(PlainTable());
	m_currentModelType = type;
	m_selectedInstances.clear();

	switch (type)
	{
		case::TabType::AmbList: view->setRows(m_ambRows); break;
		case::TabType::PerioStatus: view->setRows(m_perioRows); break;
		case::TabType::PatientSummary: view->setRows(m_patientRows); break;
		case::TabType::Financial : view->setRows(m_financialRows); break;
		case::TabType::Prescription: view->setRows(m_prescriptionRows); break;
	}
}

void BrowserPresenter::selectionChanged(std::set<int> selectedIndexes)
{ 

	m_selectedInstances.clear();

	switch (m_currentModelType) {
		case::TabType::AmbList: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_ambRows[idx]); break;
		case::TabType::PerioStatus: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_perioRows[idx]); break;
		case::TabType::PatientSummary: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_patientRows[idx]); break;
		case::TabType::Financial: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_financialRows[idx]); break;
		case::TabType::Prescription: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_prescriptionRows[idx]); break;
	}

	if (selectedIndexes.size() != 1) {
		view->setPreview(PlainTable());
		return;
	}

	int idx = *selectedIndexes.begin();

	switch (m_currentModelType) {
		case TabType::AmbList: view->setPreview(DbBrowser::getPreview(TabType::AmbList, m_ambRows[idx].rowID)); break;
		case TabType::Financial: view->setPreview(DbBrowser::getPreview(TabType::Financial, m_financialRows[idx].rowID)); break;
		case TabType::Prescription: view->setPreview(PlainTable(DbPrescription::get(m_prescriptionRows[idx].rowID).medicationGroup)); break;
	}

}




void BrowserPresenter::openNewDocument(TabType type)
{
	if (m_currentModelType == TabType::Financial) return;

	for (int i = 0; i < m_selectedInstances.size(); i ++) {

		RowInstance row(type);
		row.rowID = 0;
		row.patientRowId = m_selectedInstances[i]->patientRowId;

		TabPresenter::get().open(row, i == m_selectedInstances.size() - 1);
	}

	view->close();
}


void BrowserPresenter::openCurrentSelection()
{
	if (!m_selectedInstances.size()) return;

	if (m_currentModelType == TabType::PatientSummary) {

		auto result = ModalDialogBuilder::openButtonDialog(
			{
				"Нов амбулаторен лист",
				"Нова рецепта",
				"Нова фактура",
				"Ново пародонтално измерване",
				"История на пациента"
			},
			"Отвори"
		);

		if (result == -1) return;

		static TabType arr[5]{
			TabType::AmbList,
			TabType::Prescription,
			TabType::Financial,
			TabType::PerioStatus,
			TabType::PatientSummary
		};

		openNewDocument(arr[result]);

		return;

	}


	int counter{ 0 };

	for (auto& row : m_selectedInstances) {

		bool isLastTab = ++counter == m_selectedInstances.size();

		TabPresenter::get().open(*row, isLastTab);
	}

	if (view) view->close();
}




void BrowserPresenter::deleteCurrentSelection()
{
	if (m_selectedInstances.empty()) return;

	if (m_selectedInstances[0]->type == TabType::AmbList)
	{
		for (auto& ptr : m_selectedInstances)
		{
			if (static_cast<AmbRow*>(ptr)->his)
			{
				ModalDialogBuilder::showMessage("Не можете да изтриете амбулаторен лист, който е отворен в НЗИС. Първо го анулирайте.");
				return;
			}
		}
	}

	std::string warningMsg = "Сигурни ли сте, че искате да изтриете избраният/избраните ";

	static const std::map<TabType, const char*> endString = {
		{TabType::AmbList, "амбулаторни листoве?"},
		{TabType::PerioStatus, "пародонтални измервания?"},
		{TabType::PatientSummary, "пацинети? Всичките свързани медицински докумнети ще бъдат изтрити!"},
		{TabType::Financial, "финансови документи?"},
		{TabType::Prescription, "рецепти?"}
	};

	warningMsg += endString.at(m_currentModelType);

	if (!ModalDialogBuilder::askDialog(warningMsg))
		return;

	for (auto& row : m_selectedInstances)
	{
		if (TabPresenter::get().documentTabOpened(row->type, row->rowID))
		{
			ModalDialogBuilder::showMessage
			("Първо затворете всички избрани за изтриване документи!");
			return;
		}
	}

	for (auto& row : m_selectedInstances) {
		DbBrowser::deleteRecord(row->type, row->rowID);
	}


	refreshModel();
	
}
