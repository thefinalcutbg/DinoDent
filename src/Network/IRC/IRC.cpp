#include "IRC.h"

#include <QColor>
#include <QHostInfo>
#include <random>

IRC::IRC(QObject* parent) : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::connected, this, [&] {
		
		sendMsg(QString("USER na 0 0 na"));
		
		QString command = "NICK " + m_nickname;
		
		sendMsg(command);
    });

    connect(&m_socket, &QTcpSocket::readyRead, this, [&] {
		
		QByteArray line;

		while (true) {

			line = m_socket.readLine();

			if (line.isEmpty()) return;

			handleMsg(line);
		}
    });

}

void IRC::connectToServ(const std::string& fname, const std::string& lname)
{
	this->fname = fname.c_str();
	this->lname = lname.c_str();

	hashNickname();

    QHostInfo hostInfo = QHostInfo::fromName("irc.bgirc.com");
	
	if (hostInfo.addresses().empty()) return;

	m_socket.connectToHost(hostInfo.addresses().at(0), 9000);
}

void IRC::changeNick(const std::string& fname, const std::string& lname)
{
	this->fname = fname.c_str();
	this->lname = lname.c_str();

	hashNickname();

	sendMsg(QString("NICK ") + m_nickname);
}

void IRC::handlePrivateMessage(const QString& msg)
{
	QString buffer;

	if (!msg.contains("PRIVMSG #DinoDent :")) return;

	//parsing the nickname and the color

	for (int i = 1; i < msg.size(); i++) {

		if (msg[i] == '!') break;

		buffer += msg[i];
	}

	auto [nickname, hashIdx] = parseNickname(buffer);

	buffer.clear();

	//parsing the message:

	static const QString delimiter = "#DinoDent :";

	int msgBegin = msg.indexOf(delimiter, 0) + delimiter.size();

	int msgEnd = msg.indexOf("\r\n", 0);

	for (int i = msgBegin; i < msgEnd; i++) {
		buffer += msg[i];
	}

	emit msgRecieved(nickname, hashIdx, buffer);
}

void IRC::handleTopic(const QString& msg)
{
	static const QString delimiter = "#DinoDent :";

	int msgBegin = msg.indexOf(delimiter, 0) + delimiter.size();

	int msgEnd = msg.indexOf("\r\n", 0);

	QString topic;

	for (int i = msgBegin; i < msgEnd; i++) {
		topic += msg[i];
	}

	emit topicRecieved(topic);
}

void IRC::sendMessage(const QString& msg)
{
	if (msg.isEmpty()) return;

	if (!sendMsg(QString("PRIVMSG #DinoDent :" + msg))) {
		return;
	};

	//emmiting signal to add the message to the textEdit
	auto [nickname, hashIdx] = parseNickname(m_nickname);

	emit msgRecieved(nickname, hashIdx, msg);
}

void IRC::disconnect()
{
	m_socket.close();
}

void IRC::ping()
{
	sendMsg("PING :irc.bgirc.com");
}

void IRC::hashNickname()
{
	m_nickname.clear();

	m_nickname += fname;
	m_nickname += '_';

	m_nickname += lname;

	constexpr const int max = 21;

	if (m_nickname.length() > max) {

		m_nickname = lname;

		if (m_nickname.length() > max) {
			m_nickname.truncate(max);
		}
	}

	for (int i = 0; i < m_nickname.size(); i++) {
		if (m_nickname[i] == ' ') {
			m_nickname[i] = '_';
		}
	}

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, 999); // define the range

	m_nickname += QString::number(distr(gen));

}


std::pair<QString, int> IRC::parseNickname(const QString& nick)
{
	QString nickname;

	QString hashIndex;

	for (const QChar c : nick) {

		if (c.isDigit()) {
			hashIndex += c;
		}
		else if (c == '_') {
			nickname += ' ';
		}
		else {
			nickname += c;
		}
	}

	if (nickname != "Нов потребител") {
		nickname.prepend("д-р ");
	}

	if (nick == m_nickname) {
		return { nickname, -1 };
	}

	//nickname does not confide with rules
	if (hashIndex.isEmpty()) {
		return { {}, -1 };
	}

	int colorIndex = hashIndex.toInt();

	if (colorIndex < 0 || colorIndex > 999) {
		colorIndex = 0;
	}



	return { nickname, colorIndex };
}

void IRC::handleMsg(const QString& msg)
{
	if (msg.startsWith("PING ")) {
		sendMsg("PONG :irc.bgirc.com");
	}
	
	if (!msg.startsWith(":")) return;

	bool cmdBegin = false;

	QString command;

	//parsing command
	for (int i = 0; i < msg.size(); i++) {

		if (msg[i].isSpace()) {

			if (!cmdBegin) {
				cmdBegin = true;
				continue;
			}
			else {
				break;
			}
		}

		if (!cmdBegin) {
			continue;
		}

		command += msg[i];
	}

	//handling chat messages:

	if (command == "PRIVMSG") {
		handlePrivateMessage(msg);
		return;
	}

    //welcome message
    if (command == "001") {
		currentUsers = 0;
        sendMsg("JOIN #DinoDent");
        return;
    }

	//nick collision
	if (command == "433" || command == "436"){
		hashNickname();
		sendMsg(QString("NICK ") + m_nickname);
	}

    //handling user list
    if (command == "353") {

        currentUsers += msg.count(" ")-5;

        return;
    }

    //end of user list. user is joined.
    if (command ==  "366") {

        emit joined();
		emit userCountChanged(--currentUsers);
        return;
    }

	if (command == "JOIN") {

		emit userCountChanged(++currentUsers);
		return;
	}

	if (command == "PART" || command == "QUIT") {

		emit userCountChanged(--currentUsers);
		return;
	}

	if (command == "332" || command == "TOPIC") {

		handleTopic(msg);
		return;
	}

}

bool IRC::sendMsg(const QString& str)
{
	int result = m_socket.write((str + "\r\n").toUtf8());

	if (result != -1) return true;

	emit disconnected();

	connectToServ(fname.toStdString(), lname.toStdString());

	return false;
}

