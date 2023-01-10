#include "DbStatistics.h"
#include "Database.h"
#include "Model/Dental/DentalStatistics.h"

namespace DbStatPrv {

    std::string dateRangeToClause(const Date& from, const Date& to)
    {
        return "strftime('%Y-%m-%d', date) BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' ";
    }

    std::string ageToClause(DentalStatistic::AgeFilter filter)
    {
        switch (filter)
        {
            case DentalStatistic::None: return {};
            case DentalStatistic::Minor: return
                "AND strftime('%Y/%m/%d', patient.birth, '18 Years') > strftime('%Y/%m/%d', procedure.date) ";
            case DentalStatistic::Adult: return
                "AND strftime('%Y/%m/%d', patient.birth, '18 Years') < strftime('%Y/%m/%d', procedure.date) ";
        }
    }

    std::string nhifToClause(bool nhifOnly) {

        if (!nhifOnly) return {};

        return "AND procedure.nzok = 1 ";
    }

    std::string includeRowValues(const char* tableColumn, const std::vector<std::string>& tableValue)
    {
        if (tableValue.empty()) return "";

        std::string result = "AND (";

        for (int i = 0; i < tableValue.size(); i++)
        {
            result += tableColumn;
            result += "='" + tableValue[i] + "' ";

            i != tableValue.size() ? result += "OR " : result += ") ";
        }

        return result;
    }

    std::string toothFilterToClause(DentalStatistic::ToothFilter filter)
    {
        switch (filter)
        {
            case DentalStatistic::All: {};
            case DentalStatistic::Temporary: return "AND procedure.deciduous = 1 ";
            case DentalStatistic::Permanent: return "AND procedure.deciduous = 0 ";
        }
    }

}


const std::vector<std::string> DbStat::getProcedureNames(int procedureType)
{
    std::vector<std::string> result;

    Db db(
        "SELECT DISTINCT procedure.name "
        "FROM procedure "
        "WHERE procedure.type=?"
    );

    db.bind(1, procedureType);

    while (db.hasRows())
    {
        result.push_back(db.asString(0));
    }

    return result;
}

const std::vector<std::string> DbStat::getDiagnosis(int procedureType)
{
    std::vector<std::string> result;

    Db db(
        "SELECT DISTINCT procedure.diagnosis "
        "FROM procedure "
        "WHERE procedure.type = ?"
    );

    db.bind(1, procedureType);

    while (db.hasRows())
    {
        result.push_back(db.asString(0));
    }

    return result;
}



int DbStat::procedureCount(const DentalStatistic& stat, const Date& from, const Date& to)
{
    Db db;

    if (stat.procedureType == 0)
    {
        std::string query =
            "SELECT COUNT(*) FROM procedure "
            "LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
            "LEFT JOIN patient ON amblist.patient_rowid = patient.rowid "
            "WHERE procedure.type = 0 "
            ;
        query += DbStatPrv::dateRangeToClause(from, to);
        query += DbStatPrv::ageToClause(stat.age);
        query += DbStatPrv::nhifToClause(stat.nhifOnly);
        query += DbStatPrv::includeRowValues("procedure.diagnosis", stat.diagnosisFilter);
        query += DbStatPrv::includeRowValues("procedure.name", stat.procedureNameFilter);

        db.execute(query);

        while (db.hasRows()) return db.asInt(0);

        return 0;
    }

    std::string query = 
        "SELECT COUNT(tooth) FROM("
            "SELECT DISTINCT procedure.tooth, amblist.patient_rowid "
            "FROM procedure LEFT JOIN amblist on procedure.amblist_rowid = amblist.rowid  "
            "LEFT JOIN patient on amblist.patient_rowid =  patient.rowid "
            "WHERE procedure.type = " + std::to_string(stat.procedureType) + " " 
    ;     

    query += DbStatPrv::dateRangeToClause(from, to);
    query += DbStatPrv::ageToClause(stat.age);
    query += DbStatPrv::toothFilterToClause(stat.tooth);
    query += DbStatPrv::nhifToClause(stat.nhifOnly);
    query += DbStatPrv::includeRowValues("procedure.diagnosis", stat.diagnosisFilter);
    query += DbStatPrv::includeRowValues("procedure.name", stat.procedureNameFilter);

    query += "))";

    db.execute(query);

    while (db.hasRows()) return db.asInt(0);

    return 0;
}
