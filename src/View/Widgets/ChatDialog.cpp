#include "ChatDialog.h"

#include <QScrollBar>

#include "DinoDent.h"
#include "Network/IRC/IRC.h"
#include "Network/IRC/IRCInterface.h"
#include "Model/User.h"

ChatDialog::ChatDialog(DinoDent* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	setWindowTitle("IRC чат");
	setWindowIcon(QIcon(":/icons/icon_mirc_glow.png"));

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

	connect(&IRCInterface::getClient(), &IRC::topicRecieved, this, [&](const QString& topic) {

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

	});

	connect(&IRCInterface::getClient(), &IRC::disconnected, this, [&] {
		ui.countLabel->setText("Няма връзка със сървъра");
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

		if (!isVisible()) {
			static_cast<DinoDent*>(this->parent())->setIrcIcon(true);
		}

	});

	connect(ui.sendButton, &QPushButton::clicked, [&] {
		IRCInterface::getClient().sendMessage(ui.lineEdit->text());
		ui.lineEdit->clear();
	});

	connect(ui.invisibleCheck, &QCheckBox::clicked, this, [&](bool checked) {
		IRCInterface::getClient().setInvisible(checked);
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

bool ChatDialog::event(QEvent* e)
{
	if (e->type() == QEvent::Show) {
		QSignalBlocker b(ui.invisibleCheck);
		ui.invisibleCheck->setChecked(User::isIncognito());
	}

	return QDialog::event(e);
}

ChatDialog::~ChatDialog()
{}

