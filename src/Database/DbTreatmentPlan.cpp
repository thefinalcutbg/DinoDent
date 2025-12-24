#include "DbTreatmentPlan.h"
#include "Database.h"
#include "Model/User.h"
#include "Model/Parser.h"
#include "View/ModalDialogBuilder.h"

bool insertPlannedProcedures(Db& db, long long planRowid, std::vector<TreatmentPlan::Stage> &stages){

    long long uniqueId = 0;

    db.newStatement("SELECT id FROM planned_procedure ORDER BY id DESC LIMIT 1");
        ModalDialogBuilder::showMultilineDialog(db.getPreparedStatement());
    while(db.hasRows()){
        uniqueId = db.asLongLong(0)+1;
    }

    db.newStatement("DELETE FROM planned_procedure WHERE treatment_plan_rowid=?");

    db.bind(1, planRowid);

    db.execute();

    for(int i = 0; i < stages.size(); i++){
        for(auto& p : stages[i].plannedProcedures){

            if(!p.id) p.id = uniqueId++;

            db.newStatement("INSERT INTO planned_procedure (treatment_plan_rowid, stage, code, name, icd10, diagnosis_description, notes, data, price_from, price_to, id) VALUES (?,?,?,?,?,?,?,?,?,?,?)");

            db.bind(1, planRowid);
            db.bind(2, i);
            db.bind(3, p.code.code());
            db.bind(4, p.name);
            db.bind(5, p.diagnosis.icd.code());
            db.bind(6, p.diagnosis.additional_descr);
            db.bind(7, p.notes);
            db.bind(8, Parser::write(p));
            db.bind(9, p.priceRange.first);
            db.bind(10, p.priceRange.second);
            db.bind(11, p.id);

            if(!db.execute()) return false;
        }
    }

    return true;
}

bool DbTreatmentPlan::insert(TreatmentPlan &t, long long patientRowid)
{
    Db db;

    db.newStatement("INSERT INTO treatment_plan (date, rzi, lpk, patient_rowid, status, stage_description) VALUES (?,?,?,?,?,?)");

    db.bind(1, t.date.to8601());
    db.bind(2, User::practice().rziCode);
    db.bind(3, User::doctor().LPK);
    db.bind(4, patientRowid);
    db.bind(5, Parser::write(t.teeth));
    db.bind(6, Parser::write(t.stages));

    if(!db.execute()) return false;

    t.rowid =  db.lastInsertedRowID();

    insertPlannedProcedures(db, t.rowid, t.stages);

    return t.rowid;

}

bool DbTreatmentPlan::update(TreatmentPlan &t)
{
    Db db;

    db.newStatement("UPDATE treatment_plan SET date=?, status=?, stage_description=? WHERE rowid=?");
    db.bind(1, t.date.to8601());
    db.bind(2, Parser::write(t.teeth));
    db.bind(3, Parser::write(t.stages));
    db.bind(4, t.rowid);

    if(!db.execute()) return false;

    return insertPlannedProcedures(db, t.rowid, t.stages);
}

TreatmentPlan DbTreatmentPlan::get(long long rowid)
{
    TreatmentPlan t;

    Db db;

    db.newStatement(
        "SELECT date, lpk, status, stage_description "
        "FROM treatment_plan "
        "WHERE rowid=?"
    );

    db.bind(1, rowid);

    while(db.hasRows()){
        t.rowid = rowid;
        t.date = db.asString(0);
        t.LPK = db.asString(1);
        Parser::parse(db.asString(2), t.teeth);
        Parser::parse(db.asString(3), t.stages); //creating all the stages
    }

    db.newStatement(
        "SELECT stage, code, name, icd10, diagnosis_description, notes, data, price_from, price_to, id "
        "FROM planned_procedure "
        "WHERE treatment_plan_rowid=? "
        "ORDER BY rowid ASC"
        );

    db.bind(1, rowid);

    while (db.hasRows())
    {
        int stageIdx = db.asInt(0);

        auto& p = t.stages[stageIdx].plannedProcedures.emplace_back();


        p.code = db.asString(1);
        p.name = db.asString(2);
        p.diagnosis.icd = db.asString(3);
        p.diagnosis.additional_descr = db.asString(4);
        p.notes = db.asString(5);
        Parser::parse(db.asString(6), p);
        p.priceRange = {db.asDouble(7), db.asDouble(8)};
        p.id = db.asInt(9);
    }

    return t;
}

std::vector<TreatmentPlan::PlannedProcedure> DbTreatmentPlan::getPendingProcedures(long long patientRowid)
{
    std::vector<TreatmentPlan::PlannedProcedure> result;

    return result;
}
