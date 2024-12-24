#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QDate>
#include "Model/Dental/Notification.h"
#include <optional>

namespace Ui {
class NotificationDialog;
}

class NotificationDialog : public QDialog
{
    Q_OBJECT

    std::optional<Notification> m_result;
    QDate m_currentDate = QDate::currentDate();

    QDate recalculateDate();

    void paintEvent(QPaintEvent* event) override;

public:
    explicit NotificationDialog(QWidget *parent = nullptr);
    ~NotificationDialog();

    std::optional<Notification> getResult() { return m_result; }

private:
    Ui::NotificationDialog *ui;
};

#endif // NOTIFICATIONDIALOG_H
