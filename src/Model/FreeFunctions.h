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
    std::string LocalToUTC(const std::string& timeStamp);
    std::string escapeXml(const std::string& data);
    std::string listToString(const std::vector<std::string>& list, const std::string& emptyListValue = {});
    bool contains(const std::string& parent, const std::string& child);

};