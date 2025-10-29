#pragma once

#include <QDialog>
#include "ui_ProcedureEditDialog.h"

class ProcedureEditorPresenter;

class ProcedureEditDialog : public QDialog
{
	Q_OBJECT

private:
    ProcedureEditorPresenter& presenter;
    void paintEvent(QPaintEvent* event) override;

public:
    ProcedureEditDialog(ProcedureEditorPresenter& p, QWidget *parent = Q_NULLPTR);
	ProcedureInput* procedureInput();
	~ProcedureEditDialog();

private:
	Ui::ProcedureEditDialog ui;
};
