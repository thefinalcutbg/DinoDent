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

    bool dont_reconnect = true;

    int currentUsers = 0;
    std::vector<Nickname> m_userList;
    void handleMsg(const QString& str);
    bool sendMsg(const QString& str);
    void handleUserList(const QString& msg);
    void handleNickchange(const QString& msg);
    void connectToServ();
    QString getUserName(const QString& msg);
    void handlePrivateMessage(const QString& msg);
    void handleTopic(const QString& msg);
public:

    IRC(QObject* parent = nullptr);

	void setNames(const std::string& fname, const std::string& lname);
    void setVisible(bool visible);
    void sendMessage(const QString& msg);
    void disconnect();


signals:
    void userListChanged(const std::vector<Nickname> usrList);
    void joined();
    void topicRecieved(const QString& topic);
    void msgRecieved(const Nickname& usr, const QString& msg);
    void disconnected();
};
