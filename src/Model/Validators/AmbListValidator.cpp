#include <unordered_set>
#include <set>
#include <algorithm>
#include <map>

#include "AmbListValidator.h"
#include "Database/DbProcedure.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/PackageCounter.h"
#include "Model/User.h"

AmbListValidator::AmbListValidator(const AmbList& list, const Patient& patient)
    :
    ambList(list), patient(patient), ambListDate(ambList.getDate())
{
    _error.reserve(100);

    for (auto &p : list.procedures)
    {
        if (p.isNhif())
            m_procedures.push_back(p);
    }
}

bool AmbListValidator::ambListIsValid()
{
    if (!noDuplicates()) return false;
    
    if (!isValidAccordingToDb()) return false;

    auto& teeth = ambList.teeth;

    for (auto& p : m_procedures)
    {

        if (p.tooth_idx.isValid()) //out of range guard
        {
            //checking temporary/permanent tooth requirement of the procedure
            if (!validatePermaTemp(teeth[p.tooth_idx.index], p)) return false;

            //checking if the tooth has appliable status
            if (!validateTypeToStatus(teeth[p.tooth_idx.index], p)) return false;
        }
    }

    if (!dateIsValid()) return false;

    if (!examIsFirst()) return false;

    for (auto& p : m_procedures)
    {
        if (p.tooth_idx.supernumeral && !ambList.teeth[p.tooth_idx.index][Dental::HasSupernumeral])
        {
            _error = "Съществува процедура на срвъхброен зъб, който не е добавен в статуса";
            return false;
        }
    }

    if (!ambList.isNhifSheet()) return true;

    if (patient.HIRBNo.empty())
    {
        _error = "Не е въведен номер на здравната книжка на пациента";
        return false;
    }

    if (
        User::practice().isUnfavourable() &&
        ambList.nhifData.isUnfavourable &&
        !patient.city.isUnfav()
        ) {
        _error = 
            "Населеното място на пациента не фигурира в списъка на места с неблагоприятни условия";
            return false;
    }

    for (auto& ref : ambList.referrals)
    {
        if (ref.type != ReferralType::MH119 && !ref.isSentToHIS()) {
            _error = "Амбулаторният лист съдържа направления, които не са изпратени към НЗИС";
            return false;
        }

        if (!ref.date.isTheSameMonthAs(ambListDate)) {
            _error = "Към амбулаторния лист съществува направление, което е от друг месец";
            return false;
        }

        if (ref.date < ambListDate) {
            _error = "Издаденото направление е с дата по-малка от тази на амбулаторния лист";
            return false;
        }

    }

    auto result = exceededDailyLimit();

    if (result.has_value()) {

        auto [date, minutes] = *result;

        _error = "За дата " + date.toBgStandard()
         + " дневният лимит по НЗОК е надвишен с "
         + std::to_string(minutes) + " минути. ";

        return false;
    }

    if(isNhifInWeekend()) return false;

    _error = "";
    return true;
}


std::vector<ProcedureSummary> getSummaryFromPisHistory(const std::vector<Procedure> pisHistory, Date ambListDate)
{
    std::vector<ProcedureSummary> result;

    for (auto& p : pisHistory) {

        if (p.date >= ambListDate) continue;

        result.push_back(ProcedureSummary{
                .date = p.date,
                .code = p.code.oldCode(),
                .tooth_idx = p.tooth_idx
            });

    }

    return result;
}




bool AmbListValidator::isValidAccordingToDb()
{
    if (ambList.nhifData.specification == NhifSpecificationType::Anesthesia) return true;

    std::vector<ProcedureSummary> summary;

    std::vector<ProcedureSummary> nhifHistory;

    if (patient.PISHistory.has_value())
    {
        nhifHistory = getSummaryFromPisHistory(
                    patient.PISHistory.value(), 
                    ambList.getDate()
        );
    }
   
    //getting procedures from local db, from the day after the last record in NHIF

    summary = DbProcedure::getNhifSummary(
        patient.rowid,
        ambList.rowid,
        nhifHistory.empty() ? Date{} : nhifHistory.begin()->date.tomorrow(),
        ambList.getDate()
    );

    //inserting the rest of the nhif procedures from local db
    summary.insert(summary.end(), nhifHistory.begin(), nhifHistory.end());


    typedef int ProcedureCode, Count;

    std::unordered_map<ProcedureCode, Count> currentYear;
    std::unordered_set<ToothIndex> extractedTeeth;

    for (auto& p : summary) //getting procedures of the current year;
    {
        if (p.date.year == ambListDate.year)
            currentYear[p.code] ++;

        //getting the already extracted teeth
        if (p.extr) extractedTeeth.insert(p.tooth_idx);
    }

    PackageCounter packageCounter(NhifProcedures::getPackages(ambListDate)); //creating a package counter

    for (auto& t : currentYear) //loading the procedures from the current year
        for (int i = 0; i < t.second; i++) packageCounter.insertCode(t.first);

    for (int i = 0; i < m_procedures.size(); i++) //iterrating over the ambList procedures
    {
        auto& procedure = m_procedures[i];
        
        packageCounter.insertCode(procedure.code.oldCode());

        if (!packageCounter.validate(patient.isAdult(procedure.date))) //validating max allowed per year
        {
            _error = "Надвишен лимит по НЗОК за код " + std::to_string(procedure.code.oldCode()) + "!";
            return false;
        };

        NhifProcedures::getYearLimit(procedure.code.oldCode());

        for (auto& p : summary) //validating max allowed per time period and per tooth
        {
            if (p.code != procedure.code.oldCode() || p.tooth_idx != procedure.tooth_idx) continue;

            auto yearLimit = NhifProcedures::getYearLimit(procedure.code.oldCode());

            Date minimumDate = { p.date.day, p.date.month, p.date.year + yearLimit };
         
            if (procedure.date < minimumDate)
            {
                _error = "В базата данни съществува вече манипулация с код " + std::to_string(p.code) +
                    " от преди по-малко от " + std::to_string(yearLimit) + " г.";
                return false;
            }
        }

        if (procedure.tooth_idx.index != -1 && //extraction check
            extractedTeeth.count(procedure.tooth_idx)
            )
        {
            _error = "За зъб " + procedure.tooth_idx.getNhifNumenclature() +
                + " вече съществуват данни за екстракция!";
            return false;
        }

    }

    return true;
}


bool AmbListValidator::dateIsValid()
{

   for(auto& p : ambList.procedures)
   {
        if (p.date.month != ambListDate.month || p.date.year != ambListDate.year)
        {
            _error = "Манипулациите трябва да са от един и същи месец!";
            return false;
        }

        if (p.isNhif() && NhifProcedures::isMinorOnly(p.code.oldCode()) && patient.isAdult(p.date))
        {
            _error = "Манипулация " + std::to_string(p.code.oldCode()) + " е позволена само при лица под 18 годишна възраст!";
            return false;
        }
   }
    return true;
}

bool AmbListValidator::examIsFirst()
{

    auto& procedures = ambList.procedures;

    constexpr int examCode = 101;

    auto it = std::find_if(procedures.begin(), procedures.end(),
        [&](const Procedure& p){
            return p.isNhif() && p.code.oldCode() == examCode;
        });

    Date examDate;

    if (it != procedures.end()){
        examDate = it->date;
    }

    for (auto& p : procedures)
    {
        if (p.isNhif() && p.code.oldCode() != examCode && p.date < examDate)
        {
            _error = "Датата на манипулация " + std::to_string(p.code.oldCode()) + " е по-малка от датата на прегледа!";
            return false;
        }
    }

    return true;
}

bool AmbListValidator::isNhifInWeekend()
{
    if (!User::settings().nhifWeekendCheck) return false;

    for (auto& p : m_procedures)
    {
        if (p.date.isWeekend())
        {
            _error = "Манипулация " + std::to_string(p.code.oldCode()) + " e извършена в почивен ден";
            return true;
        }
    }

    for (auto& ref : ambList.referrals)
    {
        if (ref.date.isWeekend())
        {
            _error = std::string(ref.getTypeAsString()) + " №" + std::to_string(ref.number) + " e издадено в почивен ден";
            return true;
        }
    }

    return false;
}

std::optional<std::pair<Date, int>> AmbListValidator::exceededDailyLimit()
{
    if (!User::settings().nhifDailyLimitCheck) return {};

    constexpr int maxLimit = 360;

    auto dontSum = [](int code)->bool
    {
        constexpr int toPass[] = { 333, 832, 833 };

        for (auto val : toPass) {
            if (code == val) return true;
        }

        return false;
    };


    std::map<Date, int> dates_minutes;

    //getting the dates and the corresponding duration

    for (auto& p : m_procedures) {

        if (p.financingSource != FinancingSource::NHIF) continue;

       if (dontSum(p.code.oldCode())) continue;

        int minutesSum = NhifProcedures::getDuration(p.code.oldCode());

        if (dates_minutes.count(p.date)) {
            dates_minutes[p.date] += minutesSum;

        }
        else {
            dates_minutes[p.date] = minutesSum;
        }
    }


    //adding the additional duration from other nhif procedures in the dadabase

    for (auto [date, minutesSum] : dates_minutes)
    {
        auto codes = DbProcedure::getDailyNhifProcedures(date, ambList.rowid);

        for (auto code : codes)
        {
            if (dontSum(code)) continue;

            int minutes = NhifProcedures::getDuration(code);

             minutesSum += minutes;
        }

        if (minutesSum > maxLimit) {
            return std::make_pair(date, minutesSum - maxLimit);
        }

    }

    return {};
}

template<>
struct std::hash<std::pair<int, int>>
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const noexcept {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

bool AmbListValidator::noDuplicates()
{
    typedef int Tooth, Code;

    std::unordered_set<std::pair<Tooth, Code>> toothSet;
    
    for (auto& p : m_procedures)
    {
        auto pair = std::make_pair(std::hash<ToothIndex>()(p.tooth_idx), p.code.oldCode());

        if (toothSet.count(pair))
        {
            p.tooth_idx.index != -1 ?
            _error = "За зъб " + p.tooth_idx.getNhifNumenclature() +
                " манипулация с код " + std::to_string(p.code.oldCode()) + " е добавена повече от веднъж"
            :
            _error = "Направили сте 2 еднакви манипулации с код " + std::to_string(p.code.oldCode());

            return false;
        }

        toothSet.insert(pair);
    }

    return true;
}

bool AmbListValidator::validateTypeToStatus(const Tooth& t, const Procedure& p)
{
    if (p.tooth_idx.supernumeral) return true;

    std::string toothNum = ToothUtils::getNomenclature(t.index(), t[Dental::Temporary]);
    std::string code = std::to_string(p.code.oldCode());

    switch (p.code.type())
    {
        case ProcedureType::obturation:
        {
            bool statusMissing
            {
                t[Dental::Missing] ||
                t[Dental::Implant] ||
                (
                !t[Dental::Restoration] &&
                !t[Dental::Caries] &&
                !t[Dental::Pulpitis] &&
                !t[Dental::ApicalLesion] &&
                !t[Dental::Root] &&
                !t[Dental::Fracture]
                )
            };

            if (statusMissing)
            {
                _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. "
                    "Валидните статуси включват кариес, обтурация, пулпит, периодонтит, корен или фрактура.";
                return false;
            }
            break;
        }

        case ProcedureType::endo:
        {

            bool statusMissing
            {
                t[Dental::Missing] ||
                t[Dental::Implant] ||
                (
                !t[Dental::Pulpitis] &&
                !t[Dental::ApicalLesion] &&
                !t[Dental::Fracture]
                )
            };


            if (statusMissing)
            {
                _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. " +
                    "Валидните статуси включват пулпит, периодонтит или фрактура.";
                return false;
            }
            break;
        }

         case ProcedureType::extraction:
         {

            bool statusMissing 
            {
                t[Dental::Missing] ||
                (
                !t[Dental::Pulpitis] &&
                !t[Dental::Periodontitis] &&
                !t[Dental::Fracture] &&
                !t[Dental::HasSupernumeral] &&
                !t[Dental::Implant] &&
                !t[Dental::ApicalLesion] &&
                !t[Dental::Root] &&
                !t[Dental::Mobility] 
                 )
            };

        if (statusMissing)
        {
            _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. "
                "Валидните статуси включват периодонтит, корен, фрактура, пулпит, пародонтит, подвижност, имплант, свръхброен или временен зъб.";
            return false;
        }
        break;

    }

        default: break;
    }

    return true;
}

bool AmbListValidator::validatePermaTemp(const Tooth& tooth, const Procedure& p)
{
    if (p.tooth_idx.supernumeral) return true;

    bool temp = tooth[Dental::Temporary];

    if (NhifProcedures::isTempOnly(p.code.oldCode()) && !temp)
    {
        _error = "Манипулация "+ std::to_string(p.code.oldCode()) + " на зъб " + ToothUtils::getNomenclature(p.tooth_idx.index, temp) + " е позволена само при временни зъби";
        return false;
    }
    
    if (NhifProcedures::isPermaOnly(p.code.oldCode()) && temp)
    {
        _error = "Манипулация " + std::to_string(p.code.oldCode()) + " на зъб " + ToothUtils::getNomenclature(p.tooth_idx.index, temp) + " е позволена само при постоянни зъби";
        return false;
    }

    return true;
}



const std::string& AmbListValidator::getErrorMsg()
{
    return _error;
}
