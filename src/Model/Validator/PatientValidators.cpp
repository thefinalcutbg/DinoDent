#include "PatientValidators.h"

bool EgnValidator::dateCheck(int& d, int& m, int& y)
{
    if (m < 1 || m > 12) return 0;

    if (d > Date::getMaxDayOfMonth(m, y) || d < 1)
    {
        return false;
    }

    if (Date(d, m, y) > Date::CurrentDate())
    {
        return false;
    }

    return true;
}

bool EgnValidator::validate(const std::string& text)
{
    const std::string& egn = text;

    if (egn.size() < 10) return false;

    //check the first 6 digits:
    int y = stoi(egn.substr(0, 2));
    int m = stoi(egn.substr(2, 2));
    int d = stoi(egn.substr(4, 2));
    //check if month is correct:
    if (m < 1 || (m > 12 && m < 41) || m > 52) return false;
    //converting year and month to full numbers:
   // if (m > 20 && m < 33) { y = y + 1800; m = m - 20; }
    if (m <= 12) { y = y + 1900; }
    else if (m >= 41) { y = y + 2000; m = m - 40; }
    if (!dateCheck(d, m, y)) return false;

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

bool Ln4Validator::validate(const std::string& text)
{
    const std::string& ln4 = text;

    //length check:
    if (ln4.length() != 10)
        return 0;

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

bool HIRBNoValidator::validate(const std::string& text)
{
    const std::string& hirbNo = text;

    if (hirbNo.length() == 0)
        return true;
    else if (hirbNo.length() != 8)
        return false;
    else if (hirbNo[0] != '0')
        return false;

    return true;
}

NameValidator::NameValidator() : letters("абвгдежзийклмнопрстуфхцчшщъьюя-АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЮЯ ")
{}

bool NameValidator::validate(const std::string& text)
{

    if (text.empty() || text.length() == 1) return false;

    QString name = text.c_str();

    bool foundletter;

    bool onlyspaces = true;
    for (int i = 0; i < text.size(); i++)
    {
        if (text[i] != ' ' && text[i] != '-') { onlyspaces = false; break; }
        onlyspaces = true;
    }
    if (onlyspaces) return false;

    for (int i = 0; i < name.size(); i++)
    {
        foundletter = false;

        for (int y = 0; y < 63; y++)
            if (name[i] == letters[y]) { foundletter = true; break; }


        if (foundletter == false) return false;
    }

    return true;
}

CityValidator::CityValidator()
{
    std::ifstream infile("cities.txt");
    std::string line;

    int maxCharLength = 0;

    while (std::getline(infile, line))
        cityCheckMap[line] = true;

    infile.close();
}

bool CityValidator::validate(const std::string& text)
{
    if (cityCheckMap[text])
        return true;

    return false;
}

BirthValidator::BirthValidator()
{}

bool BirthValidator::validate(const std::string& text)
{
    Date date(text);
    if (date == defaultDate) return false;

    return Date::CurrentDate() > date;
}
