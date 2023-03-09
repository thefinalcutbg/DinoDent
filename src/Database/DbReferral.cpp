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
        "AND amblist.rzi=? "
        ;

    int result{ 0 };

    Db db(query);
    db.bind(1, static_cast<int>(type));
    db.bind(2, std::to_string(year));
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
    db.bind(3, std::to_string(year));
    db.bind(4, static_cast<int>(type));
    db.bind(5, refRowid);


    while(db.hasRows()) {
        return db.asBool(0);
    }

    return false;
}

std::vector<Referral> DbReferral::getReferrals(long long ambListId, Db& db)
{
    std::vector<Referral> referrals;

    db.newStatement(
        "SELECT referral.type, referral.number, referral.date, referral.reason,"
        "referral.diag_main, referral.diag_add, comorb_main, comorb_add, referral.tooth, "
        "referral.spec_119, referral.reason_119, referral.motives_119 "
        "FROM referral LEFT JOIN amblist ON referral.amblist_rowid=amblist.rowid "
        "WHERE amblist.rowid = ?"
    );

    db.bind(1, ambListId);

    while (db.hasRows())
    {
       referrals.emplace_back(static_cast<ReferralType>(db.asInt(0)));

        auto& ref = referrals.back();

        ref.number = db.asInt(1);
        ref.date = db.asString(2);
        ref.reason = db.asInt(3);
        ref.diagnosis.main = db.asString(4);
        ref.diagnosis.additional = db.asString(5);
        ref.comorbidity.main = db.asString(6);
        ref.comorbidity.additional = db.asString(7);

        if (ref.type == ReferralType::MDD4) {
            ref.data = MDD4Data(db.asInt(8));
        }

        if (ref.type == ReferralType::MH119) {

            ref.data = MH119Data{
                .specCode = db.asInt(9),
                .reason = db.asInt(10),
                .description = db.asString(11)
            };
        }

    }

    return referrals;
}

void DbReferral::saveReferrals(const std::vector<Referral>& ref, long long ambListRowid, Db& db)
{
    db.newStatement("DELETE FROM referral WHERE amblist_rowid = ?");
    db.bind(1, ambListRowid);
    db.execute();

    for (auto& r : ref)
    {
        db.newStatement(
            "INSERT INTO referral "
            "(amblist_rowid, number, nrn, lrn, date, reason, "
            "diag_main, diag_add, comorb_main, comorb_add, type, "
            "tooth, reason_119, motives_119, spec_119) "//<-specific for some referrals
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) "
        );

        db.bind(1, ambListRowid);

        if (r.isNrnType()) {
            db.bind(3, r.nrn);
            db.bind(4, r.lrn);
        }
        else
        {
            db.bind(2, r.number);
        }

        db.bind(5, r.date.to8601());
        db.bind(6, r.reason.getIndex());
        db.bind(7, r.diagnosis.main.code());
        db.bind(8, r.diagnosis.additional.code());
        db.bind(9, r.comorbidity.main.code());
        db.bind(10, r.comorbidity.additional.code());
        db.bind(11, static_cast<int>(r.type));

        if (r.type == ReferralType::MDD4) {
            db.bind(12, std::get<MDD4Data>(r.data).tooth_idx);
        }

        if (r.type == ReferralType::MH119)
        {
            auto& data = std::get<MH119Data>(r.data);
            db.bind(13, data.reason.getIndex());
            db.bind(14, data.description);
            db.bind(15, data.specCode.getIndex());
        }

        db.execute();
    }
}
