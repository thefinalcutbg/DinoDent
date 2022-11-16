#include "CommonValidators.h"

const std::string NotEmptyValidator::couldntBeEmpty{ "Това поле е задължително" };

NotEmptyValidator::NotEmptyValidator()
{
    _errorMsg = &couldntBeEmpty;
}

bool NotEmptyValidator::validateInput(const std::string& text)
{
    return text.size() > 0;
}

const std::string CyrillicValidator::mustBeCyrillic{ "Пишете на кирилица, ако обичате!" };
static std::string_view allowed = ("абвгдежзийклмнопрстуфхцчшщъьюя-АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЮЯ ");

bool CyrillicValidator::validateInput(const std::string& text)
{
    bool found = false;

    for (auto& c : text)
    {
        found = false;

        for (auto& a : allowed)
        {
            if (c == a)
            {
                found = true;
                break;
            }
        }

        if (!found) return false;

    };

    return true;
}

const std::string mustBeNumber{ "Това поле може да съдържа само цифри" };

bool DigitsOnlyValidator::validateInput(const std::string& text)
{
    _errorMsg = &mustBeNumber;

    for (char c : text)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }

    return true;
}
