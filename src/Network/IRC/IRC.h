#pragma once
#include <string>
#include <QTcpSocket>

class IRC : public QObject {

    Q_OBJECT

    QTcpSocket m_socket;
    QString m_nickname;

    QString fname;
    QString lname;

    int currentUsers = 0;

    void hashNickname();
    void handleMsg(const QString& str);
    bool sendMsg(const QString& str);

    //returns nick and hash index
     std::pair<QString, int> parseNickname(const QString& nickname);

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
    void msgRecieved(const QString& usr, int hashIndex, const QString& msg);
    void disconnected();
};
