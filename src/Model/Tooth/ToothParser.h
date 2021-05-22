#pragma once

#include "../jsoncpp/jsoncpp-src-0.5.0/include/json/json.h"
#include <array>
#include "Tooth.h"

#include <QDebug>

class ToothParser
{
	Json::FastWriter writer;

	Json::Value writePathology(int index, const Pathology &pathology);
	Json::Value writeDentistMade(int index, const DentistMade& procedure);
	Json::Value writeMaterial(int index, const Material& procedure);

public:

	std::string write(const std::array<Tooth, 32>& teeth);
    void parse(const std::string& jsonString, std::array<Tooth, 32>& teeth);
};
