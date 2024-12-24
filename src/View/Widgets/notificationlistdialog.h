#pragma once

#include <QDialog>

#include "View/TableModels/NotificationTableModel.h"

namespace Ui {
class NotificationListDialog;
}

class NotificationListDialog : public QDialog
{
    Q_OBJECT

    NotificationTableModel active_model;
    NotificationTableModel future_model;

    void appointmentLogic(long long patientRowid, long long notificatioNRowid, const std::string& descr = {}, bool forceAppointment = false);

    void paintEvent(QPaintEvent* event) override;
public:
    explicit NotificationListDialog(QWidget *parent = nullptr);
    ~NotificationListDialog();

private:
    Ui::NotificationListDialog *ui;
};
