#pragma once

#include <vector>
#include <array>
#include <fstream>

#include "jsoncpp/json/json.h"

#include "ManipulationTemplate.h"

std::vector<ManipulationTemplate> getNZOK(int age, int doctor_code, bool unfav);
std::vector<ManipulationTemplate> getCustomManipulations();
