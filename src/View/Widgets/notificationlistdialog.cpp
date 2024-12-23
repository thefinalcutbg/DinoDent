#include "notificationlistdialog.h"
#include "ui_notificationlistdialog.h"
#include "Model/User.h"
#include "Database/DbNotification.h"

NotificationListDialog::NotificationListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotificationListDialog)
{
    ui->setupUi(this);

    auto notifList = DbNotification::get(Date::currentDate(), User::doctor().LPK);

    ui->activeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui->activeTable->setRowCount(notifList.size());
    ui->activeTable->setColumnCount(4);

    for(int i = 0; i < notifList.size(); i++)
    {
        auto& notif = notifList[i];

        QTableWidgetItem* cell = new QTableWidgetItem(notif.date.toBgStandard().c_str());
        ui->activeTable->setItem(i, 0, cell);

        cell = new QTableWidgetItem(notif.patientName.c_str());
        ui->activeTable->setItem(i, 1, cell);

        cell = new QTableWidgetItem(notif.phone.c_str());
        ui->activeTable->setItem(i, 2, cell);

        cell = new QTableWidgetItem(notif.description.c_str());
        ui->activeTable->setItem(i, 3, cell);
    }

    if(notifList.size())
        ui->activeTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);

}


NotificationListDialog::~NotificationListDialog()
{
    delete ui;
}
