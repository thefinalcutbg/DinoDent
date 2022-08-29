#pragma once

class ILoginView;

#include <optional>
#include "Model/UserStructs.h"
#include "View/Interfaces/ILoginView.h"

class LoginPresenter
{
	ILoginView* view{ nullptr };
	bool loginSuccessful{ false };
public:

	bool successful();
	void setView(ILoginView* view);
	void okPressed(const std::string& lpk, const  std::string& pass);
	
};

