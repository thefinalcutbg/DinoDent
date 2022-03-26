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

void ListSelectorPresenter::setListType(RowModelType type)
{
	m_currentModelType = type;
	selectedIndexes.clear();
	switch (type)
	{
		case::RowModelType::AmbListRow: view->setRows(m_ambRows); break;
		case::RowModelType::PerioRow: view->setRows(m_perioRows); break;
		case::RowModelType::PatientRow: view->setRows(m_patientRows); break;
		case::RowModelType::FinancialRow : view->setRows(m_financialRows); break;
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
	case RowModelType::AmbListRow:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_ambRows[idx]);
		break;
	case RowModelType::PerioRow:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_perioRows[idx]);
		break;
	case RowModelType::PatientRow:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_patientRows[idx]);
		break;
	case RowModelType::FinancialRow:
		for (auto idx : selectedIndexes)
			tab_presenter->open(m_financialRows[idx]);
		break;
	}


	if (view) view->close();
}

void ListSelectorPresenter::deleteCurrentSelection()
{
	switch (m_currentModelType)
	{
	case RowModelType::AmbListRow:

					if (!ModalDialogBuilder::askDialog(
						u8"Сигурни ли сте, че искате да изтриете амбулаторния лист?"
					))
						return;

		for (auto idx : selectedIndexes)
		{
			tab_presenter->removeTab(m_ambRows[idx].type, m_ambRows[idx].rowID);
			DbListOpener::deleteRecord("amblist", m_ambRows[idx].rowID);
		}
		break;

	case RowModelType::PerioRow:

					if (!ModalDialogBuilder::askDialog(
						u8"Сигурни ли сте, че искате да изтриете пародонталното измерване?"
					))
						return;

		for (auto idx : selectedIndexes)
		{
			DbListOpener::deleteRecord("periostatus", m_perioRows[idx].rowID);
			tab_presenter->removeTab(m_perioRows[idx].type, m_perioRows[idx].rowID);
		}
		break;

	case RowModelType::PatientRow:

					if (!ModalDialogBuilder::askDialog(
						u8"Сигурни ли сте, че искате да изтриете този пациент? "
						"Всички негови амбулаторни листи и пародонтални измервания ще бъдат премахнати!"
					))
			return;

		for (auto idx : selectedIndexes)
		{
			DbListOpener::deleteRecord("patient", m_patientRows[idx].rowID);
			tab_presenter->removePatientTabs(m_patientRows[idx].rowID);
		}
		break;

	case RowModelType::FinancialRow:

					if (!ModalDialogBuilder::askDialog(
						u8"Сигурни ли сте, че искате да изтриете финансовият документ?"
					))
						return;

		for (auto idx : selectedIndexes)
		{
			DbListOpener::deleteRecord("financial", m_financialRows[idx].rowID);
			tab_presenter->removeTab(m_financialRows[idx].type, m_financialRows[idx].rowID);
		}
		break;
	}

		refreshModel();
	
}

ListSelectorPresenter::~ListSelectorPresenter()
{
}

