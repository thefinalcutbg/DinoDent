#include "ChatDialog.h"
#include <QScrollBar>
#include "DinoDent.h"


ChatDialog::ChatDialog(DinoDent* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	setWindowTitle("mIRC чат");
	setWindowIcon(QIcon(":/icons/icon_mirc_glow.png"));

	userColor = QColor(38, 124, 121).name();

	for (int i = 0; i < 1000; i++) {

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

	connect(&m_irc, &IRC::userCountChanged, this, [&](int count) {

		QString label = "Потребители онлайн: ";
		label += QString::number(count);

		ui.countLabel->setText(label);
	});

	connect(&m_irc, &IRC::disconnected, this, [&] {
		ui.countLabel->setText("Няма връзка със сървъра");
	});

    connect(&m_irc, &IRC::msgRecieved, this, [&](const QString& usr, int hash, const QString& msg) {

		if (usr.isEmpty()) return;

		QString color =
			(hash < 0 || hash > 999) ? userColor : colorTable[hash];

		QString result = "<font color=\"";
		result += color;
		result += "\"><b>";
		result += usr;
		result += "</b>: ";
		result += msg;
		result += "</font>";

		appendText(result);

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


	if (!isVisible()) {
		static_cast<DinoDent*>(this->parent())->setIrcIcon(true);
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

