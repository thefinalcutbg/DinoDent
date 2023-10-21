#pragma once

#include <QDialog>
#include "ui_BrowserDialog.h"
#include "View/Interfaces/IBrowserdialog.h"
#include "View/TableModels/AmbListTableModel.h"
#include "View/TableModels/PatientTableModel.h"
#include "View/TableModels/PerioTableModel.h"
#include "View/TableModels/FinancialTableModel.h"
#include "View/TableModels/PrescriptionTableModel.h"
#include <QSortFilterProxyModel>
#include "Presenter/BrowserPresenter.h"

class BrowserDialog : public QDialog, public IBrowserDialog
{
	Q_OBJECT
private:

	BrowserPresenter presenter;

	QSortFilterProxyModel idFilter;
	QSortFilterProxyModel nameFilter;
	QSortFilterProxyModel phoneFilter;
	QMenu* main_menu{ nullptr };
	AmbListTableModel amb_model;
	PatientTableModel patient_model;
	PerioTableModel perio_model;
	FinancialTableModel financial_model;
	PrescriptionTableModel prescription_model;

	void hideRanges(bool hidden);
	void setCountLabel();

public:
	BrowserDialog();

	void setDates(const Date& from, const Date& to) override;

	void setRows(const std::vector<AmbRow>& rows) override;
	void setRows(const std::vector<PatientRow>& rows) override;
	void setRows(const std::vector<PerioRow>& rows) override;
	void setRows(const std::vector<FinancialRow>& rows) override;
	void setRows(const std::vector<PrescriptionRow>& rows) override;
	~BrowserDialog();

private:
	Ui::BrowserDialog ui;

	//shows the additional buttons and changes selectionMode

	void focus() override;
	void close() override;
	void contextMenuRequested(const QPoint& p);


};
