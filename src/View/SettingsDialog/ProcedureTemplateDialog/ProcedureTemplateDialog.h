#pragma once

#include <QDialog>
#include <optional>
#include "Model/Validator/CommonValidators.h"
#include "ui_ProcedureTemplateDialog.h"
#include "Model/Procedure/ProcedureTemplate.h"

class ProcedureTemplateDialog : public QDialog
{
	Q_OBJECT

	std::optional<ProcedureTemplate> m_procedureTemplate;

	NotEmptyValidator m_validator;

	QRadioButton* procedureType[9];

	ProcedureTemplateType currentType {ProcedureTemplateType::general};


public:

	ProcedureTemplateDialog(const ProcedureTemplate* pTemp, int code = 0, QWidget* parent = Q_NULLPTR);
	std::optional<ProcedureTemplate> getProcedureTemplate();
	~ProcedureTemplateDialog();

private:
	Ui::ProcedureTemplateDialog ui;
};
