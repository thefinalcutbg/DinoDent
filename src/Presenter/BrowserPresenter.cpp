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
	
	this->view->setDates(m_from, m_to);

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
	PlainTable tableView;
	
	sentToHis.clear();

	std::tie(rowidData, tableView) = DbBrowser::getData(m_currentModelType, m_from, m_to);

	tableView.data.insert(tableView.data.begin(), PlainColumn{});

	for (int i = 0; i < rowidData.size(); i++)
	{
		//inserting additional column containing the corresponding index in rowidData
		tableView.addCell(0, { std::to_string(i) });

		//tracking the rowids of the sheets sent to his
		if(
			m_currentModelType == TabType::AmbList &&
			tableView.data[2].rows[i].icon == PlainCell::HIS
		)
		{
			sentToHis.insert(rowidData[i].rowID);
		}
	}

	m_selectedInstances.clear();

	if (view == nullptr) return;

	int id{ 0 }, name{ 0 }, phone{ 0 };

	switch (m_currentModelType) {
		case TabType::PatientSummary:
			id = 1; name = 2; phone = 3; break;
		case TabType::AmbList:
		case TabType::Prescription:
		case TabType::Financial:
			id = 3; name = 4; phone = 5; break;
		case TabType::PerioStatus:
			id = 2; name = 3; phone = 4; break;
	}


	view->setTable(tableView, id, name, phone);
	view->setDates(m_from, m_to);
	
	selectionChanged(std::set<int>());
	

}

void BrowserPresenter::setListType(TabType type)
{
	m_currentModelType = type;

	refreshModel();
}

void BrowserPresenter::selectionChanged(const std::set<int>& selectedIndexes)
{ 
	m_selectedInstances.clear();

	for (auto idx : selectedIndexes) {
		m_selectedInstances.push_back(&rowidData[idx]);
	}
	
	long long rowid = selectedIndexes.size() == 1 ? 
		m_selectedInstances[0]->rowID
		: 
		0;
	
	view->setPreview(DbBrowser::getPreview(m_currentModelType, rowid));
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

	for (auto& ptr : m_selectedInstances)
	{
		if (sentToHis.count(ptr->rowID))
		{
			ModalDialogBuilder::showMessage("Не можете да изтриете амбулаторен лист, който е отворен в НЗИС. Първо го анулирайте.");
			return;
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
