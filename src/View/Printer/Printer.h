#pragma once
struct Patient;
struct AmbList;
struct Invoice;

#include "Model/User/UserManager.h"

namespace Print
{
	void ambList(const AmbList& amb, const Patient& patient, const User& user = UserManager::currentUser());
	void invoice(const Invoice& inv);
	void ambList(const User& user = UserManager::currentUser());
}

