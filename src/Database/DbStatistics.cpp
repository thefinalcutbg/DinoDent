#include "DbStatistics.h"
#include "Database.h"
#include "Model/Dental/DentalStatistics.h"
#include "Model/User.h"
#include "Model/Dental/Procedure.h"

namespace DbStatPrv {

    /*
    std::string dateRangeToClause(const Date& from, const Date& to)
    {
        return "AND strftime('%Y-%m-%d', procedure.date) BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' ";
    }
    */
    std::string ageToClause(DentalStatistic::AgeFilter filter)
    {
        switch (filter)
        {
            case DentalStatistic::AgeFilter::None: 
                return {};
            case DentalStatistic::AgeFilter::Minor: 
                return "AND strftime('%Y/%m/%d', patient.birth, '18 Years') > strftime('%Y/%m/%d', procedure.date) ";
            case DentalStatistic::AgeFilter::Adult: 
                return "AND strftime('%Y/%m/%d', patient.birth, '18 Years') < strftime('%Y/%m/%d', procedure.date) ";
            default: return{};
        }
    }

    std::string nhifToClause(DentalStatistic::FinancingFilter filter) {

        switch (filter)
        {
            case DentalStatistic::FinancingFilter::All: 
                return {};
            case DentalStatistic::FinancingFilter::Nhif: 
                return "AND procedure.financing_source=" + std::to_string(static_cast<int>(FinancingSource::NHIF)) + " ";
            case DentalStatistic::FinancingFilter::Cash:
                return "AND procedure.financing_source!=" + std::to_string(static_cast<int>(FinancingSource::NHIF)) + " ";
            default: return {};
        }

    }

    std::string includeRowValues(const char* tableColumn, const std::vector<std::string>& tableValue)
    {
        if (tableValue.empty()) return "";

        std::string result = "AND (";

        for (int i = 0; i < tableValue.size(); i++)
        {
            result += tableColumn;
            result += "='" + tableValue[i] + "' ";

            i != tableValue.size()-1 ? result += "OR " : result += ") ";
        }

        return result;
    }

    std::string includeRowValues(const char* tableColumn, const std::vector<int>& tableValue)
    {
        if (tableValue.empty()) return "";

        std::string result = "AND (";

        for (int i = 0; i < tableValue.size(); i++)
        {
            result += tableColumn;
            result += "='" + std::to_string(tableValue[i]) + "' ";

            i != tableValue.size() - 1 ? result += "OR " : result += ") ";
        }

        return result;
    }

    std::string toothFilterToClause(DentalStatistic::ToothFilter filter)
    {
        switch (filter)
        {
            case DentalStatistic::ToothFilter::All: 
                return {};
            case DentalStatistic::ToothFilter::Temporary: 
                return "AND procedure.temporary = 1 ";
            case DentalStatistic::ToothFilter::Permanent: return 
                "AND procedure.temporary = 0 ";
            default: return {};
        }
    }

}

int DbStat::count(const DentalStatistic& stat)
{
    Db db;

    if (stat.nonTooth_specific)
    {
        std::string query =
            "SELECT COUNT(*) FROM procedure "
            "LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
            "LEFT JOIN patient ON amblist.patient_rowid = patient.rowid "
            "WHERE amblist.lpk = '" + User::doctor().LPK + "' "
            "AND amblist.rzi = '" + User::practice().rziCode + "' "
            "AND procedure.removed = 0 "
            "AND strftime('%Y', amblist.date)='" + std::to_string(stat.year) + "' " 
            ;
       
        query += DbStatPrv::ageToClause(stat.age);
        query += DbStatPrv::nhifToClause(stat.financing);

        query += DbStatPrv::includeRowValues("procedure.diagnosis", stat.diagnosisFilter);
        query += DbStatPrv::includeRowValues("procedure.code", stat.procedureCodeFilter);

        db.newStatement(query);

        while (db.hasRows()) return db.asInt(0);

        return 0;
    }

    std::string query = 
        "SELECT COUNT(at_tooth_index) FROM("
            "SELECT DISTINCT procedure.at_tooth_index, amblist.patient_rowid "
            "FROM procedure LEFT JOIN amblist on procedure.amblist_rowid = amblist.rowid  "
            "LEFT JOIN patient on amblist.patient_rowid =  patient.rowid "
            "WHERE amblist.lpk = '" + User::doctor().LPK + "' "
            "AND procedure.removed = 0 "
            "AND amblist.rzi = '" + User::practice().rziCode + "' "
            "AND strftime('%Y', amblist.date)='" + std::to_string(stat.year) + "' "
    ;     

    query += DbStatPrv::ageToClause(stat.age);
    query += DbStatPrv::toothFilterToClause(stat.tooth);
    query += DbStatPrv::nhifToClause(stat.financing);
    query += DbStatPrv::includeRowValues("procedure.diagnosis", stat.diagnosisFilter);
    query += DbStatPrv::includeRowValues("procedure.code", stat.procedureCodeFilter);

    query += ")";

    db.newStatement(query);

    while (db.hasRows()) return db.asInt(0);

    return 0;
}
