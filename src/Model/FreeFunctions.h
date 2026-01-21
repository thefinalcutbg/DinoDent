#pragma once
#include <string>
#include <vector>

namespace FreeFn {

    std::string formatDouble(const double& price);
    std::string leadZeroes(long long num, int totalLength);
    std::string leadZeroes(const std::string& number, int totalLength);
    std::string getFile(const std::string& filepath);
    std::string removeTrailing(double a);
    std::string getUuid();
    std::string getTimeStampLocal();
    std::string getTimeStampUTC();
    std::string UTCToLocal(const std::string& timeStamp);
    std::string LocalToUTC(const std::string& timeStamp);
    std::string escapeXml(const std::string& data);
    std::string toUpper(const std::string& src);
    bool contains(const std::string& parent, const std::string& child);
    std::string getPatientName(const std::string& fname, const std::string& mname, const std::string& lname);
	std::string getPhoneFromString(const std::string& text);
    bool sendSMS(long long patientRowid);
};