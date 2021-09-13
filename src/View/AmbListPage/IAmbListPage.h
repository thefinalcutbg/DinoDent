#pragma once

#include "TabView/ITabView.h"
#include <string>
#include <map>


class IAmbListPage
{
	
public:

	virtual ITabView* tabView() = 0;

};