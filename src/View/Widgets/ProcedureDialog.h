#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "ui_ProcedureDialog.h"

#include "View/TableModels/ProcedureTemplateModel.h"

class ProcedureDialogPresenter;

class ProcedureDialog : public QDialog
{
	Q_OBJECT

	inline static int s_idx{ -1 };
	inline static QString s_search{""};

    ProcedureDialogPresenter &presenter;

	ProcedureTemplateModel model;
	QSortFilterProxyModel proxyModel;

public:
    ProcedureDialog(ProcedureDialogPresenter& presenter, QWidget* parent = Q_NULLPTR);
	 ~ProcedureDialog();

	void setSelectionLabel(const std::vector<int>& selectedTeethNum);

	IProcedureInput* procedureInput();
	ProcedureListView* procedureList();

private:
	Ui::ProcedureDialog ui;

};
