#pragma once

#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/User/CurrentUser.h"

namespace Print
{
	void ambList(const AmbList& amb, const Patient& patient, const User& user = CurrentUser::instance());
	void ambList(const User& user = CurrentUser::instance());
}

