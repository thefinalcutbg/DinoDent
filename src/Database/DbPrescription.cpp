#include "DbPrescription.h"
#include "Database/Database.h"
#include "Model/Parser.h"
#include "Model/User.h"
#include <qdebug.h>
long long DbPrescription::insert(const Prescription& p)
{

    Db db;

   bool success = db.execute(

        "INSERT INTO prescription ("
            "patient_rowid, lrn, nrn, date, dispensation, "
            "repeats, supplements, lpk, rzi ) VALUES (" +

            std::to_string(p.patient_rowid) + "," + 
            "'" + p.LRN + "'," +
            "'" + p.NRN + "'," +
            "'" + p.date.to8601() + "'," +
                  std::to_string(p.dispensation.type) + "," +
                  std::to_string(p.dispensation.repeats) + "," +
            "'" + p.supplements + "',"
            "'" + User::doctor().LPK + "',"
            "'" + User::practice().rziCode + "'"

        + ")"
    );

    long long rowid{ 0 };

    if (success) {
        rowid = db.lastInsertedRowID();
    }
    else {
        return rowid;
    }


    for (auto& m : p.medicationGroup)
    {
        db.execute(

            "INSERT INTO medication ("
            "prescription_rowid, numMed_rowid, is_form, quantity, "
            "priority, substitution, notes, dosage) VALUES (" +

                std::to_string(rowid) + "," +
                std::to_string(m.getNumenclatureKey()) + "," +
                std::to_string(m.byForm) + "," +
                std::to_string(m.quantity) + "," +
                std::to_string(m.priority) + "," +
                std::to_string(m.substitution) + "," +
                "'"+ m.note + "'," +
                "'"+ Parser::write(m.dosage) + "'"
            +")"
        );

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
        "medication.numMed_rowid,"
        "medication.is_form,"
        "medication.quantity,"
        "medication.priority,"
        "medication.substitution,"
        "medication.notes,"
        "medication.dosage "

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

             prescriptionSet = true;
         }
         
         p.medicationGroup.emplace_back(db.asInt(8));

         auto& m = p.medicationGroup.back();
         m.byForm = db.asBool(9);
         m.quantity = db.asInt(10);
         m.priority = static_cast<Medication::Priority>(db.asInt(11));
         m.substitution = db.asBool(12);
         m.note = db.asString(13);
         m.dosage = Parser::parseDosage(db.asString(14));
     }

     return p;
}

bool DbPrescription::update(const Prescription& p)
{
    Db db;

    

    bool success = db.execute(

        "UPDATE prescription SET "
        "nrn='" + p.NRN + "',"
        "date='" + p.date.to8601() + "',"
        "dispensation=" + std::to_string(p.dispensation.type) + "," +
        "repeats=" + std::to_string(p.dispensation.repeats) + ","
        "supplements='" + p.supplements + "'"

        "WHERE prescription.rowid=" + std::to_string(p.rowid)
    );


    success = db.execute(
        "DELETE FROM medication WHERE prescription_rowid=" + std::to_string(p.rowid)
    );

    if (!success) return false;

    //copy-paste from the insert function
    for (auto& m : p.medicationGroup)
    {
       success =  db.execute(

            "INSERT INTO medication ("
            "prescription_rowid, numMed_rowid, is_form, quantity, "
            "priority, substitution, notes, dosage) VALUES (" +

            std::to_string(p.rowid) + "," +
            std::to_string(m.getNumenclatureKey()) + "," +
            std::to_string(m.byForm) + "," +
            std::to_string(m.quantity) + "," +
            std::to_string(m.priority) + "," +
            std::to_string(m.substitution) + "," +
            "'" + m.note + "'," +
            "'" + Parser::write(m.dosage) + "'"
            + ")"
        );

       if (!success) return false;

    }

    return success;
}
