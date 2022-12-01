#include "DbReferral.h"
#include "Database/Database.h"
#include "Model/User.h"

int DbReferral::getRefNumber(ReferralType type, int year)
{
    std::string query =

        "SELECT MAX(number) FROM referral "
        "LEFT JOIN amblist ON referral.amblist_rowid = amblist.rowid "
        "WHERE referral.type = ? "
        "AND strftime('%Y',referral.date)=? "
        "AND amblist.lpk=? "
        "AND amblist.practice=? "
        ;

    int result{ 0 };

    Db db(query);
    db.bind(1, static_cast<int>(type));
    db.bind(2, year);
    db.bind(3, User::doctor().LPK);
    db.bind(4, User::practice().rziCode);

    while (db.hasRows())
    {
        result = db.asInt(0);
    }

    return result + 1;
}

bool DbReferral::refNumberExists(ReferralType type, int year, long long refRowid)
{
    auto query =

        "SELECT COUNT(num) FROM referral JOIN amblist ON referral.amblist_id = amblist.id WHERE "
        "AND amblist.lpk =? " 
        "AND amblist.rzi =? "
        "AND strftime('%Y',referral.date)=? "
        "AND referral.type =? "
        "AND referral.rowid != ?"
        ;

    Db db{ query };

    db.bind(1, User::doctor().LPK);
    db.bind(2, User::practice().rziCode);
    db.bind(3, year);
    db.bind(4, static_cast<int>(type));
    db.bind(5, refRowid);


    while(db.hasRows()) {
        return db.asBool(0);
    }

    return false;
}
