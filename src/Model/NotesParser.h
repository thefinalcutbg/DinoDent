#pragma once

#include "JsonCpp/json.h"

#include <array>

class NotesParser
{
	Json::FastWriter writer;
public:
	std::string write(const std::array<std::string, 32>& notes);
	void parse(const std::string& jsonString, std::array<std::string, 32>& notes);
};