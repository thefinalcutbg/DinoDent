#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>

#include "ui_DinoDent.h"

#include "View/Interfaces/IMainView.h"

class ChatDialog;

class DinoDent : public QMainWindow, public IMainView
{
    Q_OBJECT

    ChatDialog* m_chatDialog = nullptr;

    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

public:
    DinoDent(QWidget *parent = Q_NULLPTR);
    ITabView* tabView() override;
    void setUserLabel(const std::string& doctorName, const std::string& practiceName) override;
    void exitProgram() override;
    bool initialized();
    void disableButtons(bool printDisabled, bool saveDisabled, bool pdfDisabled);
    void setIrcIcon(bool glow);
    void setNotificationIcon(int activeNotifCount) override;
    void openIrc();

    ~DinoDent();

private:
    Ui::DinoDentClass ui;
};
