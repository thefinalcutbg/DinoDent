#pragma once

class ILoginView;

#include <optional>
#include "Model/UserStructs.h"
#include "View/Interfaces/ILoginView.h"
#include "Database/DbDoctor.h"

class LoginPresenter
{

	std::vector<DbDoctor::UserCredentials> m_users;
	ILoginView* view{ nullptr };
	bool loginSuccessful{ false };
	int m_currentIndex = -1;

public:

	LoginPresenter();

	bool successful();
	void practiceListPressed();
	void setView(ILoginView* view);
	void userIdxChanged(int idx);
	void okPressed(const std::string& pass, bool remember);
	void login(const std::string& lpk);
};

