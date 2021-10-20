#include "PerioToothData.h"
#include "PerioStatus.h"
#include <algorithm>

PerioToothData::PerioToothData(const PerioStatus& status, int idx)
{
    toothIndex = idx;
    mobility = status.mobility[idx];

    for (int i = 0; i < 3; i++)
    {
        furc[i] = 0; // for now!
    }

    attachment[0] = status.ag[idx];
    attachment[1] = status.ag[idx + 32];

    for (int i = 0; i < 3; i++)
    {
        pd[i] = status.pd[idx*3 + i];
        pd[i + 3] = status.pd[idx*3 + i + lingualOffset];

        cal[i] = status.cal[idx * 3 + i];
        cal[i + 3] = status.cal[idx * 3 + i + lingualOffset];

        bop[i] = status.bop[idx * 3 + i];
        bop[i + 3] = status.bop[idx * 3 + i + 3];
    }

    for (int i = 0; i < 6; i++) {
        gm[i] = pd[i] - cal[i];
    }

    for (int i = 0; i < 4; i++)
    {
        fmps[i] = status.FMPS[idx*4 + i];
        fmbs[i] = status.FMBS[idx * 4 + i];
    }

    recession[0] = 0;
    recession[1] = 0;

    for (int i = 0; i < 3; i++)
    {
        recession[0] = std::max(recession[0], gm[i] * (-1));
        recession[1] = std::max(recession[1], gm[i+3] * (-1));
    }

}
