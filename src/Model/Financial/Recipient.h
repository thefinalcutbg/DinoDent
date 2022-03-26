#pragma once
#include <string>

struct Patient;

struct Recipient
{
	Recipient() {};
	//NZOK recipient
	Recipient(int practiceRhif);

	//Patient recipient
	Recipient(const Patient& patient);

	std::string name;
	std::string address;
	std::string bulstat;
	std::string phone;

};