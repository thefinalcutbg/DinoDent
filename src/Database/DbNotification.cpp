#include "Database/DbNotification.h"
#include "Database.h"
#include "Model/User.h"

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

std::vector<Notification> DbNotification::get(const std::string& lpk)
{
    Db db;

    db.newStatement(
        "SELECT notification.date, notification.rowid, notification.patient_rowid, notification.description, "
        "patient.fname, patient.lname, patient.phone "
        "FROM notification JOIN patient on notification.patient_rowid = patient.rowid "
        "WHERE lpk=? ORDER BY notification.date DESC"
    );

    db.bind(1, lpk);

    std::vector<Notification> result;

    while(db.hasRows())
    {
        result.emplace_back();

        auto& n = result.back();

        n.date = db.asString(0);
        n.rowid = db.asLongLong(1);
        n.patientRowid = db.asLongLong(2);
        n.description = db.asString(3);
        n.patientName = db.asString(4) + " " + db.asString(5);
        n.phone = db.asString(6);
    }

    return result;
}


bool DbNotification::remove(long long notifRowid)
{
    Db db("DELETE FROM notification WHERE rowid=?");
    db.bind(1, notifRowid);
    return db.execute();
}

int DbNotification::hasNotifications(const Date &date)
{
    Db db;

    db.newStatement(
        "SELECT COUNT(*) FROM notification WHERE "
        "date <= ? "
        "AND lpk = ? "
        );

    db.bind(1, date.to8601());
    db.bind(2, User::doctor().LPK);

    while(db.hasRows()){

        return db.asInt(0);
    }

    return 0;

}
