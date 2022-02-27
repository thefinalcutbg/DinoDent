#pragma once

#include <QDialog>
#include "ui_ProcedureTemplateDialog.h"

class ProcedureTemplateDialog : public QDialog
{
	Q_OBJECT

public:
	ProcedureTemplateDialog(QWidget *parent = Q_NULLPTR);
	~ProcedureTemplateDialog();

private:
	Ui::ProcedureTemplateDialog ui;
};
