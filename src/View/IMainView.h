#pragma once

#include <string>

class ITabView;

class IMainView
{
	
public:
	bool m_initialized{ false };
	virtual ITabView* tabView() = 0;
	virtual void setDoctor(const std::string& name) = 0;
	virtual void exitProgram() = 0;

};