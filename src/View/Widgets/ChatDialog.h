#pragma once

#include <QDialog>
#include "ui_ChatDialog.h"

class DinoDent;

class ChatDialog : public QDialog
{
	Q_OBJECT

	std::vector<QString> colorTable;

	void appendText(const QString& text);

	bool event(QEvent* e) override;

public:
	ChatDialog(DinoDent *parent);
	
	~ChatDialog();

private:
	Ui::ChatDialogClass ui;
};
