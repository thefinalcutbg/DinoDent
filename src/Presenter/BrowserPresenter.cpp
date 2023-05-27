#include "BrowserPresenter.h"
#include "View/Interfaces/IBrowserDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"

BrowserPresenter::BrowserPresenter()
{

}

void BrowserPresenter::openDialog()
{
	if (!view) {
		m_selectedInstances.clear();
		ModalDialogBuilder::openDialog(this);
	}
	else view->focus();
}

void BrowserPresenter::setView(IBrowserDialog* view)
{
	this->view = view;

	if (!view) return;


	view->setDates(m_from, m_to);
	view->setRows(m_ambRows);

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
	
	m_ambRows = DbListOpener::getAmbRows(m_from, m_to);
	m_perioRows = DbListOpener::getPerioRows(m_from, m_to);
	m_patientRows = DbListOpener::getPatientRows();
	m_financialRows = DbListOpener::getFinancialRows(m_from, m_to);
	m_prescriptionRows = DbListOpener::getPrescriptionRows(m_from, m_to);
	
	if (view != nullptr)
	{
		setListType(m_currentModelType);
		view->setDates(m_from, m_to);
	}
		
}

void BrowserPresenter::setListType(TabType type)
{
	m_currentModelType = type;
	m_selectedInstances.clear();

	switch (type)
	{
		case::TabType::AmbList: view->setRows(m_ambRows); break;
		case::TabType::PerioList: view->setRows(m_perioRows); break;
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
		case::TabType::PerioList: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_perioRows[idx]); break;
		case::TabType::PatientSummary: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_patientRows[idx]); break;
		case::TabType::Financial: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_financialRows[idx]); break;
		case::TabType::Prescription: for (auto idx : selectedIndexes) m_selectedInstances.push_back(&m_prescriptionRows[idx]); break;
	}
}

#include "Presenter/TabPresenter.h"

void BrowserPresenter::setTabPresenter(TabPresenter* tabPresenter)
{
	this->tab_presenter = tabPresenter;
}



void BrowserPresenter::openNewDocument(TabType type)
{
	if (m_currentModelType == TabType::Financial) return;

	for (int i = 0; i < m_selectedInstances.size(); i ++) {

	

		RowInstance row(type);
		row.rowID = 0;
		row.patientRowId = m_selectedInstances[i]->patientRowId;

		tab_presenter->open(row, i == m_selectedInstances.size() - 1);
	}

	view->close();
}


void BrowserPresenter::openCurrentSelection()
{
	if (!m_selectedInstances.size()) return;

	int counter{ 0 };

	for (auto& row : m_selectedInstances) {

		bool isLastTab = ++counter == m_selectedInstances.size();

		tab_presenter->open(*row, isLastTab);
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
				ModalDialogBuilder::showMessage("Някои от избраните амбулаторни листи не са анулирани в НЗИС и не могат да бъдат изтрити");
				return;
			}
		}
	}

	std::string warningMsg = "Сигурни ли сте, че искате да изтриете избраният/избраните ";

	static constexpr const char* endString[5]{
		"амбулаторни листи?",
		"пародонтални измервания?",
		"пацинети? Всичките свързани медицински докумнети ще бъдат изтрити!",
		"финансови документи?",
		"рецепти?"
	};

	warningMsg += endString[static_cast<int>(m_currentModelType)];

	if (!ModalDialogBuilder::askDialog(warningMsg))
		return;

	for (auto& row : m_selectedInstances)
	{
		if (tab_presenter->documentTabOpened(row->type, row->rowID))
		{
			ModalDialogBuilder::showMessage
			("Първо затворете всички избрани за изтриване документи!");
			return;
		}
	}

	for (auto& row : m_selectedInstances) {
		DbListOpener::deleteRecord(row->type, row->rowID);
	}


	refreshModel();
	
}

BrowserPresenter::~BrowserPresenter()
{
}

