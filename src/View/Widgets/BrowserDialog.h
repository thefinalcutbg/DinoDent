#pragma once

#include <QDialog>
#include "ui_BrowserDialog.h"
#include "View/TableModels/PlainTableModel.h"
#include "View/TableModels/NameFilterProxyModel.h"
#include "Presenter/BrowserPresenter.h"

class BrowserDialog : public QDialog
{
	Q_OBJECT

private:

	BrowserPresenter presenter;

	QSortFilterProxyModel idFilter;
	NameFilterProxyModel nameFilter;
	QSortFilterProxyModel phoneFilter;
	QMenu* main_menu{ nullptr };

	PlainTableModel table_model;
	PlainTableModel preview_model;

	void setCountLabel();
	void calculateUiState();

    bool eventFilter(QObject *obj, QEvent *event);

public:
    BrowserDialog();

	void setUiState(const BrowserUiState& state) ;
	void setTable(const PlainTable& t, int idColumn, int nameColumn, int phoneColumn);
	void setPreview(const PlainTable& t) ;
	void focus();
	void close();
	~BrowserDialog();

private:
	Ui::BrowserDialog ui;

	//shows the additional buttons and changes selectionMode
    void datesChanged();

	void contextMenuRequested(const QPoint& p);


};
