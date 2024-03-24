#pragma once

#include <QDialog>
#include "ui_ChatDialog.h"
#include "Network/IRC/IRC.h"

class ChatDialog : public QDialog
{
	Q_OBJECT

	IRC m_irc;
	std::vector<QString> colorTable;
	QString userColor;

public:
	ChatDialog(QWidget *parent = nullptr);
	void changeNickname(const std::string& fname, const std::string& lname);
	void connectToServer(const std::string& fname, const std::string& lname);
	void disconnect();
	void checkConnection();
	~ChatDialog();

private:
	Ui::ChatDialogClass ui;
};
