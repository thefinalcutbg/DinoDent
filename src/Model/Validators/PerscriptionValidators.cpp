#include "PerscriptionValidators.h"

#include "Model/Perscription/Medication.h"

bool MedicationNameValidator::validateInput(const std::string& text)
{
    return Medication::isValidName(text);
}


#include "Model/Perscription/Route.h"

bool RouteFormValidator::validateInput(const std::string& text)
{
    return Route::isValidStr(text);
}

#include "Model/Perscription/WhenToTake.h"

bool WhenTextValidator::validateInput(const std::string& text)
{
    return WhenToTake::isValidValue(text);
}
