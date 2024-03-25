#pragma once
#include <string>
#include <QTcpSocket>
#include "Nickname.h"

class IRC : public QObject {

    Q_OBJECT

    static inline const QString channel = "#DinoDent";
    static inline const QString server = "irc.bgirc.com";

    QTcpSocket m_socket;
    
    Nickname m_nick;

    int currentUsers = 0;

    void handleMsg(const QString& str);
    bool sendMsg(const QString& str);

    void connectToServ();

public:

    IRC(QObject* parent = nullptr);
	void connectToServ(const std::string& fname, const std::string& lname);
	void changeNick(const std::string& fname, const std::string& lname);
    void handlePrivateMessage(const QString& msg);
    void handleTopic(const QString& msg);
    void sendMessage(const QString& msg);
    void disconnect();
    void ping();

signals:
    void userCountChanged(int count);
    void joined();
    void topicRecieved(const QString& topic);
    void msgRecieved(const Nickname& usr, const QString& msg);
    void disconnected();
};
