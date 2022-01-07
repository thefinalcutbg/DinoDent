#pragma once

class ITabView;

class IMainView
{
	
public:
	bool m_initialized{ false };
	virtual ITabView* tabView() = 0;

};