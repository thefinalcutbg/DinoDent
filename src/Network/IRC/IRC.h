#pragma once
#include <string>
#include <QTcpSocket>
#include "Nickname.h"
#include <QTimer>

class IRC : public QObject {

    Q_OBJECT

    static inline const QString channel = "#DinoDent";
    static inline const QString server = "hub.bgirc.com";

    QTcpSocket m_socket;
    
    Nickname m_nick;

    QByteArray msg_buffer;

    QTimer* timeout_timer;

    bool dont_reconnect = true;

    int currentUsers = 0;
    std::vector<Nickname> m_userList;

    void handleMsg(const QByteArray& line);
    bool sendMsg(const QString& str);

    void handleUserList(const QString& msg);
    void handleNickchange(const QString& msg);

    QString getUserName(const QString& msg);
    void handlePrivateMessage(const QString& msg);
    void handleTopic(const QString& msg);

    void connectToServ();

public:

    IRC(QObject* parent = nullptr);

	void setNames(const std::string& fname, const std::string& lname);
    void setInvisible(bool invisible);
    void sendMessage(const QString& msg);
    void disconnect();
    void forceConnect();

signals:
    void userListChanged(const std::vector<Nickname> usrList);
    void joined();
    void topicRecieved(const QString& topic);
    void msgRecieved(const Nickname& usr, const QString& msg);
    void disconnected();
};
