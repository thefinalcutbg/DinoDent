#pragma once

#include <QDialog>
#include "ui_AmbListSelector.h"
#include "IListSelectorView.h"
#include "Models/AmbListTableModel.h"
#include "Models/PatientTableModel.h"
#include "Models/PerioTableModel.h"

#include <QSortFilterProxyModel>

class ListSelectorPresenter;

class AmbListSelector : public QDialog, public IListSelectorView
{
	Q_OBJECT
private:

	ListSelectorPresenter* p;

	QSortFilterProxyModel idFilter;
	QSortFilterProxyModel nameFilter;
	QSortFilterProxyModel phoneFilter;

	AmbListTableModel amb_model;
	PatientTableModel patient_model;
	PerioTableModel perio_model;

public:
	AmbListSelector(ListSelectorPresenter *p);
	~AmbListSelector();

private:
	Ui::AmbListSelector ui;

	void setDates(const Date& from, const Date& to) override;

	void setRows(const std::vector<AmbRow>& rows) override;
	void setRows(const std::vector<PatientRow>& rows) override;
    void setRows(const std::vector<PerioRow>& rows) override;

	void focus() override;
	void close() override;

};
