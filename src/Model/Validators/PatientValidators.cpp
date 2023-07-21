#include "PatientValidators.h"

const std::string EgnValidator::invalid{ "Невалиден ЕГН" };

EgnValidator::EgnValidator()
{
    date_validator.setMaxDate(Date::currentDate());
    date_validator.setMinDate(Date(2, 1, 1900));
    _errorMsg = &invalid;
}

bool EgnValidator::validateInput(const std::string& text)
{
    const std::string& egn = text;

    if (!not_empty.validateInput(text))
    {
        _errorMsg = &not_empty.getErrorMessage();
        return false;
    }

    _errorMsg = &invalid;

    if (!digits_only.validateInput(text)) return false;

    if (egn.size() < 10) return false;

    //check the first 6 digits:
    int y = stoi(egn.substr(0, 2));
    int m = stoi(egn.substr(2, 2));
    int d = stoi(egn.substr(4, 2));
    //check if month is correct:
    if (m < 1 || (m > 12 && m < 41) || m > 52) return false;
    //converting year and month to full numbers:
    if (m <= 12) { y = y + 1900; }
    else if (m >= 41) { y = y + 2000; m = m - 40; }
    if (!date_validator.validateInput(Date(d, m, y))) return false;

    //last 4 digits check:
    int weigthctrl[9] = { 2, 4, 8, 5, 10, 9, 7, 3, 6 };
    int idnum[9]{ 0 };
    int ctrlsum = 0;

    for (int i = 0; i < 9; i++)
    {
        idnum[i] = stoi(egn.substr(i, 1)) * weigthctrl[i];
        ctrlsum += idnum[i];
    }
    int ctrl = ctrlsum % 11;

    int ctrl10 = stoi(egn.substr(9, 1));

    if (ctrl > 9 && ctrl10 != 0) return false;
    if (ctrl < 10 && ctrl != ctrl10) return false;

    return true;
}

const std::string Ln4Validator::invalid{ "Невалиден ЛНЧ" };

Ln4Validator::Ln4Validator()
{
    _errorMsg = &invalid;
}

bool Ln4Validator::validateInput(const std::string& text)
{
    const std::string& ln4 = text;

    if (!not_empty.validateInput(text))
    {
        _errorMsg = &not_empty.getErrorMessage();
        return false;
    }

    _errorMsg = &invalid;

    if (!digits_only.validateInput(text)) return false;

    if (ln4.size() < 10) return false;


    //last 4 digits check:
    int weigthctrl[9] = { 21,19,17,13,11,9,7,3,1 };
    int idnum[9];
    int ctrlsum = 0;

    for (int i = 0; i < 9; i++)
    {
        idnum[i] = stoi(ln4.substr(i, 1));
        idnum[i] *= weigthctrl[i];

        ctrlsum += idnum[i];
    }
    ctrlsum %= 10;

    int id10 = stoi(ln4.substr(9, 1));
    if (ctrlsum > 9 && id10 != 0) return 0;
    if (ctrlsum < 10 && ctrlsum != id10) return 0;
    return 1;
}


const std::string SSNValidator::invalid{ "Невалиден ССН" };

SSNValidator::SSNValidator()
{
    _errorMsg = &invalid;
}

bool SSNValidator::validateInput(const std::string& text)
{
    if (!notEmpty_validator.validateInput(text)) return false;

    if (!digits_only_validator.validateInput(text)) return false;

    return true;
}


const std::string HIRBNoValidator::invalid{ "Невалиден номер на здравна книжка" };

HIRBNoValidator::HIRBNoValidator()
{
    _errorMsg = &invalid;
}

bool HIRBNoValidator::validateInput(const std::string& text)
{
    const std::string& hirbNo = text;

    if (!digits_only.validateInput(hirbNo)) return false;

    if (hirbNo.length() == 0)
        return true;
    else if (hirbNo.length() != 8)
        return false;
    else if (hirbNo[0] != '0' && hirbNo[0] != '1')
        return false;

    return true;
}

const std::string CityValidator::invalidCity{ "Невалидно населено място" };

#include "Model/Ekatte.h"

bool CityValidator::validateInput(const std::string& text)
{
    return Ekatte::isValidStr(text);
}
