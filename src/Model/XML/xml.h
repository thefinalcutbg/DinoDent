#pragma once
#include <string>
#include "Model/Date.h"

namespace XML
{
	void saveXMLfile(const Date& from, const Date& to, std::string path);
};
