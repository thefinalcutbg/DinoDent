#pragma once

#include "PerioStatus.h"

struct PerioToothData
{
    int toothIndex;
    int mobility;
    int furc[3];

    int recession[2];
    int attachment[2];

    int pd[6];
    int cal[6];
    int gm[6];

    bool bop[6];
    bool fmbs[4];
    bool fmps[4];

    PerioToothData(const PerioStatus& status, int idx);
};