#include "IRCInterface.h"
#include "IRC.h"

IRC& IRCInterface::getClient() {

	static IRC client;

	return client;
}


void IRCInterface::connectAs(const std::string& firstName, const std::string& lastName)
{
	getClient().setNames(firstName, lastName);
}

void IRCInterface::disconnect()
{
	getClient().disconnect();
}

void IRCInterface::setInvisible(bool invisible)
{
	getClient().setInvisible(invisible);
}
