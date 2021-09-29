#include "ObturationPresenter.h"
#include "View/ErrorMessage.h"
#include "Model/Tooth/ToothUtils.h"

ObturationPresenter::ObturationPresenter(const std::vector<Tooth*>& selectedTeeth) :
    TeethMPresenter(selectedTeeth),
    view(nullptr)
{}

void ObturationPresenter::setView(IObturationView* view)
{
    this->view = view;

    view->surfaceSelector()->setInputValidator(&surf_validator);

    if (selectedTeeth->size() == 1)
    {
        diagnosis = autoDiagnosis(*selectedTeeth->at(0));
        view->surfaceSelector()->setSurfaces(autoSurfaces(*selectedTeeth->at(0)));

    }
}

void ObturationPresenter::setManipulationTemplate(const ProcedureTemplate& m)
{
    common_view->set_hidden(noTeethSelected);
    view->set_hidden(noTeethSelected);

    if (noTeethSelected) return;

    GeneralMPresenter::setManipulationTemplate(m);

    //no interface for setting material only :(
    auto data = view->getData();
    data.data.material = m.material;
    view->setData(data);
}

bool ObturationPresenter::isValid()
{
    if (!TeethMPresenter::isValid()) return false;

    view->surfaceSelector()->validateInput();

    if (!view->surfaceSelector()->isValid())
    {
        return questionDialog("Резултатът от манипулацията няма да бъде приложен към статуса, "
            "тъй като не сте избрали повърхност. Желаете ли да продължите въпреки това?");
    }

    return true;
}

Result ObturationPresenter::getResult()
{
    return view->getData();
}


std::string ObturationPresenter::autoDiagnosis(const Tooth& tooth)
{
    bool secondaryCaries = false ;
    std::string cariesDiagnosis;


    for (int i = 0; i < 6; i++)		//checking if somewhere obturation is present also, returning secondary caries
    {
        if (tooth.caries.exists(i) && tooth.obturation.exists(i))
        {
            secondaryCaries = true;
        }
    }

    for (int i = 0; i < 6; i++)		//getting the diagnosis of the first caries found
    {
        if (tooth.caries.exists(i)) {
            cariesDiagnosis = tooth.caries[i].data.getDiagnosisName();
        }
    }

    std::array<bool, 7> existing
    {
            secondaryCaries,
            tooth.caries.exists(),
            tooth.endo.exists(),
            tooth.pulpitis.exists() && tooth.lesion.exists(),
            tooth.fracture.exists(),
            tooth.root.exists(),
            tooth.obturation.exists()
    };

    std::array<std::string, 7> diagnosis
    {
        "Вторичен кариес",
        cariesDiagnosis,
        "Status post devital.",
        "Преендодонтско изграждане",
        tooth.fracture.data.getDiagnosisName(),
        tooth.root.data.getDiagnosisName(),
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
