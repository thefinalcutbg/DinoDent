#include "NameValidator.h"


bool NameValidator::validateInput(const std::string& text)
{
    if (!not_empty.validateInput(text))
    {
        _errorMsg = &not_empty.getErrorMessage();
        return false;
    }

    if (!cyrillic_only.validateInput(text))
    {
        _errorMsg = &mustBeInCyrillic;
        return false;
    }

    //    if (text.length() == 1) not working with cyrillic characters!
    //    {
    //        _errorMsg = &invalidName;
    //        return false;
    //    }

    for (char c : text)
    {
        if (c != ' ' && c != '-')
        {
            return true;
        }
    }

    _errorMsg = &invalidName;
    return false;
};