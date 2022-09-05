#include "PerscriptionValidators.h"

#include "Model/Perscription/Medication.h"

bool MedicationNameValidator::validateInput(const std::string& text)
{
    return Medication::isValidName(text);
}

#include "Model/Perscription/DoseQuantity.h"

bool DosageFormValidator::validateInput(const std::string& text)
{
    return DoseQuantity::isValidFormName(text);
}

#include "Model/Perscription/Route.h"

bool RouteFormValidator::validateInput(const std::string& text)
{
    return Route::isValidStr(text);
}
