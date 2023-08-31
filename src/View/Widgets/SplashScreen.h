#pragma once
#include <string>

namespace SplashScreen
{
	void createAndShow();
	void showMessage(const std::string& message);
	void hideAndDestroy();
}