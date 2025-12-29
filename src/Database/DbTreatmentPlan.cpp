#include "DbTreatmentPlan.h"
#include "Database.h"
#include "Model/User.h"
#include "Model/Parser.h"

bool insertPlannedProcedures(Db& db, long long planRowid, std::vector<TreatmentPlan::Stage> &stages){

    db.execute("PRAGMA foreign_keys = OFF");

    db.newStatement("DELETE FROM planned_procedure WHERE treatment_plan_rowid=?");

    db.bind(1, planRowid);

    db.execute();

    for(int i = 0; i < stages.size(); i++){
        for(int j = 0; j < stages[i].plannedProcedures.size(); j++){

            auto p = stages[i].plannedProcedures[j];

            db.newStatement(R"SQL(
INSERT INTO planned_procedure (
treatment_plan_rowid,
stage,
code,
name,
icd10,
diagnosis_description,
notes,
data,
price_from,
price_to,
rowid,
sequence
) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)
ON CONFLICT(rowid) DO UPDATE SET
treatment_plan_rowid=excluded.treatment_plan_rowid,
stage=excluded.stage,
code=excluded.code,
name=excluded.name,
icd10=excluded.icd10,
diagnosis_description=excluded.diagnosis_description,
notes=excluded.notes,
data=excluded.data,
price_from=excluded.price_from,
price_to=excluded.price_to,
sequence=excluded.sequence
            )SQL");

            db.bind(1, planRowid);
            db.bind(2, i);
            db.bind(3, p.code.code());
            db.bind(4, p.name);
            db.bind(5, p.diagnosis.icd.code());
            db.bind(6, p.diagnosis.additional_descr);
            db.bind(7, p.notes);
            db.bind(8, Parser::write(p.affectedTeeth, p.param));
            db.bind(9, p.priceRange.first);
            db.bind(10, p.priceRange.second);

            if(p.rowid) db.bind(11, p.rowid);
            else db.bindNull(11);

            db.bind(12, j);

            if(!db.execute()) return false;

            if(!p.rowid){
                p.rowid = db.lastInsertedRowID();
            }
        }
    }

    db.execute("PRAGMA foreign_keys = ON");

    return true;
}

bool DbTreatmentPlan::insert(TreatmentPlan &t, long long patientRowid)
{
    Db db;

    db.newStatement("INSERT INTO treatment_plan (date, rzi, lpk, patient_rowid, status, stage_description, has_conclusion, is_completed) VALUES (?,?,?,?,?,?,?,?)");

    db.bind(1, t.date.to8601());
    db.bind(2, User::practice().rziCode);
    db.bind(3, User::doctor().LPK);
    db.bind(4, patientRowid);
    db.bind(5, Parser::write(t.teeth));
    db.bind(6, Parser::write(t.stages));
    db.bind(7, t.lastStageIsConclusion);
    db.bind(8, t.is_completed);

    if(!db.execute()) return false;

    t.rowid =  db.lastInsertedRowID();

    insertPlannedProcedures(db, t.rowid, t.stages);

    return t.rowid;

}

bool DbTreatmentPlan::update(TreatmentPlan &t, const std::vector<long long>& deletedProcedures)
{
    Db db;

    db.newStatement("UPDATE treatment_plan SET date=?, status=?, stage_description=?, has_conclusion=?, is_completed=? WHERE rowid=?");
    db.bind(1, t.date.to8601());
    db.bind(2, Parser::write(t.teeth));
    db.bind(3, Parser::write(t.stages));
    db.bind(4, t.lastStageIsConclusion);
    db.bind(5, t.is_completed);
    db.bind(6, t.rowid);

    if(!db.execute()) return false;

    for(auto rowidForDeletion : deletedProcedures){
        db.newStatement("DELETE FROM planned_procedure WHERE rowid=?");
        db.bind(1, rowidForDeletion);
        db.execute();
    }

    return insertPlannedProcedures(db, t.rowid, t.stages);
}

TreatmentPlan DbTreatmentPlan::get(long long rowid)
{
    TreatmentPlan t;

    Db db;

    db.newStatement(
        "SELECT date, lpk, status, stage_description, has_conclusion, is_completed "
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
        t.lastStageIsConclusion = db.asBool(4);
        t.is_completed = db.asBool(5);
    }

    db.newStatement(
        "SELECT stage, code, name, icd10, diagnosis_description, notes, data, price_from, price_to, rowid "
        "FROM planned_procedure "
        "WHERE treatment_plan_rowid=? "
        "ORDER BY stage ASC, rowid ASC"
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
        Parser::parse(db.asString(6), p.affectedTeeth, p.param);
        p.priceRange = {db.asDouble(7), db.asDouble(8)};
        p.rowid = db.asInt(9);
    }

    return t;
}

std::vector<Procedure> DbTreatmentPlan::getPendingProcedures(long long patientRowid, const std::set<long long>& exclude)
{
    std::vector<Procedure> result;

    std::string query = R"SQL(
    SELECT
    pp.code,
    pp.name,
    pp.icd10,
    pp.diagnosis_description,
    pp.data,
    pp.rowid,
    pp.price_to
    FROM planned_procedure pp
    JOIN treatment_plan tp ON tp.rowid = pp.treatment_plan_rowid
    WHERE tp.patient_rowid=?
    AND tp.rzi=?
    AND tp.is_completed=0
    AND pp.rowid IS NOT NULL
    AND NOT EXISTS (
    SELECT 1
    FROM "procedure" pr
    WHERE pr.planned_procedure_rowid = pp.rowid
    )
    ORDER BY tp.date ASC, pp.stage ASC, pp.seqence ASC
    )SQL";

    Db db(query);

    db.bind(1, patientRowid);
    db.bind(2, User::practice().rziCode);

    while(db.hasRows())
    {
        auto idx = db.asLongLong(5);

        if(exclude.count(idx)) continue;

        result.emplace_back();
        auto& p = result.back();

        p.date = Date::currentDate();
        p.code = db.asString(0);
        p.notes = db.asString(1);
        p.diagnosis.icd = db.asString(2);
        p.diagnosis.additional_descr = db.asString(3);
        Parser::parse(db.asString(4), p.affectedTeeth, p.param);
        p.planned_procedure_idx = idx;
        p.financingSource = db.asDouble(6) == 0 ? FinancingSource::None :
                                FinancingSource::Patient;
    }

    return result;
}

long long DbTreatmentPlan::getActiveTreatmentPlan(long long patientRowId)
{
    Db db(R"SQL(
SELECT
tp.rowid
FROM treatment_plan tp
JOIN planned_procedure pp ON tp.rowid = pp.treatment_plan_rowid
WHERE tp.patient_rowid=?
AND tp.rzi=?
AND tp.is_completed=0
ORDER BY tp.date ASC, pp.stage ASC, pp.seqence ASC
    )SQL");

    db.bind(1, patientRowId);
    db.bind(2, User::practice().rziCode);

    while(db.hasRows()){
        return db.asRowId(0);
    }

    return 0;
}

std::set<long long> DbTreatmentPlan::getCompletedProcedures(long long patientRowid)
{
    std::set<long long> result;

    Db db(R"SQL(
SELECT procedure.planned_procedure_rowid
FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid
WHERE amblist.patient_rowid = ?
AND procedure.planned_procedure_rowid NOT NULL OR
procedure.planned_procedure_rowid != 0
    )SQL");

    db.bind(1, patientRowid);

    while(db.hasRows()){
        result.insert(db.asLongLong(0));
    }

    return result;
}

long long DbTreatmentPlan::getActivePlan(long long patientRowId)
{
    Db db(R"SQL(
SELECT
tp.rowid
FROM treatment_plan tp
WHERE tp.patient_rowid=?
AND tp.rzi=?
AND tp.is_completed=0
ORDER BY tp.date ASC
    )SQL");

    db.bind(1, patientRowId);
    db.bind(2, User::practice().rziCode);

    while(db.hasRows()){
        return db.asRowId(0);
    }

    return 0;
}

std::pair<double, double> DbTreatmentPlan::getPlannedProcedurePrice(long long rowid)
{
    Db db(R"SQL(
SELECT
price_from, price_to
FROM planned_procedure
WHERE planned_procedure.rowid=?
    )SQL");

    db.bind(1, rowid);

    while(db.hasRows()){
        return{db.asDouble(0), db.asDouble(1)};
    }

    return {0,0};
}

bool DbTreatmentPlan::setAsCompleted(long long rowid)
{
    Db db("UPDATE treatment_plan SET is_completed=1 WHERE rowid=?");
    db.bind(1, rowid);
    return db.execute();
}

std::vector<TreatmentPlan::PlannedProcedure> DbTreatmentPlan::getProcedureSummary(long long rowid)
{
    std::vector<TreatmentPlan::PlannedProcedure> result;

    Db db;

    db.newStatement(
R"SQL(
SELECT
stage, code, name, icd10, diagnosis_description, notes, data, price_from, price_to, rowid,
EXISTS (
    SELECT 1
    FROM procedure
    WHERE procedure.planned_procedure_rowid = planned_procedure.rowid
    )AS is_completed
FROM planned_procedure
WHERE treatment_plan_rowid = ?
ORDER BY stage ASC, sequence ASC;
)SQL");

    db.bind(1, rowid);

    while (db.hasRows())
    {
        auto& p = result.emplace_back();

        p.code = db.asString(1);
        p.name = db.asString(2);
        p.diagnosis.icd = db.asString(3);
        p.diagnosis.additional_descr = db.asString(4);
        p.notes = db.asString(5);
        Parser::parse(db.asString(6), p.affectedTeeth, p.param);
        p.priceRange = {db.asDouble(7), db.asDouble(8)};
        p.rowid = db.asInt(9);
        p.isCompleted = db.asBool(10);
    }


    return result;
}
