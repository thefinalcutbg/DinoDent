#pragma once

#include <QDialog>
#include "View/TableModels/PlainTableModel.h"
#include "View/TableModels/NameFilterProxyModel.h"

namespace Ui { class NewDocDialogClass; };

class NewDocPresenter;

class NewDocDialog : public QDialog
{
	Q_OBJECT

	QSortFilterProxyModel idFilter;
	NameFilterProxyModel nameFilter;
	QSortFilterProxyModel phoneFilter;

	PlainTableModel table_model;

	bool eventFilter(QObject *obj, QEvent *event);

public:
	NewDocDialog(NewDocPresenter &presenter);
	void setTable(const PlainTable& t);
	~NewDocDialog();

private:
	Ui::NewDocDialogClass *ui;
};
