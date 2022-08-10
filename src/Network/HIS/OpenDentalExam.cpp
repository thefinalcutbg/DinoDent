#include "OpenDentalExam.h"

bool OpenDentalExam::sendRequest(std::function<void(const std::string&)> callback)
{
    callback("2218900DECAF");
    return true;
}
