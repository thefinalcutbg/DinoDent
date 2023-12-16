#pragma once

#include <QDialog>
#include "ui_BrowserDialog.h"
#include "View/Interfaces/IBrowserdialog.h"
#include "View/TableModels/PlainTableModel.h"
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

	PlainTableModel table_model;
	PlainTableModel preview_model;

	void setCountLabel();
	void calculateUiState();

public:
	BrowserDialog();

	void setUiState(const BrowserUiState& state) override;

	void setTable(const PlainTable& t, int idColumn, int nameColumn, int phoneColumn) override;
	void setPreview(const PlainTable& t) override;

	~BrowserDialog();

private:
	Ui::BrowserDialog ui;

	//shows the additional buttons and changes selectionMode

	void focus() override;
	void close() override;
	void contextMenuRequested(const QPoint& p);


};
