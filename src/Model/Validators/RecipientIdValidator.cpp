#include "RecipientIdValidator.h"

bool RecipientIdValidator::validateInput(const std::string& text)
{
    if (bulstat_validator.validateInput(text)) return true;

    if (egn_validator.validateInput(text)) return true;

    if (ln4_validator.validateInput(text)) return true;

    return false;
}
