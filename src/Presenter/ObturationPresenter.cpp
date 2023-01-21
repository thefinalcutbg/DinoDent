#include "ObturationPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/ToothUtils.h"
#include "View/SubWidgets/RangeWidget.h"

ObturationPresenter::ObturationPresenter(const std::vector<Tooth*>& selectedTeeth) :
    AbstractSubPresenter(ProcedureType::obturation),
    selectedTeeth(selectedTeeth)
{
    if (selectedTeeth.size() == 1)
    {
        m_diagnosis = getDiagnosis(*selectedTeeth.front());

    }
}

void ObturationPresenter::setAdditionalTemplateParameters()
{
    if (!selectedTeeth.size())
    {
        view->setErrorMsg("Изберете поне един зъб");
        return;
    }

    if (selectedForFirstTime)
    {
        view->surfaceSelector()->setData(ProcedureObtData{
            autoSurfaces(*selectedTeeth.at(0)), false
            }
        );

        selectedForFirstTime = false;
    }

    view->setCurrentPresenter(this);

    view->setLayout(ICommonFields::Restoration, true);

    view->surfaceSelector()->setInputValidator(&surf_validator);

}

bool ObturationPresenter::additionalValidation()
{
    if(selectedTeeth.empty()) return false;

    return view->surfaceSelector()->validateInput();
    /*
    if (!view->surfaceSelector()->isValid())
    {
        return ModalDialogBuilder::askDialog("Резултатът от манипулацията няма да бъде приложен към статуса, "
            "тъй като не сте избрали повърхност. Желаете ли да продължите въпреки това?");
    }

    return true;
    */
}

std::vector<Procedure> ObturationPresenter::getProcedures()
{
    std::vector<Procedure> procedures;
    procedures.reserve(selectedTeeth.size());

    for (auto& tooth : selectedTeeth) {
        procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
        procedures.back().result = view->surfaceSelector()->getData();
        procedures.back().tooth = tooth->index;
        procedures.back().temp = tooth->temporary.exists();
    }

    return procedures;
}


std::string ObturationPresenter::getDiagnosis(const Tooth& tooth)
{
    bool secondaryCaries = false ;
  
    for (int i = 0; i < 6; i++)		//checking if somewhere obturation is present also, returning secondary caries
    {
        if (tooth.caries.exists(i) && tooth.obturation.exists(i))
        {
            secondaryCaries = true;
        }
    }

    std::array<bool, 7> existing
    {
            tooth.fracture.exists(),
            secondaryCaries,
            tooth.caries.exists(),
            tooth.endo.exists(),
            tooth.pulpitis.exists() && tooth.lesion.exists(),
            tooth.root.exists(),
            tooth.obturation.exists()
    };

    std::array<std::string, 7> diagnosis
    {
        "Фрактура",
        "Вторичен кариес",
        "Кариес",
        "Status post devital.",
        "Преендодонтско изграждане",
        "Изграждане на корен за корона",
        "Дефектна обтурация"
    };

    for (int i = 0; i < 7; i++)
    {
        if (existing[i]) {
            return diagnosis[i];
        }
    }

    return std::string{};
}

std::array<bool, 6> ObturationPresenter::autoSurfaces(const Tooth& tooth)
{
    std::array<bool, 6> surfaces{ 0,0,0,0,0,0 };

    for (int i = 0; i < 6; i++)
    {
        surfaces[i] = tooth.caries.exists(i);
    }

    if (tooth.root.exists())
    {
        surfaces = { 1, 1, 1, 1, 1, 1 };
    }

    if (tooth.endo.exists())
    {
        if (ToothUtils::getToothType(tooth.index) == ToothType::Frontal)
        {
            surfaces[Surface::Lingual] = true;
        }
        else surfaces[Surface::Occlusal] = true;
    }

    if (tooth.fracture.exists())
    {
        if (ToothUtils::getToothType(tooth.index) == ToothType::Frontal)
        {
            surfaces[Surface::Occlusal] = true;
        }
        else surfaces[Surface::Lingual] = true;
    }
    else if (tooth.obturation.exists() && !tooth.caries.exists())
    {
        for (int i = 0; i < 6; i++)
        {
            if(tooth.obturation.exists(i))
                surfaces[i] = true;
        }
    }

    return surfaces;
}
