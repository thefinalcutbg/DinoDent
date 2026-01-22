#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>

#include "ui_DinoDent.h"

class ChatDialog;
class TabView;

class DinoDent : public QMainWindow
{
    Q_OBJECT

    ChatDialog* m_chatDialog = nullptr;

    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

public:

    DinoDent(QWidget *parent = Q_NULLPTR);

    TabView* tabView();
    void setUserLabel(const std::string& doctorName, const std::string& practiceName);
    void exitProgram();
    bool initialized();
    void disableButtons(bool printDisabled, bool saveDisabled, bool pdfDisabled);
    void setIrcIconAlert(bool glow);
    void setNotificationIcon(int activeNotifCount);
    void openIrc();

    bool m_loggedIn = false; //very cool public member varaible

    ~DinoDent();

private:
    Ui::DinoDentClass ui;
};
