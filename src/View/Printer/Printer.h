#pragma once

#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/User/UserManager.h"

namespace Print
{
	void ambList(const AmbList& amb, const Patient& patient, const User& user = UserManager::currentUser());
	void ambList(const User& user = UserManager::currentUser());
}

