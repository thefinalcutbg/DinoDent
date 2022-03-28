#include "ListSelectorPresenter.h"
#include "View/AmbListSelector/IListSelectorView.h"
#include "View/ModalDialogBuilder.h"
#include <QDebug>
ListSelectorPresenter::ListSelectorPresenter()
{

}

void ListSelectorPresenter::openDialog()
{
	if (!view) {
		selectedIndexes.clear();
		ModalDialogBuilder::openDialog(this);
	}
	else view->focus();
}

void ListSelectorPresenter::setView(IListSelectorView* view)
{
	this->view = view;

	if (!view) return;


	view->setDates(m_from, m_to);
	view->setRows(m_ambRows);

	refreshModel();
	
}

void ListSelectorPresenter::setDates(const Date& from, const Date& to)
{
	m_from = from;
	m_to = to;
	refreshModel();
}



void ListSelectorPresenter::refreshModel()
{
	
	m_ambRows = DbListOpener::getAmbRows(m_from, m_to);
	m_perioRows = DbListOpener::getPerioRows(m_from, m_to);
	m_patientRows = DbListOpener::getPatientRows();
	m_financialRows = DbListOpener::getFinancialRows(m_from, m_to);
	
	if (view != nullptr)
	{
		setListType(m_currentModelType);
		view->setDates(m_from, m_to);
	}
		
}

void ListSelectorPresenter::setListType(TabType type)
{
	m_currentModelType = type;
	selectedIndexes.clear();
	switch (type)
	{
	case::TabType::AmbList: view->setRows(m_ambRows); break;
	case::TabType::PerioList: view->setRows(m_perioRows); break;
	case::TabType::PatientSummary: view->setRows(m_patientRows); break;
	case::TabType::Financial : view->setRows(m_financialRows); break;
	}
}

void ListSelectorPresenter::selectionChanged(std::set<int> selectedIndexes)
{ 
	this->selectedIndexes = selectedIndexes;
}


#include "../TabPresenter/TabPresenter.h"

void ListSelectorPresenter::setTabPresenter(TabPresenter* tabPresenter)
{
	this->tab_presenter = tabPresenter;
}

void ListSelectorPresenter::openCurrentSelection()
{
	if (!selectedIndexes.size()) return;

	switch (m_currentModelType)
	{
	case TabType::AmbList:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_ambRows[idx]);
		break;
	case TabType::PerioList:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_perioRows[idx]);
		break;
	case TabType::PatientSummary:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_patientRows[idx]);
		break;
	case TabType::Financial:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_financialRows[idx]);
		break;
	}


	if (view) view->close();
}

//a free template function to "fix" the lack of decent polymorphism
template<typename T>
void deleteDocuments(	TabPresenter* tabPresenter, 
						const std::set<int>& selectedIndexes, 
						const std::vector<T>&rows, 
						const std::string& warningMsg
						) 
{

	if (!ModalDialogBuilder::askDialog(warningMsg))
		return;
	
	for (auto idx : selectedIndexes)
	{
		if (tabPresenter->documentTabOpened(rows[idx].type, rows[idx].rowID))
		{
			ModalDialogBuilder::showMessage
			(u8"Моля, първо затворете всички избрани за изтриване документи!");
			return;
		}
	}

	for (auto idx : selectedIndexes) {
		DbListOpener::deleteRecord(rows[idx].type, rows[idx].rowID);
	}


}

void ListSelectorPresenter::deleteCurrentSelection()
{
	

	std::string warningMsg = u8"Сигурни ли сте, че искате да изтриете избраният/избраните ";

	static constexpr const char* endString[4]{
		u8"амбулаторни листи?",
		u8"пародонтални измервания?",
		u8"пацинети? Всички свързани прилежащи медицински докумнети ще бъдат изтрити!",
		u8"финансови документи?"
	};

	warningMsg += endString[static_cast<int>(m_currentModelType)];

	switch (m_currentModelType)
	{
		case TabType::AmbList:
			deleteDocuments(tab_presenter, selectedIndexes, m_ambRows, warningMsg);
			break;
		case TabType::PerioList:
			deleteDocuments(tab_presenter, selectedIndexes, m_perioRows, warningMsg);
			break;
		case TabType::Financial:
			deleteDocuments(tab_presenter, selectedIndexes, m_financialRows, warningMsg);
			break;
		case TabType::PatientSummary:
			deleteDocuments(tab_presenter, selectedIndexes, m_patientRows, warningMsg);
			break;
	}


		refreshModel();
	
}

ListSelectorPresenter::~ListSelectorPresenter()
{
}

