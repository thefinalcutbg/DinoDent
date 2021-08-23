#pragma once

#include "JsonCpp/json.h"
#include <array>
#include "Tooth.h"

class ToothContainer;

class ToothParser
{
	struct Material { std::string LPK; std::string material; int index{ -1 }; };

	Json::FastWriter writer;

	Json::Value writePathology(int index, const Pathology &pathology);
	Json::Value writeDentistMade(int index, const DentistMade& procedure);


public:

	std::string write(const ToothContainer& teeth);
    void parse(const std::string& jsonString, ToothContainer& teeth);
};
