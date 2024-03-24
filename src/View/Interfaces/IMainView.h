#pragma once

#include <string>

class ITabView;

class IMainView
{
	
public:
	bool m_loggedIn{ false };
	virtual ITabView* tabView() = 0;
	virtual void setUserLabel(const std::string& doctorName, const std::string& practiceName) = 0;
	virtual void exitProgram() = 0;

	virtual void disconnectChat() = 0;
	virtual void connectChat(const std::string& fname, const std::string lname) = 0;
	virtual void changeUsrName(const std::string& fname, const std::string lname) = 0;

};