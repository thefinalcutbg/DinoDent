#pragma once
#include <string>
#include <array>
#include "Model/Date.h"
#include "Model/Tooth/Enums.h"

class Status
{
protected:
	bool m_exists;

	const std::string* default_attribute;

public:

	Status() : m_exists(0), default_attribute(NULL) {};
	virtual bool exists() const { return m_exists; };
	virtual void set(bool exists) { this->m_exists = exists; };

	void setDefaultAttribute(const std::string& default)
	{
		default_attribute = &default;
	}

};



class Pathology : public Status
{

public:
	std::string diagnosis;
	std::string date_diagnosed;


	virtual void set(bool exists)
	{
		m_exists = exists;
		if (exists && date_diagnosed.empty()) {
			date_diagnosed = Date::toString(Date::getCurrentDate());
		}
	}

	std::string getDiagnosis() const
	{
		if (!diagnosis.empty())
		{
			return diagnosis;
		}

		if (default_attribute != NULL)
		{
			return *default_attribute;
		}

		return diagnosis;
	}
};


struct DentistMade : public Status
{
	std::string material;
	std::string LPK;
	
	std::string getMaterial()
	{
		if (!material.empty())
		{
			return material;
		}

		if (default_attribute != NULL)
		{
			return *default_attribute;
		}

		return material;
	}

};


struct Bridge : public DentistMade
{
	BridgePos position;
};

struct Mobility : public Status
{
	Mobility() : degree(Degree::First) {}
	Degree degree;
};
