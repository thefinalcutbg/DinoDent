#include "ChatDialog.h"
#include <QScrollBar>
#include "DinoDent.h"


ChatDialog::ChatDialog(DinoDent* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	setWindowTitle("IRC чат");
	setWindowIcon(QIcon(":/icons/icon_mirc_glow.png"));

	//default user color
	colorTable.push_back(QColor(2, 127, 128).name());

	for (int i = 1; i < 1000; i++) {

		colorTable.push_back(

			QColor::fromHsv(
				(double)i * 360.0 / (double)1000,
				255,
				128
			).name()
		);
	}

	connect(&m_irc, &IRC::joined, this, [&]{ });

	connect(&m_irc, &IRC::topicRecieved, this, [&](const QString& topic) {

		const QString separator = "<br>___________________________________________________________________________<br><br>";

		appendText(separator + topic + separator);
	});

	connect(&m_irc, &IRC::userListChanged, this, [&](const std::vector<Nickname>& usrList) {

		QString label = "Потребители онлайн: ";
		label += QString::number(usrList.size());

		ui.countLabel->setText(label);

		ui.usrList->clear();

		for (auto& u : usrList) {

			if (!u.isValid()) continue;

			QListWidgetItem *i = new QListWidgetItem(u.parsedName());
			i->setForeground(QColor(colorTable[u.hash()]));
			ui.usrList->addItem(i);
		
		}

	});

	connect(&m_irc, &IRC::disconnected, this, [&] {
		ui.countLabel->setText("Няма връзка със сървъра");
	});

    connect(&m_irc, &IRC::msgRecieved, this, [&](const Nickname& nick, const QString& msg) {

		if (!nick.isValid()) return;

		QString result = "<font color=\"";
		result += colorTable[nick.hash()];
		result += "\"><b>";
		result += nick.parsedName();
		result += "</b>: ";
		result += msg;
		result += "</font>";

		appendText(result);

		if (!isVisible()) {
			static_cast<DinoDent*>(this->parent())->setIrcIcon(true);
		}

	});

	connect(ui.sendButton, &QPushButton::clicked, [&] {
		m_irc.sendMessage(ui.lineEdit->text());
		ui.lineEdit->clear();
	});

	ui.lineEdit->setFocus();
}

void ChatDialog::appendText(const QString& text)
{
	ui.textEdit->append(text);

	if (ui.textEdit->verticalScrollBar()) {
		ui.textEdit->verticalScrollBar()->setValue(
			ui.textEdit->verticalScrollBar()->maximum());
	}

}


void ChatDialog::changeNickname(const std::string& fname, const std::string& lname)
{
	m_irc.changeNick(fname, lname);
}

void ChatDialog::connectToServer(const std::string& fname, const std::string& lname)
{
	m_irc.connectToServ(fname, lname);
}

void ChatDialog::disconnect()
{
	m_irc.disconnect();
	ui.countLabel->setText("Няма връзка със сървъра");
}

void ChatDialog::checkConnection()
{
	m_irc.ping();
}

ChatDialog::~ChatDialog()
{}

