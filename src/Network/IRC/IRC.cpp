#include "IRC.h"

#include <QColor>
#include <QHostInfo>
#include <random>

IRC::IRC(QObject* parent) : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::connected, this, [&] {
		
		sendMsg(QString("USER na 0 0 na"));
		
		QString command = "NICK " + m_nick.nickname();
		
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
	m_nick = Nickname(fname, lname);

	connectToServ();
}

void IRC::changeNick(const std::string& fname, const std::string& lname)
{
	m_nick = Nickname(fname, lname);

	sendMsg(QString("NICK ") + m_nick.nickname());
}

void IRC::handlePrivateMessage(const QString& msg)
{
	QString buffer;

	if (!msg.contains("PRIVMSG " + channel + " :")) return;

	//parsing the nickname and the color

	for (int i = 1; i < msg.size(); i++) {

		if (msg[i] == '!') break;

		buffer += msg[i];
	}

	Nickname nickname(buffer);

	if (!nickname.isValid()) return;

	buffer.clear();

	//parsing the message:

	static const QString delimiter = channel + " :";

	int msgBegin = msg.indexOf(delimiter, 0) + delimiter.size();

	int msgEnd = msg.indexOf("\r\n", 0);

	for (int i = msgBegin; i < msgEnd; i++) {
		buffer += msg[i];
	}

	emit msgRecieved(nickname, buffer);
}

void IRC::handleTopic(const QString& msg)
{
	static const QString delimiter = channel + " :";

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

	if (!sendMsg(QString("PRIVMSG " + channel + " :" + msg))) {
		return;
	};

	//emmiting signal to add the message to the textEdit

	emit msgRecieved(m_nick, msg);
}

void IRC::disconnect()
{
	m_socket.close();
}

void IRC::ping()
{
	sendMsg("PING :irc.bgirc.com");
}

void IRC::handleMsg(const QString& msg)
{
	if (msg.startsWith("PING ")) {
		sendMsg("PONG :" + server);
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
        sendMsg("JOIN " + channel);
        return;
    }

	//nick collision
	if (command == "433" || command == "436"){
		m_nick.generateNickname();
		sendMsg(QString("NICK ") + m_nick.nickname());
		return;
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

	connectToServ();

	return false;
}

void IRC::connectToServ()
{
	if (!m_nick.isValid()) return;

	QHostInfo hostInfo = QHostInfo::fromName(server);

	if (hostInfo.addresses().empty()) return;

	m_socket.connectToHost(hostInfo.addresses().at(0), 9000);
}

