#pragma once

#include <QDialog>
#include "ui_AmbListSelector.h"
#include "IListSelectorView.h"
#include "ListsTableModel.h"

#include <QSortFilterProxyModel>

class ListSelectorPresenter;

class AmbListSelector : public QDialog, public IListSelectorView
{
	Q_OBJECT
private:

	ListSelectorPresenter* p;

	QSortFilterProxyModel idFilter;
	QSortFilterProxyModel nameFilter;
	ListsTableModel model;

public:
	AmbListSelector(ListSelectorPresenter *p);
	~AmbListSelector();

private:
	Ui::AmbListSelector ui;

	// Inherited via IListSelectorView
	virtual void setDates(const Date& from, const Date& to) override;
	virtual void setRows(const std::vector<AmbListRow>& rows) override;
	void focus() override;
	void close() override;

};