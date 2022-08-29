#include "TimeFrame.h"
#include "Model/User.h"

std::string TimeFrame::getFrameName() const
{
    std::string result;

    switch (type) {
    case TimeFrameType::InitialAmb:
        result += u8"Амбулаторен лист №" + number + " (изходен статус)";
        break;
    case TimeFrameType::Procedures:
        result += u8"Амбулаторен лист №" + number + " (манипулации)";
        break;
    case TimeFrameType::Perio:
        result += u8"Пародонтално измерване";
        break;
    }
    return result;
}
