#pragma once

#include <QDialog>
#include "ui_ChatDialog.h"

class DinoDent;

class ChatDialog : public QDialog
{
	Q_OBJECT

	QString m_topic;

	QString session_log;

	std::vector<QString> colorTable;

	void appendText(const QString& text);

	void scrollToBottom();

	bool event(QEvent* e) override;

	static QString linkify(const QString& plain);

public:
	ChatDialog(DinoDent *parent);
	
	void openAndConnect();

	~ChatDialog();

private:
	Ui::ChatDialogClass ui;
};
