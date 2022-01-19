#include "ObturationPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Tooth/ToothUtils.h"

ObturationPresenter::ObturationPresenter(const std::vector<Tooth*>& selectedTeeth) :
    selectedTeeth(selectedTeeth),
    view(nullptr)
{
    
}

void ObturationPresenter::setView(IObturationView* view)
{
    this->view = view;

    view->surfaceSelector()->setInputValidator(&surf_validator);

    if (selectedTeeth.size() == 1)
    {
        m_diagnosis = getDiagnosis(*selectedTeeth.front());
        view->surfaceSelector()->setSurfaces(autoSurfaces(*selectedTeeth.front()));

    }
}

void ObturationPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
    bool noTeethSelected = !selectedTeeth.size();
    common_view->set_hidden(noTeethSelected);
    view->set_hidden(noTeethSelected);
    if(noTeethSelected) return;


    AbstractSubPresenter::setProcedureTemplate(m);

    auto data = view->getData();
    data.data.material = m.material;
    view->setData(data);
}

bool ObturationPresenter::isValid()
{
    if (!AbstractSubPresenter::isValid()) return false;

    view->surfaceSelector()->validateInput();

    if (!view->surfaceSelector()->isValid())
    {
        return ModalDialogBuilder::askDialog(u8"Резултатът от манипулацията няма да бъде приложен към статуса, "
            "тъй като не сте избрали повърхност. Желаете ли да продължите въпреки това?");
    }

    return true;
}

std::vector<Procedure> ObturationPresenter::getProcedures()
{
    std::vector<Procedure> procedures;
    procedures.reserve(selectedTeeth.size());

    for (auto& tooth : selectedTeeth) {
        procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
        procedures.back().result = view->getData();
        procedures.back().tooth = tooth->index;
        procedures.back().temp = tooth->temporary.exists();
    }

    return procedures;
}


std::string ObturationPresenter::getDiagnosis(const Tooth& tooth)
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
