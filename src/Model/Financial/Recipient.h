#pragma once
#include <string>

struct Patient;
struct Practice;

struct Recipient
{
	Recipient() {};

	Recipient(
		const std::string& name,
		const std::string& address,
		const std::string& bulstat,
		const std::string& phone
	) : name(name), address(address), bulstat(bulstat), phone(phone) {}

	//NZOK recipient
	Recipient(const Practice& practice);

	//Patient recipient
	Recipient(const Patient& patient);

	std::string name;
	std::string address;
	std::string bulstat;
	std::string phone;

	static bool isNhifBulstat(const std::string& bulstat);

};