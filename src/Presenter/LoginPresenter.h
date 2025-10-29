#pragma once

class ILoginView;

#include <optional>
#include "Model/UserStructs.h"
#include "Database/DbDoctor.h"

class LoginView;

class LoginPresenter
{
	std::vector<DbDoctor::UserCredentials> m_users;
	LoginView* view{ nullptr };
	bool loginSuccessful{ false };
	int m_currentIndex = -1;

public:

	LoginPresenter();

	bool successful();
	void practiceListPressed();
	void setView(LoginView* view);
	void userIdxChanged(int idx);
	void okPressed(const std::string& pass, bool remember);
	void login(const std::string& lpk);
};

