#pragma once

#include <QDialog>
#include "ui_ProcedureTemplateDialog.h"
#include "Model/Procedure/ProcedureTemplate.h"
#include <optional>

class ProcedureTemplateDialog : public QDialog
{
	Q_OBJECT

	std::optional<ProcedureTemplate> m_procedureTemplate;

public:

	ProcedureTemplateDialog(const ProcedureTemplate* pTemp = nullptr, QWidget* parent = Q_NULLPTR);
	std::optional<ProcedureTemplate> getProcedureTemplate();
	~ProcedureTemplateDialog();

private:
	Ui::ProcedureTemplateDialog ui;
};
