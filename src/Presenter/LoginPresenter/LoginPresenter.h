#pragma once

class ILoginView;

#include <optional>
#include "Model/User/User.h"
#include "View/LoginView/ILoginView.h"
#include "Database/DbLogin.h"

class LoginPresenter
{
	DbLogin db;
	ILoginView* view{ nullptr };
	std::vector<Practice> practiceList;
	bool loginSuccessful{ false };
public:

	bool successful();
	void setView(ILoginView* view);
	void okPressed(const std::string& lpk, const  std::string& pass, int practiceIdx);
	
};

