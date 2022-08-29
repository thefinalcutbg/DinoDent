#pragma once

#include <string>

class ITabView;

class IMainView
{
	
public:
	bool m_initialized{ false };
	virtual ITabView* tabView() = 0;
	virtual void setUserLabel(const std::string& doctorName, const std::string& practiceName) = 0;
	virtual void exitProgram() = 0;

};