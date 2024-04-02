#pragma once
#include <string>

class IRC;

namespace IRCInterface {

	IRC& getClient();

	//use these 3 functions to control the mirc chat
	void connectAs(const std::string& firstName, const std::string& lastName);
	void setVisible(bool visible);
	void disconnect();

}