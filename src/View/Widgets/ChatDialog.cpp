#include "ChatDialog.h"
#include <QScrollBar>

ChatDialog::ChatDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	setWindowTitle("Чат канал");

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

	connect(&m_irc, &IRC::joined, this, [&]{
        ui.textEdit->append("Добре дошли в чат канала на DinoDent! Бъдете учтиви и пишете на кирилица :)");
	});

	connect(&m_irc, &IRC::userCountChanged, this, [&](int count) {

		QString label = "Активни потребители: ";
		label += QString::number(count);

		ui.countLabel->setText(label);
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

		ui.textEdit->append(result);

		if (ui.textEdit->verticalScrollBar()) {
			ui.textEdit->verticalScrollBar()->setValue(
				ui.textEdit->verticalScrollBar()->maximum());
		}
	});

	connect(ui.sendButton, &QPushButton::clicked, [&] {
		m_irc.sendMessage(ui.lineEdit->text());
		ui.lineEdit->clear();
	});

	ui.lineEdit->setFocus();
}

void ChatDialog::changeNickname(const std::string& fname, const std::string& lname)
{
	m_irc.changeNick(fname, lname);
}

void ChatDialog::connectToServer(const std::string& fname, const std::string& lname)
{
	m_irc.connectToServ("Христо", "Константинов");
}

void ChatDialog::disconnect()
{
	ui.textEdit->clear();
	m_irc.disconnect();
}

void ChatDialog::checkConnection()
{
	m_irc.ping();
}

ChatDialog::~ChatDialog()
{}

