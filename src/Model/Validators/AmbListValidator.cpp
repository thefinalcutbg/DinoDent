#include <unordered_set>
#include <algorithm>
#include "AmbListValidator.h"
#include "Database/DbProcedure.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/PackageCounter.h"

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


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
    if (ambList.nrn.size() && !ambList.his_updated) {
        _error = "Амбулаторният лист е редактиран, но промените не са отразени в НЗИС";
        return false;
    }

    for (auto& p : m_procedures)
    {
        if (p.hyperdontic && !ambList.teeth[p.tooth].hyperdontic)
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


    auto& teeth = ambList.teeth;
    auto& procedures = m_procedures;

    if (!noDuplicates()) return false;

    if (!dateIsValid()) return false;

    if (!examIsFirst()) return false;


    for (auto& p : procedures)
    {

        if (p.date.isWeekend())
        {
            _error = "Манипулация " + std::to_string(p.code.oldCode()) + " не може да бъде извършена в почивен ден";
                return false;
        }

        if (p.tooth != -1) //out of range guard
        {
            //checking temporary/permanent tooth requirement of the procedure
            if (!validatePermaTemp(teeth[p.tooth], p)) return false;

            //checking if the tooth has appliable status
            if (!validateTypeToStatus(teeth[p.tooth], p)) return false;
        }
    }

    for (auto& ref : ambList.referrals)
    {
        if (ref.date.isWeekend())
        {
            _error = std::string(ref.getTypeAsString()) + " №" + std::to_string(ref.number) + " не може да бъде издадено в почивен ден";
            return false;
        }
    }

    if (!isValidAccordingToDb()) return false;

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
                .tooth = p.tooth,
                .temp = p.temp,
                .extr = p.code.type() == ProcedureType::extraction,
                .hyperdontic = p.hyperdontic,
            });

    }

    return result;
}

bool AmbListValidator::isValidAccordingToDb()
{
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


    typedef int Code, Count, Tooth;
    typedef bool Temporary;
    std::unordered_map<Code, Count> currentYear;
    std::unordered_set<std::pair<Tooth, Temporary>, pair_hash> extractedTeeth;

    for (auto& p : summary) //getting procedures of the current year;
    {
        if (p.date.year == ambListDate.year)
            currentYear[p.code] ++;

        //getting the already extracted teeth
        if (p.extr) extractedTeeth.insert(std::make_pair(p.tooth, p.temp));
    }

    PackageCounter packageCounter(NhifProcedures::getPackages(ambListDate)); //creating a package counter

    for (auto& t : currentYear) //loading the procedures from the current year
        for (int i = 0; i < t.second; i++) packageCounter.insertCode(t.first);

    for (int i = 0; i < m_procedures.size(); i++) //iterrating over the ambList procedures
    {
        auto& procedure = m_procedures[i];
        
        packageCounter.insertCode(procedure.code.oldCode());

        if (ambList.nhifData.specification != NhifSpecification::Anesthesia &&
            !packageCounter.validate(patient.isAdult(procedure.date), ambList.procedures.hasPregnancy())) //validating max allowed per year
        {
            _error = "Надвишен лимит по НЗОК за код " + std::to_string(procedure.code.oldCode()) + "!";
            return false;
        };

        NhifProcedures::getYearLimit(procedure.code.oldCode());

        for (auto& p : summary) //validating max allowed per time period and per tooth
        {
            if (p.code != procedure.code.oldCode() || p.tooth != procedure.tooth) continue;

            auto yearLimit = NhifProcedures::getYearLimit(procedure.code.oldCode());

            Date date = { p.date.day, p.date.month, p.date.year + yearLimit };
         
            if (procedure.date < date)
            {
                _error = "В базата данни съществува вече манипулация с код " + std::to_string(p.code) +
                    " от преди по-малко от " + std::to_string(yearLimit) + " г.";
                return false;
            }
        }

        if (procedure.tooth != -1 && //extraction check
            extractedTeeth.count
            (std::make_pair(procedure.tooth, ambList.teeth[procedure.tooth].temporary.exists())))
        {
            _error = "За зъб " + ToothUtils::getNomenclature(ambList.teeth[procedure.tooth])
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
        [&examCode](const Procedure& p){
            return p.isNhif() && p.code.oldCode() == examCode;
        });

    if (it != procedures.end()){
        ambListDate = it->date;
    }

    for (auto& p : procedures)
    {
        if (p.isNhif() && p.code.oldCode() != examCode && p.date < ambListDate)
        {
            _error = "Датата на манипулация " + std::to_string(p.code.oldCode()) + " е по-малка от датата на прегледа!";
            return false;
        }
    }

    return true;
}

bool AmbListValidator::noDuplicates()
{
    typedef int Tooth, Code;

    std::unordered_set<std::pair<Tooth, Code>, pair_hash> tooth_set;

    for (auto& p : m_procedures)
    {
        auto tooth = p.hyperdontic ? p.tooth + 80 : p.tooth;

        auto pair = std::make_pair(tooth, p.code.oldCode());

        if (tooth_set.count(pair))
        {
            p.tooth != -1 ?
            _error = "За зъб " + ToothUtils::getNhifNumber(p.tooth, p.temp, p.hyperdontic ) +
                " манипулация с код " + std::to_string(p.code.oldCode()) + " е добавена повече от веднъж"
            :
            _error = "Направили сте 2 еднакви манипулации с код " + std::to_string(p.code.oldCode());

            return false;
        }

        tooth_set.insert(pair);
    }

    return true;
}

bool AmbListValidator::validateTypeToStatus(const Tooth& t, const Procedure& p)
{

    std::string toothNum = ToothUtils::getNomenclature(t);
    std::string code = std::to_string(p.code.oldCode());

    switch (p.code.type())
    {
        case ProcedureType::obturation:
        {
            bool statusMissing
            {
                t.extraction.exists() ||
                t.implant.exists() ||
                (
                !t.obturation.exists() &&
                !t.caries.exists() &&
                !t.pulpitis.exists() &&
                !t.lesion.exists() &&
                !t.root.exists() &&
                !t.fracture.exists()
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
                t.extraction.exists() ||
                t.implant.exists() ||
                (
                !t.pulpitis.exists() &&
                !t.lesion.exists() &&
                !t.fracture.exists()
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
                t.extraction.exists() ||
                (
                !t.pulpitis.exists() &&
                !t.periodontitis.exists() &&
                !t.fracture.exists() &&
                !t.hyperdontic.exists() &&
                !t.implant.exists() &&
                !t.temporary.exists() &&
                !t.lesion.exists() &&
                !t.root.exists() &&
                !t.mobility.exists() 
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

    bool temp = tooth.temporary.exists();

    if (NhifProcedures::isTempOnly(p.code.oldCode()) && !temp)
    {
        _error = "Манипулация "+ std::to_string(p.code.oldCode()) + " на зъб " + ToothUtils::getNomenclature(p.tooth, temp) + " е позволена само при временни зъби";
        return false;
    }
    
    if (NhifProcedures::isPermaOnly(p.code.oldCode()) && temp)
    {
        _error = "Манипулация " + std::to_string(p.code.oldCode()) + " на зъб " + ToothUtils::getNomenclature(p.tooth, temp) + " е позволена само при постоянни зъби";
        return false;
    }

    return true;
}



const std::string& AmbListValidator::getErrorMsg()
{
    return _error;
}
