#pragma once

class ITabView;

class IMainView
{
	
public:

	virtual ITabView* tabView() = 0;

};