#pragma once

#include "TabView/ITabView.h"
#include <string>
#include <map>

enum class DialogAnswer{Yes, No, Cancel};

class IAmbListPage
{
	
public:

	virtual int openSaveAsDialog(int defaultNum, std::map<int, bool>) = 0;
	virtual DialogAnswer openSaveDialog(std::string title) = 0;
	virtual ITabView* tabView() = 0;

};