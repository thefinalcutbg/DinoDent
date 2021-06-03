#pragma once

#include <vector>
#include <array>
#include <type_traits>

#include "Model/CheckState.h"
#include "Model/Tooth/Tooth.h"

class CheckModelCreator
{

    static std::array<bool, 18> generalStatusBool(const Tooth& tooth);
    static std::array<bool, 6> obturationStatusBool(const Tooth& tooth);
    static std::array<bool, 6> cariesStatusBool(const Tooth& tooth);

    template <typename Function, size_t Size = std::tuple_size<typename std::invoke_result<Function, const Tooth &>::type>::value>
    std::array<CheckState, Size> getModelArray(const std::vector<Tooth*>& teeth, Function function);

public:
    CheckModelCreator();

    CheckModel refreshModel(const std::vector<Tooth*>& selectedTeeth);
};

template <typename Function, size_t Size>
inline std::array<CheckState, Size> CheckModelCreator::getModelArray(const std::vector<Tooth*>& teeth, Function function)
{

    std::array<CheckState, Size> checkArray{ CheckState::unchecked };

    for (int toothNum = 0; toothNum < teeth.size(); toothNum++)
    {
        std::array<bool, Size> boolArray = function(*teeth[toothNum]);

        if (toothNum == 0) //first iteration;
        {
            for (int i = 0; i < Size; i++)
            {
                if (boolArray[i]) {
                    checkArray[i] = CheckState::checked;
                }
            }
            continue;
        }

        for (int i = 0; i < Size; i++) //further iterations;
        {
            if (checkArray[i] == CheckState::partially_checked) {
                continue;
            }
            else if (int(checkArray[i]) != boolArray[i]) {
                checkArray[i] = CheckState::partially_checked;
            }

        }
    }

    return checkArray;
}
