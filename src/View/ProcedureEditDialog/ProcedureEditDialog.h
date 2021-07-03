#pragma once

#include <QDialog>
#include "ui_ProcedureEditDialog.h"
#include "IProcedureEditDialog.h"

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

	void setMtype(ProcedureType m) override;
	ICommonFields* commonFields() override;

	ICrownView* crownView() override;
	IObturationView* obturationView() override;
	IImplantView* implantView()override;
	void closeDialog() override;

private:
	Ui::ProcedureEditDialog ui;
};
