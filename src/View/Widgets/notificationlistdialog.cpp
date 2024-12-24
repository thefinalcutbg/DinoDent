#include "NotificationListDialog.h"
#include "ui_notificationlistdialog.h"

#include <QPainter>

#include "Model/TableRows.h"
#include "Model/User.h"
#include "Database/DbNotification.h"
#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/TabPresenter.h"

#include "View/CommonIcon.h"

NotificationListDialog::NotificationListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotificationListDialog)
{
    ui->setupUi(this);

    setWindowTitle("Напомняния");

    auto notifList = DbNotification::get(User::doctor().LPK);

    active_model.setNotifications(notifList, false);
    future_model.setNotifications(notifList, true);

    auto initTable = [&](QTableView* table){

        table->hideColumn(0);
        table->verticalHeader()->hide();
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeMode::Stretch);
        table->horizontalHeader()->setHighlightSections(false);
        table->setColumnWidth(5, 50);
        table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        table->setMouseTracking(true);
        auto removeDelegate = new NotificationRemoveDelegate();
        table->setItemDelegateForColumn(5, removeDelegate);
        table->selectRow(0);

        connect(removeDelegate, &NotificationRemoveDelegate::updateRequested, table, [=] {
            table->viewport()->update();
        });

        connect(removeDelegate, &NotificationRemoveDelegate::removeClicked, this, [=, this](long long rowId) {

            if(!ModalDialogBuilder::askDialog("Сигурни ли сте, че искате да изтриете напомнянето?")){
                return;
            }

            DbNotification::remove(rowId);

            auto newList = DbNotification::get(User::doctor().LPK);

            active_model.setNotifications(newList, false);
            future_model.setNotifications(newList, true);

        });

        connect(table, &QTableView::doubleClicked, this, [=, this](const QModelIndex& index){

            long long patientRowid = table->model()->index(index.row(), 0).data().toLongLong();
            long long notifRowid = table->model()->index(index.row(), 5).data().toLongLong();

            appointmentLogic(patientRowid, notifRowid);
        });
    };

    ui->futureTable->setModel(&future_model);
    ui->activeTable->setModel(&active_model);

    initTable(ui->activeTable);
    initTable(ui->futureTable);

    ui->appointmentButton->setIcon(CommonIcon::getPixmap(CommonIcon::CALENDAR));

    connect(ui->appointmentButton, &QPushButton::clicked, this, [&]{

        auto table = ui->tabWidget->currentIndex() ? ui->futureTable : ui->activeTable;

        if(!table->selectionModel()->hasSelection()) return;

        appointmentLogic(
            table->selectionModel()->selectedRows(0)[0].data().toLongLong(),
            table->selectionModel()->selectedRows(5)[0].data().toLongLong(),
            true
        );
    });


}

void NotificationListDialog::appointmentLogic(long long patientRowid, long long notifRowid, bool forceAppointment)
{
    int result(0);

    if(!forceAppointment){

    result = ModalDialogBuilder::openButtonDialog(
        {
            "Запази посещение",
            "Нов амбулаторен лист",
            "Нова рецепта",
            "Нова фактура",
            "Ново пародонтално измерване",
            "История на пациента"
        },
        "Отвори"
        );

        if (result == -1) return;
    }

    static TabType arr[6]{
        TabType::Calendar,
        TabType::AmbList,
        TabType::Prescription,
        TabType::Financial,
        TabType::PerioStatus,
        TabType::PatientSummary
    };

    if(arr[result] == TabType::Calendar){

        CalendarEvent ev(DbPatient::get(patientRowid));

        TabPresenter::get().openCalendar(ev);
    }
    else {
        RowInstance r(arr[result]);
        r.patientRowId = patientRowid;
        TabPresenter::get().open(r, true);
    }

    close();

    if(ui->tabWidget->currentIndex() //opening from future notifications
      || !ModalDialogBuilder::askDialog("Желаете ли да изтриете напомнянето?")
    ){
        return;
    }

    DbNotification::remove(notifRowid);
}

void NotificationListDialog::paintEvent(QPaintEvent*)
{
   // QPainter p(this);
  //  p.fillRect(rect(), Qt::white);
}


NotificationListDialog::~NotificationListDialog()
{
    delete ui;
}
