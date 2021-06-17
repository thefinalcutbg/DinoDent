#include "ObturationPresenter.h"


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

void ObturationPresenter::setManipulationTemplate(const ManipulationTemplate& m)
{
    common_view->set_hidden(noTeethSelected);
    view->set_hidden(noTeethSelected);

    if (noTeethSelected) return;

    GeneralMPresenter::setManipulationTemplate(m);
    view->material()->set_Text(m.material);
}

bool ObturationPresenter::isValid()
{
    if (!TeethMPresenter::isValid()) return false;

    view->surfaceSelector()->validateInput();

    if (!view->surfaceSelector()->isValid())
    {
        view->surfaceSelector()->setFocus();
        return false;
    }

    return true;
}

Result ObturationPresenter::getResult()
{
    return ObturationData
    {
        view->surfaceSelector()->getSurfaces(),
        view->hasPost(),
        std::get<0>(view->getColor()),
        view->material()->getText()
    };
}


std::string ObturationPresenter::autoDiagnosis(const Tooth& tooth)
{
    bool secondaryCaries = 0;
    std::string cariesDiagnosis;


    for (int i = 0; i < 6; i++)		//checking if somewhere obturation is present also, returning secondary caries
    {
        if (tooth.caries.exists(i) && tooth.obturation.exists(i))
        {
            secondaryCaries = 0;
        }
    }

    for (int i = 0; i < 6; i++)		//getting the diagnosis of the first caries found
    {
        if (tooth.caries.exists()) {
            cariesDiagnosis = tooth.caries[i].getDiagnosis();
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
        tooth.fracture.getDiagnosis(),
        tooth.root.getDiagnosis(),
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
        if (utils.getToothType(tooth.index) == ToothType::Frontal)
        {
            surfaces[static_cast<int>(Surface::Lingual)] = true;
        }
        else surfaces[static_cast<int>(Surface::Occlusal)] = true;
    }

    if (tooth.fracture.exists())
    {
        if (utils.getToothType(tooth.index) == ToothType::Frontal)
        {
            surfaces[static_cast<int>(Surface::Occlusal)] = true;
        }
        else surfaces[static_cast<int>(Surface::Lingual)] = true;
    }
    else if (tooth.obturation.exists())
    {
        for (int i = 0; i < 6; i++)
        {
            surfaces[i] = tooth.obturation.exists(i);
        }
    }

    return surfaces;
}
