#include "ChatDialog.h"

#include <QScrollBar>

#include "DinoDent.h"
#include "Network/IRC/IRC.h"
#include "Network/IRC/IRCInterface.h"
#include "Model/User.h"
#include <QDir>
#include <QStandardPaths>

ChatDialog::ChatDialog(DinoDent* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	setWindowTitle("IRC чат");
	setWindowIcon(QIcon(":/icons/icon_mirc.png"));

	setWindowFlag(Qt::WindowMaximizeButtonHint);

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

	//getting logs
	QFile log(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("irc_log.txt"));

	log.open(QIODevice::OpenModeFlag::ReadWrite);
	
	//deleting large log
	if (log.size() > 1000000) {
		log.resize(0);
	}

	while (!log.atEnd()) {
		ui.textEdit->append(log.readLine());
	}

	scrollToBottom();

	connect(&IRCInterface::getClient(), &IRC::topicRecieved, this, [&](const QString& topic) {

		if (topic == m_topic) return; //just reconnecting

		if (m_topic.size()) //if it isn't the initial topic
		{
			if (!isVisible()) {
				static_cast<DinoDent*>(this->parent())->setIrcIcon(true);
			}
		}

		m_topic = topic;

		const QString separator = "<br>___________________________________________________________________________<br><br>";

		appendText(separator + topic + separator);

	});


	connect(&IRCInterface::getClient(), &IRC::userListChanged, this, [&](const std::vector<Nickname>& usrList) {

		QString label = "Потребители онлайн: ";
		label += QString::number(usrList.size());

		ui.countLabel->setText(label);

		ui.usrList->clear();

		for (auto& u : usrList) {

			if (!u.isValid()) continue;

			if (u.isInvisible()) continue;

			QListWidgetItem *i = new QListWidgetItem(u.parsedName());
			i->setForeground(QColor(colorTable[u.hash()]));

			//setting bold font to user
			if (u.isCurrentUser()) {
				auto font = i->font();
				font.setBold(true);
				i->setFont(font);
			}

			u.isCurrentUser() ? 
					ui.usrList->insertItem(0, i)
					:
					ui.usrList->addItem(i);
		
		}

		ui.usrList->sortItems();
	});

	connect(&IRCInterface::getClient(), &IRC::disconnected, this, [&] {
		ui.countLabel->setText("Няма връзка със сървъра");
		ui.usrList->clear();
	});

    connect(&IRCInterface::getClient(), &IRC::msgRecieved, this, [&](const Nickname& nick, const QString& msg) {

		if (!nick.isValid()) return;

		QString result = "<font color=\"";
		result += colorTable[nick.hash()];
		result += "\"><b>";
		result += nick.parsedName();
		result += "</b>: ";
		result += msg;
		result += "</font>";

		appendText(result);

		session_log.append(result + "\n");

		if (!isVisible()) {
			static_cast<DinoDent*>(this->parent())->setIrcIcon(true);
		}

	});

	connect(ui.sendButton, &QPushButton::clicked, [&] {
		IRCInterface::getClient().sendMessage(ui.lineEdit->text());
		ui.lineEdit->clear();
		ui.lineEdit->setFocus();
	});

	connect(ui.invisibleCheck, &QCheckBox::clicked, this, [&](bool checked) {
		IRCInterface::getClient().setInvisible(checked);
	});

	ui.lineEdit->setFocus();
}

void ChatDialog::openAndConnect()
{
	if (!ui.usrList->count()) {
		IRCInterface::getClient().forceConnect();
	}

	exec();
}

void ChatDialog::appendText(const QString& text)
{
	ui.textEdit->append(text);

	scrollToBottom();
}

void ChatDialog::scrollToBottom()
{
	if (ui.textEdit->verticalScrollBar()) {
		ui.textEdit->verticalScrollBar()->setValue(
			ui.textEdit->verticalScrollBar()->maximum());
	}
}

bool ChatDialog::event(QEvent* e)
{
	if (e->type() == QEvent::Show) {
		QSignalBlocker b(ui.invisibleCheck);
		ui.invisibleCheck->setChecked(User::isIncognito());
	}

	return QDialog::event(e);
}

ChatDialog::~ChatDialog()
{
	if (session_log.isEmpty()) return;

	QFile log(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("irc_log.txt"));
	log.open(QIODevice::Append);
	QString header = "<br><font color=\"#777777\"><i>• ";
	header += QDate::currentDate().toString("dd.MM.yyyy");
	header += " •</i></font><br>\n";

	log.write(header.toUtf8());
	log.write(session_log.toUtf8());
}

