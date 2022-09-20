#include "PrescriptionValidators.h"

#include "Model/Prescription/Medication.h"

bool MedicationNameValidator::validateInput(const std::string& text)
{
    return Medication::isValidName(text);
}


#include "Model/Prescription/Route.h"

bool RouteFormValidator::validateInput(const std::string& text)
{
    return Route::isValidStr(text);
}

#include "Model/Prescription/WhenToTake.h"

bool WhenTextValidator::validateInput(const std::string& text)
{
    return WhenToTake::isValidValue(text);
}
