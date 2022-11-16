#include "TimeFrame.h"
#include "Model/User.h"

std::string TimeFrame::getFrameName() const
{
    std::string result;

    switch (type) {
    case TimeFrameType::InitialAmb:
        result += "Амбулаторен лист №" + number + " (изходен статус)";
        break;
    case TimeFrameType::Procedures:
        result += "Амбулаторен лист №" + number + " (манипулации)";
        break;
    case TimeFrameType::Perio:
        result += "Пародонтално измерване";
        break;
    }
    return result;
}
