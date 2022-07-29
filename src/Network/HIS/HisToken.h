#pragma once

#include "HisService.h"

namespace HisToken
{
	/*
	 	Can save only up to 1 service per token request.
		Returns true if the requester and its query are saved,
		and will be executed IF the token is recieved.

	*/
	bool requestToken(HisService* requester, const std::string& query);
	
	//returns empty string if no token or token expired
	const std::string& getToken();

	//called by the network manager when token reply arrives
	void setAuthRepy(const std::string& reply);

	//called by the network manager, when challenge arrives
	void setChallengeMessage(const std::string& challenge);

	
	void nullifyToken();

}

