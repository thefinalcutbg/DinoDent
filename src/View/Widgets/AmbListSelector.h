#pragma once

#include <QDialog>
#include "ui_AmbListSelector.h"
#include "View/Interfaces/IListSelectorView.h"
#include "View/TableModels/AmbListTableModel.h"
#include "View/TableModels/PatientTableModel.h"
#include "View/TableModels/PerioTableModel.h"
#include "View/TableModels/FinancialTableModel.h"
#include "View/TableModels/PrescriptionTableModel.h"
#include <QSortFilterProxyModel>

class ListSelectorPresenter;

class AmbListSelector : public QDialog, public IListSelectorView
{
	Q_OBJECT
private:

	ListSelectorPresenter* presenter;

	QSortFilterProxyModel idFilter;
	QSortFilterProxyModel nameFilter;
	QSortFilterProxyModel phoneFilter;
	QMenu* main_menu;
	QMenu* sub_menu;
	AmbListTableModel amb_model;
	PatientTableModel patient_model;
	PerioTableModel perio_model;
	FinancialTableModel financial_model;
	PrescriptionTableModel perscription_model;


public:
	AmbListSelector(ListSelectorPresenter *p);

	void setDates(const Date& from, const Date& to) override;

	void setRows(const std::vector<AmbRow>& rows) override;
	void setRows(const std::vector<PatientRow>& rows) override;
	void setRows(const std::vector<PerioRow>& rows) override;
	void setRows(const std::vector<FinancialRow>& rows) override;
	void setRows(const std::vector<PrescriptionRow>& rows) override;
	~AmbListSelector();

private:
	Ui::AmbListSelector ui;

	//shows the additional buttons and changes selectionMode

	void focus() override;
	void close() override;
	void contextMenuRequested(const QPoint& p);


};
