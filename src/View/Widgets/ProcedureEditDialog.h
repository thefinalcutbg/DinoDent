#pragma once

#include <QDialog>
#include "ui_ProcedureEditDialog.h"
#include "View/Interfaces/IProcedureEditDialog.h"


class ProcedureEditorPresenter;

class ProcedureEditDialog : public QDialog, public IProcedureEditDialog
{
	Q_OBJECT

private:
	ProcedureEditorPresenter* presenter;

	void paintEvent(QPaintEvent* event);

public:

	ProcedureEditDialog(ProcedureEditorPresenter* p, QWidget *parent = Q_NULLPTR);
	~ProcedureEditDialog();

	ICommonFields* commonFields() override;

	void closeDialog() override;

private:
	Ui::ProcedureEditDialog ui;
};
