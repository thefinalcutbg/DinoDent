#pragma once

#include <QDialog>
#include "ui_NotesTemplateDialog.h"

class NotesTemplateDialog : public QDialog
{
	Q_OBJECT

	bool edited = false;

	QString m_result;

public:
	NotesTemplateDialog(QWidget *parent = nullptr);
	QString getResult() const { return m_result; }
	~NotesTemplateDialog();

private:
	Ui::NotesTemplateDialogClass ui;
};
