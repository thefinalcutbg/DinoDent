#include "Database/DbNotification.h"
#include "Database.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
bool DbNotification::insert(const Notification& n){

    Db db;

    db.newStatement(
        "INSERT INTO notification (date, patient_rowid, lpk, description) VALUES (?,?,?,?)"
    );

    db.bind(1, n.date.to8601());
    db.bind(2, n.patientRowid);
    db.bind(3, User::doctor().LPK);
    db.bind(4, n.description);

    return db.execute();
}
std::vector<Notification> DbNotification::get(const Date& currentDate, const std::string& lpk)
{
    Db db;

    db.newStatement(
        "SELECT notification.date, notification.rowid, notification.patient_rowid, notification.description, notification.dismissed, "
        "patient.fname, patient.lname, patient.phone "
        "FROM notification JOIN patient on notification.patient_rowid = patient.rowid "
        "WHERE date <= ? "
        "AND lpk = ? "
    );

    db.bind(1, currentDate.to8601());
    db.bind(2, lpk);

    std::vector<Notification> result;

    while(db.hasRows())
    {
        result.emplace_back();

        auto& n = result.back();

        n.date = db.asString(0);
        n.rowid = db.asLongLong(1);
        n.patientRowid = db.asLongLong(2);
        n.description = db.asString(3);
        n.dismissed = db.asBool(4);
        n.patientName = db.asString(5) + " " + db.asString(6);
        n.phone = db.asString(7);
    }

    return result;
}

