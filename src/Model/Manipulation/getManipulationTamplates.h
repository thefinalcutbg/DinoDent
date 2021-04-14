#pragma once

#include <vector>
#include <array>
#include <fstream>

#include "c:/Dev/TorqueMVP/jsoncpp/jsoncpp-src-0.5.0/include/json/json.h"

#include "ManipulationTemplate.h"

std::vector<ManipulationTemplate> getNZOK(int age, int doctor_code, bool unfav);
std::vector<ManipulationTemplate> getCustomManipulations();