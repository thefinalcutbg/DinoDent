#pragma once
#include <string>

struct SMSMessage
{
    static constexpr const char* expectedFormat = "yyyy-MM-ddTHH:mm";

	SMSMessage() = default;
    SMSMessage(
        const std::string& phone,
        const std::string& msgTemplate,
        const std::string& appointmentDateTime,
		int hoursBefore = 0
    );

    std::string phone;
    std::string message;
    std::string toDate;
};