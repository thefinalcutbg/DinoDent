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
		const std::string& identifier,
		const std::string& phone
	) : name(name), address(address), identifier(identifier), phone(phone) {}

	//NZOK recipient
	Recipient(const Practice& practice);

	//Patient recipient
	Recipient(const Patient& patient);

	std::string name;
	std::string address;
	std::string identifier;
	std::string phone;
	bool hasVat{ false };

	std::string getVat() const;

	static bool isNhifBulstat(const std::string& bulstat);

};