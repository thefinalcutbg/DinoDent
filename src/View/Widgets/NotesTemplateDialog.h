#pragma once

#include <QDialog>
#include "ui_NotesTemplateDialog.h"
#include "Database/DbNotes.h"

class NotesTemplateDialog : public QDialog
{
	Q_OBJECT

	bool edited = false;

    DbNotes::TemplateType m_type;

	QString m_result;

public:
    NotesTemplateDialog(DbNotes::TemplateType type, QWidget *parent = nullptr);
    QString getResult() { exec(); return m_result; }
	~NotesTemplateDialog();

private:
	Ui::NotesTemplateDialogClass ui;
};
