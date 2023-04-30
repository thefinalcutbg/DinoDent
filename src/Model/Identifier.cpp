#include "Identifier.h"
#include <algorithm>
#include "Model/Date.h"


Identifier::Identifier(const std::string& id)
{
    if (egnIsValid(id)) {
        m_id = id;
        is_egn = true;
        return;
    }

    if (lnchIsValid(id)) {
        m_id = id;
        is_egn = false;
    }
}

Identifier::Identifier(const std::string& id, PersonType t)
{
    if (t != PersonType::EGN && t != PersonType::LNCH) return;

    m_id = id;
    is_egn = t == PersonType::EGN;
}


bool Identifier::egnIsValid(const std::string& id)
{
    const std::string& egn = id;

    if (egn.size() != 10) return false;

    if (!std::all_of(egn.begin(), egn.end(), ::isdigit)) return false;

    //check the first 6 digits:
    int y = stoi(egn.substr(0, 2));
    int m = stoi(egn.substr(2, 2));
    int d = stoi(egn.substr(4, 2));
    //check if month is correct:
    if (m < 1 || (m > 12 && m < 41) || m > 52) return false;
    //converting year and month to full numbers:
    if (m <= 12) { y = y + 1900; }
    else if (m >= 41) { y = y + 2000; m = m - 40; }

    if(Date(d, m, y) > Date::currentDate()) return false;

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

bool Identifier::lnchIsValid(const std::string& id)
{
    const std::string& lnch = id;

    if (lnch.size() != 10) return false;

    if (!std::all_of(lnch.begin(), lnch.end(), ::isdigit)) return false;

    //last 4 digits check:
    int weigthctrl[9] = { 21,19,17,13,11,9,7,3,1 };
    int idnum[9];
    int ctrlsum = 0;

    for (int i = 0; i < 9; i++)
    {
        idnum[i] = stoi(lnch.substr(i, 1));
        idnum[i] *= weigthctrl[i];

        ctrlsum += idnum[i];
    }
    ctrlsum %= 10;

    int id10 = stoi(lnch.substr(9, 1));
    if (ctrlsum > 9 && id10 != 0) return 0;
    if (ctrlsum < 10 && ctrlsum != id10) return 0;
    return 1;
}
