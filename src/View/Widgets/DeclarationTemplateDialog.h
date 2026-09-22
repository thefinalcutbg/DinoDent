#pragma once

#include <QDialog>
#include "ui_DeclarationTemplateDialog.h"
#include <optional>
#include "Model/DeclarationTemplate.h"

class DeclarationTemplateDialog : public QDialog
{
	Q_OBJECT

	long long m_rowid = 0;

	std::optional<DeclarationTemplate> m_result;

	void insertTag(const QString& tag);

	bool checkForm();

public:
	DeclarationTemplateDialog(const DeclarationTemplate& d);
	std::optional<DeclarationTemplate> getResult() { exec(); return m_result; }
	~DeclarationTemplateDialog();

private:
	Ui::DeclarationTemplateDialogClass ui;
};

