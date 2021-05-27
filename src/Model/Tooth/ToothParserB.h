#pragma once

#include "../json/json.h"
#include "Model/Tooth/Tooth.h"

class ToothParserB
{
    Json::FastWriter writer;

    Json::Value getToothJson(const Tooth& t);

public:
    std::string write(const std::array<Tooth, 32>& teeth);
    void parse(const std::string& jsonString, std::array<Tooth, 32>& teeth);
};

