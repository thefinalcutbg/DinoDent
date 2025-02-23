#include "IRC.h"
#include <QTimer>
#include <QColor>
#include <QHostInfo>
#include <random>

#include "Model/User.h"

IRC::IRC(QObject* parent) : QObject(parent)
{
	timeout_timer = new QTimer(this);

    connect(&m_socket, &QTcpSocket::connected, this, [&] {
		
		sendMsg(QString("USER na 0 0 na"));
		
		QString command = "NICK " + m_nick.nickname();
		
		sendMsg(command);
    });

    connect(&m_socket, &QTcpSocket::readyRead, this, [&] {
		
		QByteArray line;

		timeout_timer->start(130000);

		while (true) {

			line = m_socket.readLine();

			if (line.isEmpty()) break;

			handleMsg(line);
		}
    });

	connect(timeout_timer, &QTimer::timeout, this, [&] { 
		
		emit disconnected();

		connectToServ();
	});
}

void IRC::connectToServ()
{

	if (!m_nick.isValid()) return;

	if (dont_reconnect) return;
	//trying to reconnect every 30 seconds
	timeout_timer->start(30000);

	QHostInfo hostInfo = QHostInfo::fromName(server);

	if (hostInfo.addresses().empty()) return;

	m_socket.connectToHost(hostInfo.addresses().at(0), 9000);
}


void IRC::setNames(const std::string& fname, const std::string& lname)
{
	m_nick = Nickname(fname, lname, User::isIncognito());

	dont_reconnect = false;

	sendMsg(QString("NICK ") + m_nick.nickname());
}

void IRC::setInvisible(bool invisible)
{
	User::setIncognito(invisible);

	m_nick.setInvisible(invisible);

	sendMsg(QString("NICK ") + m_nick.nickname());
}

void IRC::disconnect()
{
	m_socket.close();

	timeout_timer->stop();

	dont_reconnect = true;
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

void IRC::handleMsg(const QByteArray& line)
{
	
	if (line.startsWith("PING ")) {
		sendMsg("PONG :" + server);
		return;
	}
	
	//msg_buffer starts with : and ends with \n
	if (line.startsWith(':') && line.endsWith('\n')) {
		msg_buffer = line;
	}
	else if (line.startsWith(':')) {
		msg_buffer = line;
		return;
	}
	else if (line.endsWith('\n')) {
		msg_buffer += line;
	}
	else {
		msg_buffer += line;
		return;
	}

	bool cmdBegin = false;

	QString command;

	//parsing command
	for (int i = 0; i < msg_buffer.size(); i++) {

		if (msg_buffer[i] == ' ') {

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

		command += msg_buffer[i];
	}

	//handling chat messages:

	if (command == "PRIVMSG") {
		handlePrivateMessage(msg_buffer);
		return;
	}

    //welcome message
    if (command == "001") {
		currentUsers = 0;
		m_userList.clear();
        sendMsg("JOIN " + channel);
        return;
    }

	//nick collision
	if (command == "433" || command == "436"){
		m_nick.rehashNickname();
		sendMsg(QString("NICK ") + m_nick.nickname());
		return;
	}

    //handling user list
    if (command == "353") {
		handleUserList(msg_buffer);
        return;
    }

    //end of user list. user is joined.
    if (command ==  "366") {
		emit userListChanged(m_userList);

        return;
    }

	if (command == "JOIN") {

		auto nick = getUserName(msg_buffer);

		if (nick == m_nick.nickname()) return;
		m_userList.push_back(getUserName(msg_buffer));
		emit userListChanged(m_userList);
		return;
	}

	if (command == "PART" || command == "QUIT") {
		
		auto userName = getUserName(msg_buffer);

		for (int i = 0; i < m_userList.size(); i++) {
			if (userName == m_userList[i].nickname())
			{
				m_userList.erase(m_userList.begin() + i);
				break;
			}
		}

		emit userListChanged(m_userList);
		return;
	}

	if (command == "332" || command == "TOPIC") {

		handleTopic(msg_buffer);
		return;
	}

	if (command == "NICK") {
		handleNickchange(msg_buffer);
		emit userListChanged(m_userList);
	};
}

bool IRC::sendMsg(const QString& str)
{
	int result = m_socket.write((str + "\r\n").toUtf8());

	if (result != -1) return true;

	m_userList.clear();

	emit disconnected();

	connectToServ();

	return false;
}

void IRC::handleUserList(const QString& msg)
{
	static const QString delimiter = channel + " :";

	int msgBegin = msg.indexOf(delimiter, 0) + delimiter.size();

	int msgEnd = msg.indexOf("\r\n", 0);

	QString buffer;

	for (int i = msgBegin; i < msgEnd; i++) {
		
		if (msg[i].isSpace()) {

			if (buffer == m_nick.nickname()) {
				m_userList.push_back(m_nick);
			}
			else {
				m_userList.push_back(buffer);
			}

			if (m_userList.back().parsedName().isEmpty()) {
				m_userList.pop_back();
			}

			buffer.clear();
			continue;
		}
		
		buffer += msg[i];
	}

}

void IRC::handleNickchange(const QString& msg)
{
	auto oldNick = getUserName(msg);

	static const QString delimiter = " NICK ";

	int msgBegin = msg.indexOf(delimiter, 0) + delimiter.size();

	int msgEnd = msg.indexOf("\r\n", 0);

	QString newNick;

	for (int i = msgBegin; i < msgEnd; i++) {
		newNick += msg[i];
	}

	for (auto& n : m_userList) {

		if (n.nickname() == oldNick) {
			n = newNick == m_nick.nickname() ?
				m_nick : newNick;

			return;
		}
	}

	m_userList.push_back(newNick);
	
}

QString IRC::getUserName(const QString& msg)
{
	QString result;

	for (auto c : msg)
	{
		if (c == ':') continue;

		if (c == '!') break;

		result += c;
	}

	return result;
}

