﻿#include <unordered_set>

#include "AmbListValidator.h"
#include "../Presenter/TabPresenter/ListInstance.h"
#include "Procedure/MasterNZOK.h"
#include "Tooth/ToothUtils.h"
#include "Model/Procedure/PackageCounter.h"

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


AmbListValidator::AmbListValidator(const ListInstance& list)
    :
    ambList(list.amb_list), patient(*list.patient.get())
{
    _error.reserve(100);
    for (auto &p : list.amb_list.procedures)
    {
        if (p.nzok)
            m_procedures.push_back(p);
    }
}


bool AmbListValidator::ambListIsValid()
{
    auto& teeth = ambList.teeth;
    auto& procedures = m_procedures;

    if (!noDuplicates()) return false;

    for (auto& p : procedures)
    {

        if (!dateIsValid(p)) return false;

        if (p.tooth != -1) //out of range guard
        {
            //checking temporary/permanent tooth requirement of the procedure
            if (!validatePermaTemp(teeth[p.tooth], p)) return false;

            //checking if the tooth has appliable status
            if (!validateTypeToStatus(teeth[p.tooth], p)) return false;
        }
    }

    if (!isValidAccordingToDb()) return false;

    _error = "";
    return true;
}

bool AmbListValidator::isValidAccordingToDb()
{
    auto summary = _db.getSummary(patient.id, ambList.id); //getting all procedures;

    typedef int Code, Count, Tooth;
    typedef bool Temporary;
    std::unordered_map<Code, Count> currentYear;
    std::unordered_set<std::pair<Tooth, Temporary>, pair_hash> extractedTeeth;

    for (auto& p : summary) //getting procedures of the current year;
    {
        if (p.date.year == ambList.date.year)
            currentYear[p.code] ++;

        //getting the already extracted teeth
        if (p.code == 508) extractedTeeth.insert(std::make_pair(p.tooth, true));
        else if (p.code == 509 || p.code == 510) extractedTeeth.insert(std::make_pair(p.tooth, false));
    }

    PackageCounter packageCounter(MasterNZOK::instance().getPackages(ambList.date)); //creating a package counter

    for (auto& t : currentYear) //loading the procedures from the current year
        for (int i = 0; i < t.second; i++) packageCounter.insertCode(t.first);

    for (int i = 0; i < m_procedures.size(); i++) //iterrating over the ambList procedures
    {
        auto& procedure = m_procedures[i];
        
        packageCounter.insertCode(procedure.code);

        if (!packageCounter.validate(patient.isAdult(procedure.date), ambList.pregnancy)) //validating max allowed per year
        {
            _error = "Надвишен лимит по НЗОК за код " + std::to_string(procedure.code) + "!";
            return false;
        };

        MasterNZOK::instance().getYearLimit(procedure.code);

        for (auto& p : summary) //validating max allowed per time period and per tooth
        {
            if (p.code != procedure.code || p.tooth != procedure.tooth) continue;

            auto yearLimit = MasterNZOK::instance().getYearLimit(procedure.code);

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

bool AmbListValidator::dateIsValid(const Procedure& p)
{
    if (p.date < ambList.date || p.date > ambList.date.getMaxDateOfMonth())
    {
        _error = "Датата на манипулация " + std::to_string(p.code) + " е невалидна по спрямо на датата на амбулаторния лист";
        return false;
    }

    if (p.nzok && MasterNZOK::instance().isMinorOnly(p.code) && patient.isAdult(p.date))
    {
        _error = "Манипулация " + std::to_string(p.code) + " е позволена само при лица под 18 годишна възраст!";
        return false;
    }

    return true;
}


bool AmbListValidator::noDuplicates()
{
    typedef int Tooth, Code;

    std::unordered_set<std::pair<Tooth, Code>, pair_hash> tooth_set;

    for (auto& p : m_procedures)
    {
        auto pair = std::make_pair(p.tooth, p.code);

        if (tooth_set.count(pair))
        {
            p.tooth != -1 ?
            _error = "За зъб " + ToothUtils::getNomenclature(ambList.teeth[p.tooth]) +
                " манипулация с код " + std::to_string(p.code) + " е добавена повече от веднъж"
            :
            _error = "Направили сте 2 еднакви манипулации с код " + std::to_string(p.code);

            return false;
        }

        tooth_set.insert(pair);
    }

    return true;
}

bool AmbListValidator::validateTypeToStatus(const Tooth& t, const Procedure& p)
{

    std::string toothNum = ToothUtils::getNomenclature(t);
    std::string code = std::to_string(p.code);

    switch (p.type)
    {
        case ProcedureType::obturation:
        {
            bool statusMissing
            {
                t.extraction.exists() ||
                t.implant.exists() ||
                (
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

    if (MasterNZOK::instance().isTempOnly(p.code) && !temp)
    {
        _error = "Манипулация "+ std::to_string(p.code) + " на зъб " + ToothUtils::getNomenclature(p.tooth, temp) + " е позволена само при временни зъби";
        return false;
    }
    
    if (MasterNZOK::instance().isPermaOnly(p.code) && temp)
    {
        _error = "Манипулация " + std::to_string(p.code) + " на зъб " + ToothUtils::getNomenclature(p.tooth, temp) + " е позволена само при постоянни зъби";
        return false;
    }

    return true;
}



const std::string& AmbListValidator::getErrorMsg()
{
    return _error;
}