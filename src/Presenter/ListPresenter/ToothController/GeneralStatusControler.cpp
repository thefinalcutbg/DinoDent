#include "GeneralStatusControler.h"


GeneralStatusController::GeneralStatusController() : tooth_type{ ToothType::Molar }
{

    
}

void GeneralStatusController::setTooth(Tooth* tooth)
{
    tooth_type = utils.getToothType(tooth->index);
    ToothController::setTooth(tooth);
}

void GeneralStatusController::addStatus(int statusIndex)
{
    switch (statusIndex)
    {

    case int(StatusCode::Temporary):  //temporary tooth;
        if (tooth_type == ToothType::Molar) return;

        tooth->temporary.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->post.set(false);
        break;

    case int(StatusCode::Obturation):  //obturation
    {
        bool noSurfaces = true;

        for (int i = 0; i < tooth->o_surf.size(); i++) {

            if (tooth->o_surf[i].exists()) {
                noSurfaces = false; break;
            }
        }

        if (noSurfaces && tooth_type == ToothType::Frontal) {

            tooth->o_surf[static_cast<int>(Surface::Buccal)].set(true);
        }
        else if (noSurfaces) tooth->o_surf[static_cast<int>(Surface::Occlusal)].set(true);

        tooth->obturation = true;
        tooth->root.set(false); //it can't be radix, implant or extracted;
        tooth->implant.set(false);
        tooth->extraction.set(false);
    }
    break;

    case int(StatusCode::Caries): //caries
    {
        bool noSurfaces = true;

        for (int i = 0; i < tooth->c_surf.size(); i++) {
            if (tooth->c_surf[i].exists()) {
                noSurfaces = false; break;
            }
        }


        if (noSurfaces && tooth_type == ToothType::Frontal) {
            tooth->c_surf[static_cast<int>(Surface::Buccal)].set(true);
        }

        else if (noSurfaces) {
            tooth->c_surf[static_cast<int>(Surface::Occlusal)].set(true);
        }

        tooth->caries = true;
        tooth->implant.set(false);
        tooth->extraction.set(false);
        tooth->root.set(false);
    }
    break;

    case int(StatusCode::Pulpitis):  //pulpitis
        tooth->pulpitis.set(true);
        tooth->lesion.set(false);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->endo.set(false);
        tooth->post.set(false);

        break;

    case int(StatusCode::EndoTreatment): //endo treatment

        tooth->endo.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->pulpitis.set(false);
        break;

        case int(StatusCode::Post) : //radicular post
        {
            tooth->post.set(true);
            tooth->endo.set(true);
            tooth->temporary.set(false);
            tooth->extraction.set(false);
            tooth->implant.set(false);
            tooth->pulpitis.set(false);
            break;
        }

    case int(StatusCode::Extraction): //extraction

        if (tooth->hyperdontic.exists()) {
            tooth->hyperdontic.set(false);
            break;
        }

        if (tooth->temporary.exists()) {
            tooth->temporary.set(false);
            break;
        }

        tooth->extraction.set(true);
        tooth->obturation = false;
        tooth->caries = false;
        tooth->implant.set(false);
        tooth->pulpitis.set(false);
        tooth->endo.set(false);
        tooth->fracture.set(false);
        tooth->root.set(false);
        tooth->lesion.set(false);
        tooth->periodontitis.set(false);
        tooth->crown.set(false);
        tooth->post.set(false);


        break;

    case int(StatusCode::Root): //root

        tooth->root.set(true);
        tooth->caries = false;
        tooth->obturation = false;
        tooth->crown.set(false);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        break;

    case int(StatusCode::Implant): //implant

        tooth->implant.set(true);
        tooth->extraction.set(false);
        tooth->obturation = false;
        tooth->caries = false;
        tooth->hyperdontic.set(false);
        tooth->pulpitis.set(false);
        tooth->endo.set(false);
        tooth->fracture.set(false);
        tooth->root.set(false);
        tooth->post.set(false);

        break;

    case int(StatusCode::ApicalLesion): //apical lesion

        tooth->lesion.set(true);
        tooth->pulpitis.set(false);
        tooth->extraction.set(false);

        break;

    case int(StatusCode::Fracture):
        tooth->fracture.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        break;

    case int(StatusCode::Periodontitis): //Periodontitis
        tooth->periodontitis.set(true);
        tooth->extraction.set(false);

        break;
    case int(StatusCode::Mobility1):
        tooth->extraction.set(false);
        tooth->mobility.set(true);
        tooth->mobility.degree = Degree::First;
        break;

    case int(StatusCode::Mobility2):
        tooth->extraction.set(false);
        tooth->mobility.set(true);
        tooth->mobility.degree = Degree::Second;
        break;

    case int(StatusCode::Mobility3):
        tooth->extraction.set(false);
        tooth->mobility.set(true);
        tooth->mobility.degree = Degree::Third;
        break;

    case int(StatusCode::Crown):
        tooth->crown.set(true);
        tooth->bridge.set(false);
        tooth->extraction.set(false);
        tooth->root.set(false);
        break;

    case int(StatusCode::Bridge):
        tooth->bridge.set(true);
        tooth->hyperdontic.set(false);
        tooth->crown.set(false);
        break;
    case int(StatusCode::Dsn):
        tooth->hyperdontic.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->crown.set(false);
        tooth->bridge.set(false);
        break;
    }
}

void GeneralStatusController::removeStatus(int statusIndex)
{
    switch (statusIndex)
    {
    case int(StatusCode::Temporary):
        tooth->temporary.set(false);
        break;
    case int(StatusCode::Obturation):
        tooth->obturation = false;
        break;
    case int(StatusCode::Caries):
        tooth->caries = false;
        break;
    case int(StatusCode::Pulpitis):
        tooth->pulpitis.set(false);
        break;
    case int(StatusCode::EndoTreatment):
        tooth->endo.set(false);
        tooth->post.set(false);
        break;
    case int(StatusCode::Post) :
        tooth->post.set(false);
        break;
    case int(StatusCode::ApicalLesion):
        tooth->lesion.set(false);
        break;
    case int(StatusCode::Extraction):
        tooth->extraction.set(false);
        break;
    case int(StatusCode::Root):
        tooth->root.set(false);
        break;
    case int(StatusCode::Mobility1):
    case int(StatusCode::Mobility2):
    case int(StatusCode::Mobility3):
        tooth->mobility.set(false);
        break;
    case int(StatusCode::Implant):
        tooth->implant.set(false);
        break;
    case int(StatusCode::Fracture):
        tooth->fracture.set(false);
        break;
    case int(StatusCode::Crown):
        tooth->crown.set(false);
        break;
    case int(StatusCode::Bridge):
        tooth->bridge.set(false);
        break;
    case int(StatusCode::Periodontitis):
        tooth->periodontitis.set(false);
        break;
    case int(StatusCode::Dsn):
        tooth->hyperdontic.set(false);
        break;

    }
}

void GeneralStatusController::removeStatus()
{
    for (int i = 0; i < 17; i++) {
        removeStatus(i);
    }

}