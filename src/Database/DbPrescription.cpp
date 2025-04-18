#include "DbPrescription.h"
#include "Database/Database.h"
#include "Model/Parser.h"
#include "Model/User.h"

long long DbPrescription::insert(const Prescription& p)
{

    Db db;

    db.newStatement(

        "INSERT INTO prescription ("
        "patient_rowid, lrn, nrn, date, dispensation, "
        "repeats, supplements, lpk, rzi, is_pregnant, is_breastfeeding) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, p.patient_rowid);
    db.bind(2, p.LRN);
    db.bind(3, p.NRN);
    db.bind(4, p.date.to8601());
    db.bind(5, p.dispensation.type);
    db.bind(6, static_cast<int>(p.dispensation.repeats));
    db.bind(7, p.supplements);
    db.bind(8, User::doctor().LPK);
    db.bind(9, User::practice().rziCode);
    db.bind(10, p.isPregnant);
    db.bind(11, p.isBreastFeeding);

    bool success = db.execute();

    long long rowid{ 0 };

    if (success) {
        rowid = db.lastInsertedRowID();
    }
    else {
        return rowid;
    }

    for (auto& m : p.medicationGroup)
    {
        db.newStatement(

            "INSERT INTO medication ("
            "prescription_rowid, numMed_rowid, is_form, quantity, "
            "priority, substitution, notes, dosage, from_date, to_date) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)"
        );

        db.bind(1, rowid);
        db.bind(2, m.getNumenclatureKey());
        db.bind(3, m.byForm);
        db.bind(4, static_cast<int>(m.quantity));
        db.bind(5, m.priority);
        db.bind(6, m.substitution);
        db.bind(7, m.note);
        db.bind(8, Parser::write(m.dosage));

        if (m.dosePeriod)
        {
            db.bind(9, m.dosePeriod->from.to8601());
            db.bind(10, m.dosePeriod->to.to8601());
        }
        else
        {
            db.bindNull(9);
            db.bindNull(10);
        }

        db.execute();
    }

    return rowid;
}

Prescription DbPrescription::get(long long rowid)
{

    std::string query =
        "SELECT "
        "prescription.rowid,"
        "prescription.patient_rowid,"
        "prescription.lrn,"
        "prescription.nrn,"
        "prescription.date,"
        "prescription.dispensation,"
        "prescription.repeats,"
        "prescription.supplements,"
        "prescription.is_pregnant,"
        "prescription.is_breastfeeding,"
        "medication.numMed_rowid,"
        "medication.is_form,"
        "medication.quantity,"
        "medication.priority,"
        "medication.substitution,"
        "medication.notes,"
        "medication.dosage, "
        "medication.from_date, "
        "medication.to_date "

        "FROM medication LEFT JOIN prescription ON "
        "medication.prescription_rowid = prescription.rowid "
        "WHERE medication.prescription_rowid = " + std::to_string(rowid);

     Db db(query);

     Prescription p;

     bool prescriptionSet{ false };

     while(db.hasRows())
     {
         if (!prescriptionSet)
         {
             p.rowid = db.asRowId(0);
             p.patient_rowid = db.asRowId(1);
             p.LRN = db.asString(2);
             p.NRN = db.asString(3);
             p.date = db.asString(4);
             p.dispensation.type = static_cast<Dispensation::Type>(db.asInt(5));
             p.dispensation.repeats = db.asInt(6);
             p.supplements = db.asString(7);
             p.isPregnant = db.asBool(8);
             p.isBreastFeeding = db.asBool(9);

             prescriptionSet = true;
         }
         
         p.medicationGroup.emplace_back(db.asInt(10));

         auto& m = p.medicationGroup.back();
         m.byForm = db.asBool(11);
         m.quantity = db.asInt(12);
         m.priority = static_cast<Medication::Priority>(db.asInt(13));
         m.substitution = db.asBool(14);
         m.note = db.asString(15);
         m.dosage = Parser::parseDosage(db.asString(16));
         
         std::string fromDate = db.asString(17);
         if (fromDate.size())
         {
             m.dosePeriod = DosePeriod{
                 .from = fromDate,
                 .to = db.asString(18),
             };
         }
     }

     return p;
}

bool DbPrescription::update(const Prescription& p)
{
    Db db(
        "UPDATE prescription SET "
        "nrn=?,"
        "lrn=?,"
        "date=?,"
        "dispensation=?,"
        "repeats=?,"
        "supplements=?,"
        "is_pregnant=?,"
        "is_breastfeeding=? "
        "WHERE prescription.rowid=?"
    );

    db.bind(1, p.NRN);
    db.bind(2, p.LRN);
    db.bind(3, p.date.to8601());
    db.bind(4, p.dispensation.type);
    db.bind(5, p.dispensation.repeats);
    db.bind(6, p.supplements);
    db.bind(7, p.isPregnant);
    db.bind(8, p.isBreastFeeding);
    db.bind(9, p.rowid);

    bool success = db.execute();

    if (!success) return false;

    success = db.execute(
        "DELETE FROM medication WHERE prescription_rowid=" + std::to_string(p.rowid)
    );

    if (!success) return false;

    //copy-paste from the insert function
    for (auto& m : p.medicationGroup)
    {
        db.newStatement(

            "INSERT INTO medication ("
            "prescription_rowid, numMed_rowid, is_form, quantity, "
            "priority, substitution, notes, dosage, from_date, to_date) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)"
        );

        db.bind(1, p.rowid);
        db.bind(2, m.getNumenclatureKey());
        db.bind(3, m.byForm);
        db.bind(4, static_cast<int>(m.quantity));
        db.bind(5, m.priority);
        db.bind(6, m.substitution);
        db.bind(7, m.note);
        db.bind(8, Parser::write(m.dosage));

        if (m.dosePeriod)
        {
            db.bind(9, m.dosePeriod->from.to8601());
            db.bind(10, m.dosePeriod->to.to8601());
        }
        else
        {
            db.bindNull(9);
            db.bindNull(10);
        }

        success = db.execute();

       if (!success) return false;

    }

    return success;
}

bool DbPrescription::nrnExists(const std::string& nrn)
{
    if (nrn.empty()) return false;

    Db db(
        "SELECT COUNT(*) FROM prescription WHERE nrn=?"
    );

    db.bind(1, nrn);

    while(db.hasRows()) return db.asBool(0);

    return false;
}

std::vector<std::pair<long long, Medication>> DbPrescription::getTemplates()
{
    std::vector<std::pair<long long, Medication>> result;

    Db db(
        "SELECT "
        "rowid, "
        "numMed_rowid,"
        "is_form,"
        "quantity,"
        "priority,"
        "substitution,"
        "notes,"
        "dosage "
        "FROM medication_template "
    );

    while (db.hasRows())
    {
        result.emplace_back(std::make_pair(db.asRowId(0), db.asInt(1)));

        auto& m = result.back().second;
        m.byForm = db.asBool(2);
        m.quantity = db.asInt(3);
        m.priority = static_cast<Medication::Priority>(db.asInt(4));
        m.substitution = db.asBool(5);
        m.note = db.asString(6);
        m.dosage = Parser::parseDosage(db.asString(7));
    }

    return result;
}

bool DbPrescription::insertTemplate(const Medication& m)
{
    Db db(
        "INSERT INTO medication_template ("
        "numMed_rowid, is_form, quantity, "
        "priority, substitution, notes, dosage) "
        "VALUES (?,?,?,?,?,?,?)"
    );

    db.bind(1, m.getNumenclatureKey());
    db.bind(2, m.byForm);
    db.bind(3, static_cast<int>(m.quantity));
    db.bind(4, m.priority);
    db.bind(5, m.substitution);
    db.bind(6, m.note);
    db.bind(7, Parser::write(m.dosage));

    return db.execute();
}

bool DbPrescription::deleteTemplate(long long rowid)
{
    Db db("DELETE FROM medication_template WHERE rowid=?");

    db.bind(1, rowid);

    return db.execute();
}
