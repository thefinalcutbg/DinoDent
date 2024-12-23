#ifndef NOTIFICATIONLISTDIALOG_H
#define NOTIFICATIONLISTDIALOG_H

#include <QDialog>

namespace Ui {
class NotificationListDialog;
}

class NotificationListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationListDialog(QWidget *parent = nullptr);
    ~NotificationListDialog();

private:
    Ui::NotificationListDialog *ui;
};

#endif // NOTIFICATIONLISTDIALOG_H
