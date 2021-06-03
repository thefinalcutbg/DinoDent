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

    case StatusCode::Temporary:  //temporary tooth;
        if (tooth_type == ToothType::Molar) return;

        tooth->temporary.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->post.set(false);
        break;

    case StatusCode::Obturation:  //obturation
    {
        tooth->obturation.set(true);
        tooth->root.set(false);
        tooth->implant.set(false);
        tooth->extraction.set(false);
    }
    break;

    case StatusCode::Caries: //caries
    {

        tooth->caries.set(true);
        tooth->implant.set(false);
        tooth->extraction.set(false);
        tooth->root.set(false);
    }
    break;

    case StatusCode::Pulpitis:  //pulpitis
        tooth->pulpitis.set(true);
        tooth->lesion.set(false);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->endo.set(false);
        tooth->post.set(false);

        break;

    case StatusCode::EndoTreatment: //endo treatment

        tooth->endo.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->pulpitis.set(false);
        break;

    case StatusCode::Post: //radicular post

        tooth->post.set(true);
        tooth->endo.set(true);
        tooth->temporary.set(false);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        tooth->pulpitis.set(false);
        break;


    case StatusCode::Extraction: //extraction

        if (tooth->hyperdontic.exists()) {
            tooth->hyperdontic.set(false);
            break;
        }

        if (tooth->temporary.exists()) {
            tooth->temporary.set(false);
            break;
        }

        tooth->extraction.set(true);
        tooth->obturation.set(false);
        tooth->caries.set(false);
        tooth->implant.set(false);
        tooth->pulpitis.set(false);
        tooth->endo.set(false);
        tooth->fracture.set(false);
        tooth->root.set(false);
        tooth->lesion.set(false);
        tooth->periodontitis.set(false);
        tooth->crown.set(false);
        tooth->post.set(false);
        tooth->mobility.set(false);
        break;

    case StatusCode::Root: //root

        tooth->root.set(true);
        tooth->caries.set(false);
        tooth->obturation.set(false);
        tooth->crown.set(false);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        break;

    case StatusCode::Implant: //implant

        tooth->implant.set(true);
        tooth->extraction.set(false);
        tooth->obturation.set(false);
        tooth->caries.set(false);
        tooth->hyperdontic.set(false);
        tooth->pulpitis.set(false);
        tooth->endo.set(false);
        tooth->fracture.set(false);
        tooth->root.set(false);
        tooth->post.set(false);
        tooth->mobility.set(false);

        break;

    case StatusCode::ApicalLesion: //apical lesion

        tooth->lesion.set(true);
        tooth->pulpitis.set(false);
        tooth->extraction.set(false);

        break;

    case StatusCode::Fracture:
        tooth->fracture.set(true);
        tooth->extraction.set(false);
        tooth->implant.set(false);
        break;

    case StatusCode::Periodontitis: //Periodontitis
        tooth->periodontitis.set(true);
        tooth->extraction.set(false);

        break;
    case StatusCode::Mobility1:
        tooth->extraction.set(false);
        tooth->mobility.set(true);
        tooth->mobility.degree = Degree::First;
        break;

    case StatusCode::Mobility2:
        tooth->extraction.set(false);
        tooth->mobility.set(true);
        tooth->mobility.degree = Degree::Second;
        break;

    case StatusCode::Mobility3:
        tooth->extraction.set(false);
        tooth->mobility.set(true);
        tooth->mobility.degree = Degree::Third;
        break;

    case StatusCode::Crown:
        tooth->crown.set(true);
        tooth->bridge.set(false);
        tooth->extraction.set(false);
        tooth->root.set(false);
        break;

    case StatusCode::Bridge:
        tooth->bridge.set(true);
        tooth->bridge.LPK.clear();
        tooth->hyperdontic.set(false);
        tooth->crown.set(false);
        break;

    case StatusCode::Dsn:
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
    case StatusCode::Temporary:
        tooth->temporary.set(false);
        break;
    case StatusCode::Obturation:
        tooth->obturation.set(false);
        break;
    case StatusCode::Caries:
        tooth->caries.set(false);
        break;
    case StatusCode::Pulpitis:
        tooth->pulpitis.set(false);
        break;
    case StatusCode::EndoTreatment:
        tooth->endo.set(false);
        tooth->post.set(false);
        break;
    case StatusCode::Post:
        tooth->post.set(false);
        break;
    case StatusCode::ApicalLesion:
        tooth->lesion.set(false);
        break;
    case StatusCode::Extraction:
        tooth->extraction.set(false);
        break;
    case StatusCode::Root:
        tooth->root.set(false);
        break;
    case StatusCode::Mobility1:
    case StatusCode::Mobility2:
    case StatusCode::Mobility3:
        tooth->mobility.set(false);
        break;
    case StatusCode::Implant:
        tooth->implant.set(false);
        break;
    case StatusCode::Fracture:
        tooth->fracture.set(false);
        break;
    case StatusCode::Crown:
        tooth->crown.set(false);
        break;
    case StatusCode::Bridge:
        tooth->bridge.set(false);
        break;
    case StatusCode::Periodontitis:
        tooth->periodontitis.set(false);
        break;
    case StatusCode::Dsn:
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