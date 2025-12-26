#pragma once

#include <QDialog>
#include "ui_MultilineDialog.h"
#include <string>
#include "Database/DbNotes.h"

class MultilineDialog : public QDialog
{
	Q_OBJECT

	std::optional<std::string> m_result;

    DbNotes::TemplateType m_template_type;

public:

    MultilineDialog(const std::string& text, QWidget *parent = Q_NULLPTR);
	std::optional<std::string> getResult();
    void enableEditing();
    void enableNotesFormat();
    void enableTemplateLoading(DbNotes::TemplateType type);
	~MultilineDialog();

private:
	Ui::MultilineDialog ui;
};
