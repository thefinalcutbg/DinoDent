#include "notificationdialog.h"
#include "ui_notificationdialog.h"

NotificationDialog::NotificationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotificationDialog)
{
    ui->setupUi(this);

    setWindowTitle("Добавяне на напомняне");

    connect(ui->okButton, &QPushButton::clicked, this, [&]{

        Notification n;
        n.date = ui->dateEdit->getDate();
        n.description = ui->plainTextEdit->toPlainText().toStdString();
        m_result = n;
        close();
    });
}

NotificationDialog::~NotificationDialog()
{
    delete ui;
}
